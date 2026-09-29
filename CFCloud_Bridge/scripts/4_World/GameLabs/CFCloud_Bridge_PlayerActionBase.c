#ifdef GAMELABS

// Shared plumbing for every player-context action in this mod, whatever mod
// it drives (Terje, Expansion, vanilla): resolve the GameLabs context to an
// online PlayerBase, label it for the log, and find another online player by
// SteamID64. Sorts after "ActionBase" and before "TerjeActionBase", so both
// its parent and its subclasses in this folder compile in the right order.
class CFCloud_Bridge_PlayerActionBase extends CFCloud_Bridge_ActionBase
{
	protected bool CFCloud_Bridge_ResolvePlayer(GameLabsActionContext context, out PlayerBase player)
	{
		player = null;

		if (!PlayerBase.CastTo(player, context.GetReferencedObject()))
		{
			CFCloud_Bridge_Logger.Error(actionCode + ": context is not a player.");
			return false;
		}

		if (!player.GetIdentity())
		{
			CFCloud_Bridge_Logger.Error(actionCode + ": player has no identity.");
			return false;
		}

		return true;
	}

	protected string CFCloud_Bridge_PlayerLabel(PlayerBase player)
	{
		PlayerIdentity identity = player.GetIdentity();
		if (!identity)
			return player.GetType();

		return identity.GetName() + " (" + identity.GetPlainId() + ")";
	}

	//! Online, alive player with this SteamID64, or null. Bodies of players
	//! that are logging out still have their identity for a few seconds, so
	//! the caller gets whoever the engine still lists - same rule GameLabs'
	//! own GLGetPlayerBySteam64() applies to the order's context player.
	static PlayerBase CFCloud_Bridge_FindOnlinePlayer(string steamId)
	{
		if (steamId == "")
			return null;

		array<Man> players = new array<Man>;
		g_Game.GetPlayers(players);

		foreach (Man candidate : players)
		{
			PlayerBase player = PlayerBase.Cast(candidate);
			if (!player)
				continue;

			PlayerIdentity identity = player.GetIdentity();
			if (!identity)
				continue;

			if (identity.GetPlainId() == steamId)
				return player;
		}

		return null;
	}
}

#endif
