#ifdef GAMELABS
#ifdef EXPANSIONMODBASEBUILDING

// Adds an online player to the territory or changes their rank.
//
// The player has to be online: members are stored under
// PlayerIdentity.GetId(), and only a connected identity yields that value.
// The target comes from the online-player dropdown (see
// CFCloud_Bridge_PlayerDropdown) or a typed SteamID64.
//
// Expansion's member and territory limits are NOT applied - this is an admin
// override, the same way Expansion lets holders of the COT permission
// "Expansion.Territories.Edit" bypass them.
class CFCloud_Bridge_TerritorySetMember extends CFCloud_Bridge_TerritoryActionBase
{
	void CFCloud_Bridge_TerritorySetMember()
	{
		actionCode = "CFCloudBridge_TerritorySetMember";
		actionName = "Territorium: Mitglied hinzufuegen / Rang setzen";
		actionIcon = "user-plus";
		actionColour = "warning";
		CFCloud_Bridge_UseFlagContext();

		GameLabsActionParameter rank = new GameLabsActionParameter("Rang", "Rang im Territorium.", "options");
		CFCloud_Bridge_AddRank(rank, ExpansionTerritoryRank.MEMBER, "Mitglied");
		CFCloud_Bridge_AddRank(rank, ExpansionTerritoryRank.MODERATOR, "Moderator");
		CFCloud_Bridge_AddRank(rank, ExpansionTerritoryRank.ADMIN, "Admin");

		parameters.Insert("target", CFCloud_Bridge_PlayerDropdown.CreateParameter("Spieler", "Spieler, der Mitglied wird (alle Spieler online)."));
		parameters.Insert("targetSteamId", GameLabsActionParameter("SteamID64 (optional)", "Ueberschreibt die Auswahl, falls der Spieler in der Liste noch fehlt.", "string"));
		parameters.Insert("rank", rank);
	}

	protected void CFCloud_Bridge_AddRank(GameLabsActionParameter param, int rank, string displayName)
	{
		GameLabsActionParameter option = new GameLabsActionParameter(displayName, "", "string");
		option.valueString = rank.ToString();
		param.options.Insert(rank.ToString(), option);
	}

	override bool Execute(GameLabsActionContext context)
	{
		TerritoryFlag flag;
		ExpansionTerritory territory;
		if (!CFCloud_Bridge_ResolveFlag(context, flag, territory))
			return false;

		CFCloud_Bridge_Settings settings = CFCloud_Bridge_Manager.GetInstance().GetSettings();
		if (!settings.m_AllowTerritoryMembers)
			return CFCloud_Bridge_Outcome(false, "Territorium abgelehnt: Mitgliederverwaltung in Settings.json deaktiviert.", CFCloud_Bridge_DescribeFlag(flag, territory));

		if (!territory)
			return CFCloud_Bridge_Outcome(false, "Territorium abgelehnt: die Flagge hat kein Expansion-Territorium.", CFCloud_Bridge_DescribeFlag(flag, territory));

		string steamId = "";
		GameLabsActionParameter overrideParam = CFCloud_Bridge_Param(context, "targetSteamId");
		if (overrideParam)
			steamId = overrideParam.GetString().Trim();

		if (steamId == "")
		{
			GameLabsActionParameter targetParam = CFCloud_Bridge_Param(context, "target");
			if (targetParam)
				steamId = targetParam.GetString().Trim();
		}

		if (steamId == "")
			return CFCloud_Bridge_Outcome(false, "Territorium abgelehnt: kein Spieler gewaehlt.", CFCloud_Bridge_DescribeFlag(flag, territory));

		PlayerBase target = CFCloud_Bridge_FindOnlinePlayer(steamId);
		if (!target)
			return CFCloud_Bridge_Outcome(false, "Territorium abgelehnt: kein Spieler mit SteamID64 " + steamId + " online.", CFCloud_Bridge_DescribeFlag(flag, territory));

		int rank = ExpansionTerritoryRank.MEMBER;
		GameLabsActionParameter rankParam = CFCloud_Bridge_Param(context, "rank");
		if (rankParam && rankParam.GetString() != "")
			rank = rankParam.GetString().ToInt();

		if (rank < ExpansionTerritoryRank.MEMBER || rank > ExpansionTerritoryRank.ADMIN)
			return CFCloud_Bridge_Outcome(false, "Territorium abgelehnt: unbekannter Rang " + rank.ToString() + ".", CFCloud_Bridge_DescribeFlag(flag, territory));

		PlayerIdentity identity = target.GetIdentity();
		string uid = identity.GetId();
		string label = CFCloud_Bridge_PlayerLabel(target);

		// The owner's rank is what makes them the owner in Expansion's menus.
		// Changing it would leave an owner who cannot manage their own base.
		if (uid == territory.GetOwnerID())
			return CFCloud_Bridge_Outcome(false, "Territorium abgelehnt: " + label + " ist der Besitzer, sein Rang bleibt Admin.", CFCloud_Bridge_DescribeFlag(flag, territory));

		string headline;
		ExpansionTerritoryMember member = territory.GetMember(uid);
		if (member)
		{
			string oldRank = CFCloud_Bridge_RankName(member.GetRank());
			member.SetRank(rank);
			headline = "Rang von " + label + " geaendert: " + oldRank + " -> " + CFCloud_Bridge_RankName(rank) + ".";
		}
		else
		{
			territory.AddMember(uid, identity.GetName(), false);
			member = territory.GetMember(uid);
			if (member)
				member.SetRank(rank);

			// A pending invite would still show up in the player's menu.
			if (territory.HasInvite(uid))
				territory.RemoveTerritoryInvite(uid);

			headline = label + " als " + CFCloud_Bridge_RankName(rank) + " hinzugefuegt.";
		}

		ExpansionTerritoryModule module = CFCloud_Bridge_Module();
		if (module)
		{
			module.UpdateClient(territory.GetTerritoryID());
			module.SyncPlayerInvitesServer(identity);
		}

		return CFCloud_Bridge_Outcome(true, headline, CFCloud_Bridge_DescribeFlag(flag, territory));
	}
}

// Removes a member - online or offline - from the territory. Offline members
// are matched by UID or name (both listed by the info action), online ones
// also by SteamID64. The owner cannot be removed; delete the territory
// instead.
class CFCloud_Bridge_TerritoryRemoveMember extends CFCloud_Bridge_TerritoryActionBase
{
	void CFCloud_Bridge_TerritoryRemoveMember()
	{
		actionCode = "CFCloudBridge_TerritoryRemoveMember";
		actionName = "Territorium: Mitglied entfernen";
		actionIcon = "user-minus";
		actionColour = "danger";
		CFCloud_Bridge_UseFlagContext();

		parameters.Insert("member", GameLabsActionParameter("Mitglied", "Name, UID (aus 'Territorium: Info') oder SteamID64 eines Spielers online.", "string"));
	}

	override bool Execute(GameLabsActionContext context)
	{
		TerritoryFlag flag;
		ExpansionTerritory territory;
		if (!CFCloud_Bridge_ResolveFlag(context, flag, territory))
			return false;

		CFCloud_Bridge_Settings settings = CFCloud_Bridge_Manager.GetInstance().GetSettings();
		if (!settings.m_AllowTerritoryMembers)
			return CFCloud_Bridge_Outcome(false, "Territorium abgelehnt: Mitgliederverwaltung in Settings.json deaktiviert.", CFCloud_Bridge_DescribeFlag(flag, territory));

		if (!territory)
			return CFCloud_Bridge_Outcome(false, "Territorium abgelehnt: die Flagge hat kein Expansion-Territorium.", CFCloud_Bridge_DescribeFlag(flag, territory));

		string query = "";
		GameLabsActionParameter memberParam = CFCloud_Bridge_Param(context, "member");
		if (memberParam)
			query = memberParam.GetString().Trim();

		if (query == "")
			return CFCloud_Bridge_Outcome(false, "Territorium abgelehnt: kein Mitglied angegeben.", CFCloud_Bridge_DescribeFlag(flag, territory));

		ExpansionTerritoryMember member = CFCloud_Bridge_FindMember(territory, query);
		if (!member)
			return CFCloud_Bridge_Outcome(false, "Territorium abgelehnt: '" + query + "' ist kein Mitglied.", "Mitglieder:\n" + CFCloud_Bridge_DescribeMembers(territory));

		string uid = member.GetID();
		string memberLabel = member.GetName() + " (" + uid + ")";

		if (uid == territory.GetOwnerID())
			return CFCloud_Bridge_Outcome(false, "Territorium abgelehnt: " + memberLabel + " ist der Besitzer und kann nicht entfernt werden.", CFCloud_Bridge_DescribeFlag(flag, territory));

		int territoryID = territory.GetTerritoryID();
		territory.RemoveMember(member);

		ExpansionTerritoryModule module = CFCloud_Bridge_Module();
		if (module)
			module.UpdateClient(territoryID);

		// Expansion's own kick sends this to the kicking player instead of the
		// kicked one (ExpansionTerritoryModule.c:1568). Sent to the right one.
		PlayerBase removed = PlayerBase.GetPlayerByUID(uid);
		if (removed && removed.GetIdentity())
			CFCloud_Bridge_ClearClient(territoryID, removed.GetIdentity());

		return CFCloud_Bridge_Outcome(true, memberLabel + " aus dem Territorium entfernt.", CFCloud_Bridge_DescribeFlag(flag, territory));
	}
}

#endif
#endif
