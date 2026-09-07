#ifdef GAMELABS
#ifdef TERJE_CORE_MOD

// Shared plumbing for the Terje player-state actions.
//
// Everything goes through TerjeCore's admin-tool bridge,
// TerjeCore/Scripts/4_World/Compatibility/TerjeAdmintoolSupport.c. Each Terje
// module registers its player states there as a named stat with a min/max
// range and a Get/SetValue pair - Medicine (sleep, mind, wounds, diseases),
// Radiation (accumulated dose, sickness), Skills (one stat per skill) and
// StartScreen (souls). VPP and COT drive Terje through exactly this API, so
// this mod does the same instead of touching TerjePlayerStats directly: one
// code path covers every module, and modules that are not loaded simply
// register nothing.
//
// Only TERJE_CORE_MOD is required. The IDs an admin can use depend on which
// Terje modules run on the server - the status action lists them.
class CFCloud_Bridge_TerjeActionBase extends CFCloud_Bridge_ActionBase
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
		return identity.GetName() + " (" + identity.GetPlainId() + ")";
	}

	//! "terjeSleep=1200/1500" - the ID is what the set action expects.
	protected string CFCloud_Bridge_FormatStat(TerjeAdmintoolSupport_PlayerStat stat, PlayerBase player)
	{
		float value = stat.GetValue(player);
		float max = stat.GetMax();
		return stat.GetId() + "=" + value.ToString() + "/" + max.ToString();
	}

	protected string CFCloud_Bridge_DescribeStats(PlayerBase player)
	{
		array<ref TerjeAdmintoolSupport_PlayerStat> stats = new array<ref TerjeAdmintoolSupport_PlayerStat>;
		GetTerjeAdmintoolSupport().GetPlayerStats(stats);

		if (stats.Count() == 0)
			return "no Terje stats registered";

		string text = "";
		foreach (TerjeAdmintoolSupport_PlayerStat stat : stats)
		{
			if (text != "")
				text = text + ", ";

			text = text + CFCloud_Bridge_FormatStat(stat, player);
		}

		return text;
	}
}

#endif
#endif
