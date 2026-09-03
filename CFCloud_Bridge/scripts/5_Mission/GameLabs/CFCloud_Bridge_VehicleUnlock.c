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
			CFCloud_Bridge_Logger.Warning("Unlock refused, disabled in Settings.json. " + CFCloud_Bridge_Describe(vehicle, obj));
			return false;
		}

		// Without a paired key the vehicle has no lock at all. Forcing a state
		// would claim a key that does not exist.
		if (!vehicle.HasKey())
		{
			CFCloud_Bridge_Logger.Warning("Unlock refused, vehicle has no paired key. " + CFCloud_Bridge_Describe(vehicle, obj));
			return false;
		}

		ExpansionVehicleLockState state = vehicle.GetLockState();
		if (state != ExpansionVehicleLockState.LOCKED && state != ExpansionVehicleLockState.FORCEDLOCKED && state != ExpansionVehicleLockState.READY_TO_LOCK && state != ExpansionVehicleLockState.READY_TO_FORCELOCK)
		{
			CFCloud_Bridge_Logger.Info("Unlock skipped, vehicle is already open. " + CFCloud_Bridge_Describe(vehicle, obj));
			return true;
		}

		// UNLOCKED, never ForceUnlock()'s FORCEDUNLOCKED default: Expansion
		// uses that state as its "was lockpicked" marker, and the P2P market
		// then refuses to let the owner sell the vehicle.
		vehicle.ForceUnlock(ExpansionVehicleLockState.UNLOCKED);

		CFCloud_Bridge_Logger.Warning("Unlocked. " + CFCloud_Bridge_Describe(vehicle, obj));
		return true;
	}
}

#endif
#endif
