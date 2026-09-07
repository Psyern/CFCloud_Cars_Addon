#ifdef GAMELABS
#ifdef TERJE_CORE_MOD

// Sets one Terje stat of an online player to an absolute value, addressed
// by its registry ID (terjeSleep, terjeMind, terjeRadiationSick, ...) as a
// plain text parameter.
//
// This is the generic path, mainly for the Data API where the caller knows
// the ID. For the interface, every stat also gets its own action with just
// a value field - see CFCloud_Bridge_TerjeStatAction.c. A dropdown here
// would be friendlier, but GameLabs' "options" data type is only exercised
// by one shipped action (sakhal_bunker_teleport.c) and only with vector
// options; how the backend hands a string option back is not documented.
// The status action prints the valid IDs.
//
// The value is clamped to the stat's own [min, max] range so an admin cannot
// push Terje into a state its own admin tools would refuse.
class CFCloud_Bridge_TerjeSetStat extends CFCloud_Bridge_TerjeActionBase
{
	void CFCloud_Bridge_TerjeSetStat()
	{
		actionCode = "CFCloudBridge_TerjeSetStat";
		actionName = "Terje-Zustand setzen";
		actionIcon = "syringe";
		actionColour = "warning";
		actionContext = "player";

		parameters.Insert("stat", GameLabsActionParameter("Zustand (ID)", "z.B. terjeSleep, terjeMind, terjeRadiationSick, terjeRadiationAccum. Alle IDs: Aktion 'Terje-Zustand anzeigen'.", "string"));
		parameters.Insert("value", GameLabsActionParameter("Wert", "Absoluter Wert. Wird auf den gueltigen Bereich des Zustands begrenzt.", "float"));
	}

	override bool Execute(GameLabsActionContext context)
	{
		PlayerBase player;
		if (!CFCloud_Bridge_ResolvePlayer(context, player))
			return false;

		string label = CFCloud_Bridge_PlayerLabel(player);

		CFCloud_Bridge_Settings settings = CFCloud_Bridge_Manager.GetInstance().GetSettings();
		if (!settings.m_AllowTerjeSetStat)
		{
			return CFCloud_Bridge_Outcome(false, "Terje-Zustand abgelehnt: in Settings.json deaktiviert.", label);
		}

		GameLabsActionParameter statParam = CFCloud_Bridge_Param(context, "stat");
		string statId = "";
		if (statParam)
			statId = statParam.GetString();

		statId = statId.Trim();
		if (statId == "")
		{
			return CFCloud_Bridge_Outcome(false, "Terje-Zustand abgelehnt: keine Zustand-ID angegeben.", label);
		}

		TerjeAdmintoolSupport_PlayerStat stat;
		if (!GetTerjeAdmintoolSupport().FindPlayerStat(statId, stat) || !stat)
		{
			return CFCloud_Bridge_Outcome(false, "Terje-Zustand abgelehnt: unbekannte ID '" + statId + "'.", label + " bekannt: " + CFCloud_Bridge_DescribeStats(player));
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
