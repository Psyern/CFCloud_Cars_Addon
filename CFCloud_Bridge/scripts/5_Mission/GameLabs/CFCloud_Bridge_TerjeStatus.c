#ifdef GAMELABS
#ifdef TERJE_CORE_MOD

// Read-only. Lists every Terje stat the server knows, with the current value
// and the maximum, keyed by the ID that "Terje-Zustand setzen" takes. Result
// reaches the server log and the webhook only - the CFCloud interface shows
// nothing but a queued acknowledgement.
class CFCloud_Bridge_TerjeStatus extends CFCloud_Bridge_TerjeActionBase
{
	void CFCloud_Bridge_TerjeStatus()
	{
		actionCode = "CFCloudBridge_TerjeStatus";
		actionName = "Terje-Zustand anzeigen";
		actionIcon = "heartbeat";
		actionColour = "info";
		actionContext = "player";
	}

	override bool Execute(GameLabsActionContext context)
	{
		PlayerBase player;
		if (!CFCloud_Bridge_ResolvePlayer(context, player))
			return false;

		string headline = "Terje-Zustand von " + CFCloud_Bridge_PlayerLabel(player) + ":";
		return CFCloud_Bridge_Outcome(true, headline, CFCloud_Bridge_DescribeStats(player));
	}
}

#endif
#endif
