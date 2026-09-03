#ifdef GAMELABS
#ifdef EXPANSIONMODVEHICLE

// Player context. Walks GameLabs' own vehicle registry rather than the world,
// because that registry is the single source the CFCloud interface shows - and
// thanks to CFCloud_Bridge_ExpansionVehicleBase it now contains Expansion
// aircraft too, so there is nothing to merge or de-duplicate.
//
// Read-only, so the result reaches the server log only.
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

		CFCloud_Bridge_Logger.Warning("Vehicles of " + identity.GetName() + " (" + identity.GetPlainId() + "):");

		array<ref _Vehicle> tracked = GetGameLabs().GetVehicles();
		int matches = 0;

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
			CFCloud_Bridge_Logger.Warning("  " + CFCloud_Bridge_Describe(vehicle, obj));
		}

		CFCloud_Bridge_Logger.Warning("Total: " + matches.ToString() + " of " + tracked.Count().ToString() + " tracked vehicles.");
		return true;
	}
}

#endif
#endif
