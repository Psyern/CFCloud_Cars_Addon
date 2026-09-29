#ifdef GAMELABS
#ifdef EXPANSIONMODBASEBUILDING

// Shared plumbing for the Expansion territory actions.
//
// The flag is the handle: GameLabs registers every TerritoryFlag as an _Event
// with the "pennant" icon (4_World/4_Hooking/EntityTracking/!TerritoryFlag.c),
// so territory actions use the object context filtered to "TerritoryFlag" -
// the admin clicks the flag on the CFCloud map. GameLabs' own "Clear
// territory" action works the same way.
//
// Expansion keeps the territory data on the flag (TerritoryFlag.GetTerritory(),
// saved with the flag through ModStorage). Every public mutator on
// ExpansionTerritoryModule is a client-side RPC sender, and the server-side
// Exec_* handlers are private and demand a sender of the right rank. So the
// actions change the ExpansionTerritory object directly and then push it to
// the members exactly the way Expansion does (UpdateClient).
//
// Member IDs are PlayerIdentity.GetId() - the hashed DayZ UID - not the
// SteamID64 (ExpansionTerritoryModule.c:405,436).
class CFCloud_Bridge_TerritoryActionBase extends CFCloud_Bridge_PlayerActionBase
{
	protected void CFCloud_Bridge_UseFlagContext()
	{
		actionContext = "object";
		actionContextFilter = new array<string>;
		actionContextFilter.Insert("TerritoryFlag");
	}

	protected bool CFCloud_Bridge_ResolveFlag(GameLabsActionContext context, out TerritoryFlag flag, out ExpansionTerritory territory)
	{
		flag = null;
		territory = null;

		_Event eventInstance;
		if (!_Event.CastTo(eventInstance, context.GetReferencedObject()))
		{
			CFCloud_Bridge_Logger.Error(actionCode + ": context is not a map object.");
			return false;
		}

		if (!TerritoryFlag.CastTo(flag, eventInstance.Ref()))
		{
			CFCloud_Bridge_Logger.Error(actionCode + ": the referenced object is not a territory flag.");
			return false;
		}

		territory = flag.GetTerritory();
		return true;
	}

	protected ExpansionTerritoryModule CFCloud_Bridge_Module()
	{
		return ExpansionTerritoryModule.s_Instance;
	}

	protected string CFCloud_Bridge_RankName(int rank)
	{
		return typename.EnumToString(ExpansionTerritoryRank, rank);
	}

	protected string CFCloud_Bridge_OwnerLabel(ExpansionTerritory territory)
	{
		string ownerId = territory.GetOwnerID();
		ExpansionTerritoryMember owner = territory.GetMember(ownerId);
		if (owner)
			return owner.GetName() + " (" + ownerId + ")";

		return ownerId;
	}

	protected string CFCloud_Bridge_DescribeFlag(TerritoryFlag flag, ExpansionTerritory territory)
	{
		vector position = flag.GetPosition();
		float level = Math.Round(flag.GetRefresherTime01() * 100);

		if (!territory)
			return "Flagge ohne Expansion-Territorium, pos=" + position.ToString() + ", Flaggenstand " + level.ToString() + "%";

		string text = "'" + territory.GetTerritoryName() + "'";
		text = text + " id=" + territory.GetTerritoryID().ToString();
		text = text + " stufe=" + territory.GetTerritoryLevel().ToString();
		text = text + " besitzer=" + CFCloud_Bridge_OwnerLabel(territory);
		text = text + " mitglieder=" + territory.NumberOfMembers().ToString();
		text = text + " flagge=" + level.ToString() + "%";
		text = text + " pos=" + position.ToString();
		return text;
	}

	protected string CFCloud_Bridge_DescribeMembers(ExpansionTerritory territory)
	{
		string text = "";
		array<ref ExpansionTerritoryMember> members = territory.GetTerritoryMembers();
		foreach (ExpansionTerritoryMember member : members)
		{
			text = CFCloud_Bridge_AddLine(text, member.GetName() + " [" + CFCloud_Bridge_RankName(member.GetRank()) + "] " + member.GetID());
		}

		if (text == "")
			return "keine Mitglieder";

		return text;
	}

	//! Finds a member by UID, by SteamID64 of an online player, or by name
	//! (case-insensitive). Offline members can only be found by UID or name -
	//! Expansion does not store their SteamID64.
	protected ExpansionTerritoryMember CFCloud_Bridge_FindMember(ExpansionTerritory territory, string query)
	{
		if (query == "")
			return null;

		ExpansionTerritoryMember byId = territory.GetMember(query);
		if (byId)
			return byId;

		PlayerBase online = CFCloud_Bridge_FindOnlinePlayer(query);
		if (online && online.GetIdentity())
		{
			ExpansionTerritoryMember bySteam = territory.GetMember(online.GetIdentity().GetId());
			if (bySteam)
				return bySteam;
		}

		string wanted = query;
		wanted.ToLower();

		array<ref ExpansionTerritoryMember> members = territory.GetTerritoryMembers();
		foreach (ExpansionTerritoryMember member : members)
		{
			string memberName = member.GetName();
			memberName.ToLower();
			if (memberName == wanted)
				return member;
		}

		return null;
	}

	//! Tells one client that it no longer belongs to this territory - the
	//! same "null territory" update Expansion sends on delete
	//! (ExpansionTerritoryModule.c:618,750). Send_UpdateClient() itself is
	//! private, so the RPC is built through the public Expansion_CreateRPC().
	protected void CFCloud_Bridge_ClearClient(int territoryID, PlayerIdentity identity)
	{
		ExpansionTerritoryModule module = CFCloud_Bridge_Module();
		if (!module || !identity)
			return;

		ExpansionTerritory none = null;

		ExpansionScriptRPC rpc = module.Expansion_CreateRPC("RPC_UpdateClient");
		rpc.Write(territoryID);
		rpc.Write(none);
		rpc.Expansion_Send(true, identity);
	}
}

#endif
#endif
