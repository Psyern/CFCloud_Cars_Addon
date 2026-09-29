#ifdef GAMELABS
#ifdef EXPANSIONMODVEHICLE

// Player context. Walks GameLabs' own vehicle registry rather than the world,
// because that registry is the single source the CFCloud interface shows - and
// thanks to CFCloud_Bridge_ExpansionVehicleBase it now contains Expansion
// aircraft too, so there is nothing to merge or de-duplicate.
//
// Read-only: one vehicle per line in the result file, the server log and the
// webhook (see CFCloud_Bridge_ActionBase.CFCloud_Bridge_Outcome).
class CFCloud_Bridge_PlayerVehicles extends CFCloud_Bridge_VehicleActionBase
{
	void CFCloud_Bridge_PlayerVehicles()
	{
		actionCode = "CFCloudBridge_PlayerVehicles";
		actionName = "Fahrzeuge dieses Spielers";
		actionIcon = "list";
		actionColour = "info";
		actionContext = "player";
	}

	override bool Execute(GameLabsActionContext context)
	{
		PlayerBase player;
		if (!PlayerBase.CastTo(player, context.GetReferencedObject()))
		{
			CFCloud_Bridge_Logger.Error(actionCode + ": context is not a player.");
			return false;
		}

		PlayerIdentity identity = player.GetIdentity();
		if (!identity)
		{
			CFCloud_Bridge_Logger.Error(actionCode + ": player has no identity.");
			return false;
		}

		string ownerUID = identity.GetId();

		array<ref _Vehicle> tracked = GetGameLabs().GetVehicles();
		int matches = 0;
		string detail = "";

		foreach (_Vehicle glVehicle : tracked)
		{
			Object obj = glVehicle.Ref();
			if (!obj)
				continue;

			ExpansionVehicle vehicle;
			if (!ExpansionVehicle.Get(vehicle, obj))
				continue;

			if (!vehicle.IsOwner(ownerUID))
				continue;

			matches++;
			detail = CFCloud_Bridge_AddLine(detail, CFCloud_Bridge_Describe(vehicle, obj));
		}

		if (matches == 0)
			detail = "keine";

		string headline = identity.GetName() + " (" + identity.GetPlainId() + ") besitzt " + matches.ToString() + " von " + tracked.Count().ToString() + " erfassten Fahrzeugen:";

		return CFCloud_Bridge_Outcome(true, headline, detail);
	}
}

#endif
#endif
