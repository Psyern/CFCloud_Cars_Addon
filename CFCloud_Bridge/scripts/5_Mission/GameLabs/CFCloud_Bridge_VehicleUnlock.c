#ifdef GAMELABS
#ifdef EXPANSIONMODVEHICLE

class CFCloud_Bridge_VehicleUnlock extends CFCloud_Bridge_VehicleActionBase
{
	void CFCloud_Bridge_VehicleUnlock()
	{
		actionCode = "CFCloudBridge_VehicleUnlock";
		actionName = "Fahrzeug entsperren";
		actionIcon = "key-skeleton";
		actionColour = "default";
		actionContext = "vehicle";
	}

	override bool Execute(GameLabsActionContext context)
	{
		Object obj;
		ExpansionVehicle vehicle;

		if (!CFCloud_Bridge_Resolve(context, obj, vehicle))
			return false;

		CFCloud_Bridge_Settings settings = CFCloud_Bridge_Manager.GetInstance().GetSettings();
		if (!settings.m_AllowUnlock)
		{
			return CFCloud_Bridge_Outcome(false, "Entsperren abgelehnt: in Settings.json deaktiviert.", CFCloud_Bridge_Describe(vehicle, obj));
		}

		// Without a paired key the vehicle has no lock at all. Forcing a state
		// would claim a key that does not exist.
		if (!vehicle.HasKey())
		{
			return CFCloud_Bridge_Outcome(false, "Entsperren abgelehnt: kein Schlüssel gepaart, das Fahrzeug hat kein Schloss.", CFCloud_Bridge_Describe(vehicle, obj));
		}

		ExpansionVehicleLockState state = vehicle.GetLockState();
		if (state != ExpansionVehicleLockState.LOCKED && state != ExpansionVehicleLockState.FORCEDLOCKED && state != ExpansionVehicleLockState.READY_TO_LOCK && state != ExpansionVehicleLockState.READY_TO_FORCELOCK)
		{
			return CFCloud_Bridge_Outcome(true, "Nichts zu tun: Fahrzeug ist bereits offen.", CFCloud_Bridge_Describe(vehicle, obj));
		}

		// UNLOCKED, never ForceUnlock()'s FORCEDUNLOCKED default: Expansion
		// uses that state as its "was lockpicked" marker, and the P2P market
		// then refuses to let the owner sell the vehicle.
		vehicle.ForceUnlock(ExpansionVehicleLockState.UNLOCKED);

		return CFCloud_Bridge_Outcome(true, "Fahrzeug entsperrt.", CFCloud_Bridge_Describe(vehicle, obj));
	}
}

#endif
#endif
