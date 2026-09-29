#ifdef GAMELABS
#ifdef EXPANSIONMODBASEBUILDING

// Raises the flag to the top and resets the lifetime of everything within
// the territory radius - what a player refreshing the flag achieves, without
// anyone having to stand at it.
//
// Vanilla AddRefresherTime01() clamps to the maximum and turns the refresher
// on, but SetRefresherActive() only acts on a state change (Expansion
// TerritoryFlag.c:133), so an already active flag would not reset the CE
// lifetime. The radius reset is therefore called directly, with Expansion's
// configured territory size - the radius Expansion itself uses there.
class CFCloud_Bridge_TerritoryRefresh extends CFCloud_Bridge_TerritoryActionBase
{
	void CFCloud_Bridge_TerritoryRefresh()
	{
		actionCode = "CFCloudBridge_TerritoryRefresh";
		actionName = "Territorium: Flagge auffrischen";
		actionIcon = "sync-alt";
		actionColour = "success";
		CFCloud_Bridge_UseFlagContext();
	}

	override bool Execute(GameLabsActionContext context)
	{
		TerritoryFlag flag;
		ExpansionTerritory territory;
		if (!CFCloud_Bridge_ResolveFlag(context, flag, territory))
			return false;

		CFCloud_Bridge_Settings settings = CFCloud_Bridge_Manager.GetInstance().GetSettings();
		if (!settings.m_AllowTerritoryRefresh)
			return CFCloud_Bridge_Outcome(false, "Auffrischen abgelehnt: in Settings.json deaktiviert.", CFCloud_Bridge_DescribeFlag(flag, territory));

		float before = Math.Round(flag.GetRefresherTime01() * 100);

		flag.AddRefresherTime01(1.0);

		// The flag phase is inverted: 0 is the top of the pole.
		flag.AnimateFlag(0);

		float radius = GameConstants.REFRESHER_RADIUS;
		if (GetExpansionSettings() && GetExpansionSettings().GetTerritory())
			radius = GetExpansionSettings().GetTerritory().TerritorySize;

		GetCEApi().RadiusLifetimeReset(flag.GetPosition(), radius);

		return CFCloud_Bridge_Outcome(true, "Flagge aufgefrischt (vorher " + before.ToString() + "%, Radius " + radius.ToString() + " m).", CFCloud_Bridge_DescribeFlag(flag, territory));
	}
}

// Deletes the territory and its flag through Expansion's own admin path,
// Exec_DeleteTerritoryAdmin(id, null) - the call TerritoryFlag.EEDelete makes.
// With a null sender it skips the permission check, tells every online member
// the territory is gone, clears pending invites, deletes the flag and, with
// the Garage territory mode on, releases stored vehicles. Buildings stay;
// GameLabs' own "Clear territory" is the action that removes those too.
//
// Off by default: it cannot be undone.
class CFCloud_Bridge_TerritoryDelete extends CFCloud_Bridge_TerritoryActionBase
{
	void CFCloud_Bridge_TerritoryDelete()
	{
		actionCode = "CFCloudBridge_TerritoryDelete";
		actionName = "Territorium: loeschen";
		actionIcon = "trash-alt";
		actionColour = "danger";
		CFCloud_Bridge_UseFlagContext();
	}

	override bool Execute(GameLabsActionContext context)
	{
		TerritoryFlag flag;
		ExpansionTerritory territory;
		if (!CFCloud_Bridge_ResolveFlag(context, flag, territory))
			return false;

		string description = CFCloud_Bridge_DescribeFlag(flag, territory);

		CFCloud_Bridge_Settings settings = CFCloud_Bridge_Manager.GetInstance().GetSettings();
		if (!settings.m_AllowTerritoryDelete)
			return CFCloud_Bridge_Outcome(false, "Loeschen abgelehnt: in Settings.json deaktiviert.", description);

		if (!territory)
			return CFCloud_Bridge_Outcome(false, "Loeschen abgelehnt: die Flagge hat kein Expansion-Territorium.", description);

		ExpansionTerritoryModule module = CFCloud_Bridge_Module();
		if (!module)
			return CFCloud_Bridge_Outcome(false, "Loeschen abgelehnt: Expansion-Territorium-Modul nicht geladen.", description);

		string members = CFCloud_Bridge_DescribeMembers(territory);
		module.Exec_DeleteTerritoryAdmin(territory.GetTerritoryID(), null);

		return CFCloud_Bridge_Outcome(true, "Territorium geloescht: " + description, "Ehemalige Mitglieder:\n" + members);
	}
}

#endif
#endif
