#ifdef GAMELABS
#ifdef TERJE_CORE_MOD

// One action per Terje stat - the CFCloud counterpart of the slider list
// Terje's COT compatibility module builds (TerjeCompatibilityCOT/Scripts/
// 5_Mission/CotCompatibility.c, JMPlayerForm.InitActionWidgetsStats). Both
// walk the same registry, GetTerjeAdmintoolSupport().GetPlayerStats(), so
// whatever COT can change, one of these actions can change too - including
// stats a newer Terje version adds, without touching this mod.
//
// Instances are created in MissionServer.GLActionRegisterHook(), one per
// registered stat. Reading the registry there is safe: the vanilla
// MissionBase constructor runs PluginManagerInit() (missionBase.c:21), which
// creates and OnInit()s every plugin including PluginTerjeSettings, before
// GameLabs' MissionServer constructor calls the hook (mission.c:208). The
// Terje modules' OnInit() overrides only need those settings.
//
// The stat ID is carried in actionCode rather than in a member: GameLabs
// serialises the whole action object with JsonMakeData() when it posts the
// action list to CFCloud, and an extra field there is an unknown.
class CFCloud_Bridge_TerjeStatAction extends CFCloud_Bridge_TerjeActionBase
{
	static const string ACTION_CODE_PREFIX = "CFCloudBridge_Terje_";

	void CFCloud_Bridge_TerjeStatAction(TerjeAdmintoolSupport_PlayerStat stat)
	{
		float minValue = stat.GetMin();
		float maxValue = stat.GetMax();

		actionCode = ACTION_CODE_PREFIX + stat.GetId();
		actionName = "Terje: " + stat.GetName() + " setzen";
		actionIcon = "sliders-h";
		actionColour = "warning";
		actionContext = "player";

		parameters.Insert("value", GameLabsActionParameter("Wert", stat.GetId() + ", Bereich " + minValue.ToString() + " bis " + maxValue.ToString() + ".", "float"));
	}

	protected string CFCloud_Bridge_StatId()
	{
		int prefixLength = ACTION_CODE_PREFIX.Length();
		return actionCode.Substring(prefixLength, actionCode.Length() - prefixLength);
	}

	override bool Execute(GameLabsActionContext context)
	{
		PlayerBase player;
		if (!CFCloud_Bridge_ResolvePlayer(context, player))
			return false;

		string label = CFCloud_Bridge_PlayerLabel(player);
		string statId = CFCloud_Bridge_StatId();

		CFCloud_Bridge_Settings settings = CFCloud_Bridge_Manager.GetInstance().GetSettings();
		if (!settings.m_AllowTerjeSetStat)
		{
			return CFCloud_Bridge_Outcome(false, "Terje-Zustand abgelehnt: in Settings.json deaktiviert.", label);
		}

		// Re-resolved on every call rather than kept from the constructor, so
		// the min/max range is whatever Terje reports right now.
		TerjeAdmintoolSupport_PlayerStat stat;
		if (!GetTerjeAdmintoolSupport().FindPlayerStat(statId, stat) || !stat)
		{
			return CFCloud_Bridge_Outcome(false, "Terje-Zustand abgelehnt: Stat '" + statId + "' ist nicht mehr registriert.", label);
		}

		GameLabsActionParameter valueParam = CFCloud_Bridge_Param(context, "value");
		if (!valueParam)
		{
			return CFCloud_Bridge_Outcome(false, "Terje-Zustand abgelehnt: kein Wert angegeben.", label);
		}

		float requested = valueParam.GetFloat();
		float minValue = stat.GetMin();
		float maxValue = stat.GetMax();
		float value = Math.Clamp(requested, minValue, maxValue);
		float before = stat.GetValue(player);

		stat.SetValue(player, value);

		float after = stat.GetValue(player);

		string headline = "Terje-Zustand " + statId + " (" + stat.GetName() + ") gesetzt: " + before.ToString() + " -> " + after.ToString() + ".";
		string detail = label;
		if (value != requested)
			detail = detail + " Angefragt " + requested.ToString() + ", begrenzt auf " + value.ToString() + " (Bereich " + minValue.ToString() + "-" + maxValue.ToString() + ").";

		return CFCloud_Bridge_Outcome(true, headline, detail);
	}
}

#endif
#endif
