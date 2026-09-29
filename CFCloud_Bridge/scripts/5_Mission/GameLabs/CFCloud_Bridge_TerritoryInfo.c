#ifdef GAMELABS
#ifdef EXPANSIONMODBASEBUILDING

// Read-only: name, ID, level, owner, flag level and every member with rank
// and UID. The UID is what the remove action accepts for offline members.
class CFCloud_Bridge_TerritoryInfo extends CFCloud_Bridge_TerritoryActionBase
{
	void CFCloud_Bridge_TerritoryInfo()
	{
		actionCode = "CFCloudBridge_TerritoryInfo";
		actionName = "Territorium: Info";
		actionIcon = "info-circle";
		actionColour = "default";
		CFCloud_Bridge_UseFlagContext();
	}

	override bool Execute(GameLabsActionContext context)
	{
		TerritoryFlag flag;
		ExpansionTerritory territory;
		if (!CFCloud_Bridge_ResolveFlag(context, flag, territory))
			return false;

		if (!territory)
			return CFCloud_Bridge_Outcome(true, "Territorium-Info:", CFCloud_Bridge_DescribeFlag(flag, territory));

		return CFCloud_Bridge_Outcome(true, "Territorium-Info: " + CFCloud_Bridge_DescribeFlag(flag, territory), "Mitglieder:\n" + CFCloud_Bridge_DescribeMembers(territory));
	}
}

// Lists every territory the context player belongs to. Loops the server's
// flag map - GetTerritories() and IsPlayerTerritoryMember() read the
// client-only map and return nothing on the server.
class CFCloud_Bridge_PlayerTerritories extends CFCloud_Bridge_TerritoryActionBase
{
	void CFCloud_Bridge_PlayerTerritories()
	{
		actionCode = "CFCloudBridge_PlayerTerritories";
		actionName = "Territorien des Spielers";
		actionIcon = "flag";
		actionColour = "default";
		actionContext = "player";
	}

	override bool Execute(GameLabsActionContext context)
	{
		PlayerBase player;
		if (!CFCloud_Bridge_ResolvePlayer(context, player))
			return false;

		string label = CFCloud_Bridge_PlayerLabel(player);
		string uid = player.GetIdentity().GetId();

		ExpansionTerritoryModule module = CFCloud_Bridge_Module();
		if (!module)
			return CFCloud_Bridge_Outcome(false, "Territorien: Expansion-Territorium-Modul nicht geladen.", label);

		string text = "";
		int found = 0;

		map<int, TerritoryFlag> flags = module.GetAllTerritoryFlags();
		foreach (int territoryID, TerritoryFlag flag : flags)
		{
			if (!flag)
				continue;

			ExpansionTerritory territory = flag.GetTerritory();
			if (!territory)
				continue;

			ExpansionTerritoryMember member = territory.GetMember(uid);
			if (!member)
				continue;

			found = found + 1;
			text = CFCloud_Bridge_AddLine(text, CFCloud_Bridge_DescribeFlag(flag, territory) + " rang=" + CFCloud_Bridge_RankName(member.GetRank()));
		}

		if (found == 0)
			return CFCloud_Bridge_Outcome(true, label + " ist in keinem Territorium.", "UID " + uid);

		return CFCloud_Bridge_Outcome(true, label + " ist in " + found.ToString() + " Territorium/Territorien.", "UID " + uid + "\n" + text);
	}
}

#endif
#endif
