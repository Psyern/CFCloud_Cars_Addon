#ifdef GAMELABS

// Teleports the context player (X, picked in CFCloud's player list) to
// another online player (Y, picked from a dropdown of everyone online).
//
// The dropdown is filled by CFCloud_Bridge_PlayerDropdown - read its header
// for how and when CFCloud gets to see the current list. Because that list can
// lag behind, the action also takes a SteamID64 directly; when set, it wins
// over the dropdown. Either way Y is looked up again at execution time.
//
// Needs nothing but GameLabs: the move goes through GameLabs' own
// PlayerBase.SetPositionEx() (4_World/4_Hooking/!PlayerBase.c:262), which also
// resets its speed-hack tracker so the jump is not reported as a speed hack -
// the same call GameLabs' built-in teleport uses.
class CFCloud_Bridge_PlayerTeleportToPlayer extends CFCloud_Bridge_PlayerActionBase
{
	void CFCloud_Bridge_PlayerTeleportToPlayer()
	{
		actionCode = "CFCloudBridge_TeleportToPlayer";
		actionName = "Zu Spieler teleportieren";
		actionIcon = "people-arrows";
		actionColour = "warning";
		actionContext = "player";

		parameters.Insert("target", CFCloud_Bridge_PlayerDropdown.CreateParameter("Ziel-Spieler", "Der Spieler, zu dem teleportiert wird (alle Spieler online)."));
		parameters.Insert("targetSteamId", GameLabsActionParameter("Ziel-SteamID64 (optional)", "Ueberschreibt die Auswahl, falls der Ziel-Spieler in der Liste noch fehlt.", "string"));
	}

	override bool Execute(GameLabsActionContext context)
	{
		PlayerBase player;
		if (!CFCloud_Bridge_ResolvePlayer(context, player))
			return false;

		string label = CFCloud_Bridge_PlayerLabel(player);

		CFCloud_Bridge_Settings settings = CFCloud_Bridge_Manager.GetInstance().GetSettings();
		if (!settings.m_AllowTeleportToPlayer)
		{
			return CFCloud_Bridge_Outcome(false, "Teleport abgelehnt: in Settings.json deaktiviert.", label);
		}

		string targetId = "";
		GameLabsActionParameter overrideParam = CFCloud_Bridge_Param(context, "targetSteamId");
		if (overrideParam)
			targetId = overrideParam.GetString().Trim();

		if (targetId == "")
		{
			GameLabsActionParameter targetParam = CFCloud_Bridge_Param(context, "target");
			if (targetParam)
				targetId = targetParam.GetString().Trim();
		}

		if (targetId == "")
		{
			return CFCloud_Bridge_Outcome(false, "Teleport abgelehnt: kein Ziel-Spieler gewaehlt.", label);
		}

		PlayerBase target = CFCloud_Bridge_FindOnlinePlayer(targetId);
		if (!target)
		{
			return CFCloud_Bridge_Outcome(false, "Teleport abgelehnt: kein Spieler mit SteamID64 " + targetId + " online.", label);
		}

		if (target == player)
		{
			return CFCloud_Bridge_Outcome(false, "Teleport abgelehnt: Spieler und Ziel sind dieselbe Person.", label);
		}

		// Moving a player who sits in a vehicle only moves the body out from
		// under the vehicle's command and desyncs the seat. Refuse instead.
		if (player.IsInVehicle())
		{
			return CFCloud_Bridge_Outcome(false, "Teleport abgelehnt: " + label + " sitzt in einem Fahrzeug.", "Ziel: " + CFCloud_Bridge_PlayerLabel(target));
		}

		vector destination = target.GetPosition();

		// Y in a vehicle: land next to it on the ground rather than inside it.
		if (target.IsInVehicle())
		{
			destination[0] = destination[0] + 3.0;
			destination[1] = g_Game.SurfaceY(destination[0], destination[2]);
		}

		vector origin = player.GetPosition();
		player.SetPositionEx(destination);

		string detail = "Von " + origin.ToString() + " nach " + destination.ToString() + ".";
		return CFCloud_Bridge_Outcome(true, label + " zu " + CFCloud_Bridge_PlayerLabel(target) + " teleportiert.", detail);
	}
}

#endif
