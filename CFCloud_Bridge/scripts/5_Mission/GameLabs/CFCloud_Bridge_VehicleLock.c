#ifdef GAMELABS
#ifdef EXPANSIONMODVEHICLE

class CFCloud_Bridge_VehicleLock extends CFCloud_Bridge_VehicleActionBase
{
	void CFCloud_Bridge_VehicleLock()
	{
		actionCode = "CFCloudBridge_VehicleLock";
		actionName = "Fahrzeug sperren (Admin)";
		actionIcon = "lock-alt";
		actionColour = "warning";
		actionContext = "vehicle";
	}

	override bool Execute(GameLabsActionContext context)
	{
		Object obj;
		ExpansionVehicle vehicle;

		if (!CFCloud_Bridge_Resolve(context, obj, vehicle))
			return false;

		CFCloud_Bridge_Settings settings = CFCloud_Bridge_Manager.GetInstance().GetSettings();
		if (!settings.m_AllowLock)
		{
			CFCloud_Bridge_Logger.Warning("Lock refused, disabled in Settings.json. " + CFCloud_Bridge_Describe(vehicle, obj));
			return false;
		}

		if (!vehicle.HasKey())
		{
			CFCloud_Bridge_Logger.Warning("Lock refused, vehicle has no paired key. " + CFCloud_Bridge_Describe(vehicle, obj));
			return false;
		}

		ExpansionVehicleLockState state = vehicle.GetLockState();
		if (state == ExpansionVehicleLockState.FORCEDLOCKED || state == ExpansionVehicleLockState.READY_TO_FORCELOCK)
		{
			CFCloud_Bridge_Logger.Info("Lock skipped, vehicle is already admin-locked. " + CFCloud_Bridge_Describe(vehicle, obj));
			return true;
		}

		// Expansion's own Market module force-locks exactly like this: set the
		// intermediate state and let UpdateLock() convert it to FORCEDLOCKED on
		// the next server tick, which also fires OnLocked() and the lock sound.
		// Setting FORCEDLOCKED directly would skip that.
		vehicle.SetLockState(ExpansionVehicleLockState.READY_TO_FORCELOCK);

		CFCloud_Bridge_Logger.Warning("Admin lock queued, becomes FORCEDLOCKED on the next server tick. " + CFCloud_Bridge_Describe(vehicle, obj));
		return true;
	}
}

#endif
#endif
