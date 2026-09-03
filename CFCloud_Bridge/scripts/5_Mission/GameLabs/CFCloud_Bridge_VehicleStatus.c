#ifdef GAMELABS
#ifdef EXPANSIONMODVEHICLE

// Read-only. Note that its result reaches the server log only - the CFCloud
// interface shows nothing but a queued acknowledgement, because GameLabs marks
// the "output" response type as unavailable for general use.
class CFCloud_Bridge_VehicleStatus extends CFCloud_Bridge_VehicleActionBase
{
	void CFCloud_Bridge_VehicleStatus()
	{
		actionCode = "CFCloudBridge_VehicleStatus";
		actionName = "Fahrzeug-Status";
		actionIcon = "info-circle";
		actionColour = "info";
		actionContext = "vehicle";
	}

	override bool Execute(GameLabsActionContext context)
	{
		Object obj;
		ExpansionVehicle vehicle;

		if (!CFCloud_Bridge_Resolve(context, obj, vehicle))
			return false;

		// NOT named referenceKey: GameLabsContextAction already has a member of
		// that name, and shadowing it makes the compiler warn
		// "Possible variable name conflict".
		_Vehicle glVehicle;
		string glRefKey = "unknown";
		if (_Vehicle.CastTo(glVehicle, context.GetReferencedObject()))
			glRefKey = glVehicle.GetID();

		string text = "Status: " + CFCloud_Bridge_Describe(vehicle, obj);
		text = text + " masterKey=" + vehicle.GetMasterKeyPersistentIDString();
		text = text + " glRef=" + glRefKey;

		CFCloud_Bridge_Logger.Warning(text);
		return true;
	}
}

#endif
#endif
