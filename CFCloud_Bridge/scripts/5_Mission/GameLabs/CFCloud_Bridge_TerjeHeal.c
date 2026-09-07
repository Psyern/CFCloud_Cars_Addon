#ifdef GAMELABS
#ifdef TERJE_CORE_MOD

// Resets every Terje condition of an online player at once - the same
// "heal" every Terje module contributes to for VPP/COT: wounds, diseases,
// pain, radiation, and sleep/mind back to their maximum. Vanilla health,
// blood and shock are NOT touched; GameLabs ships its own action for those.
class CFCloud_Bridge_TerjeHeal extends CFCloud_Bridge_TerjeActionBase
{
	void CFCloud_Bridge_TerjeHeal()
	{
		actionCode = "CFCloudBridge_TerjeHeal";
		actionName = "Terje: vollstaendig heilen";
		actionIcon = "medkit";
		actionColour = "success";
		actionContext = "player";
	}

	override bool Execute(GameLabsActionContext context)
	{
		PlayerBase player;
		if (!CFCloud_Bridge_ResolvePlayer(context, player))
			return false;

		string label = CFCloud_Bridge_PlayerLabel(player);

		CFCloud_Bridge_Settings settings = CFCloud_Bridge_Manager.GetInstance().GetSettings();
		if (!settings.m_AllowTerjeHeal)
		{
			return CFCloud_Bridge_Outcome(false, "Terje-Heilung abgelehnt: in Settings.json deaktiviert.", label);
		}

		string before = CFCloud_Bridge_DescribeStats(player);

		GetTerjeAdmintoolSupport().OnHeal(player);

		string after = CFCloud_Bridge_DescribeStats(player);

		return CFCloud_Bridge_Outcome(true, "Terje-Heilung ausgefuehrt fuer " + label + ".", "Vorher: " + before + " || Nachher: " + after);
	}
}

#endif
#endif
