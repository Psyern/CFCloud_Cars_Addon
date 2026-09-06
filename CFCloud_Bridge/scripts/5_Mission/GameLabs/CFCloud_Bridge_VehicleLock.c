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
			return CFCloud_Bridge_Outcome(false, "Sperren abgelehnt: in Settings.json deaktiviert.", CFCloud_Bridge_Describe(vehicle, obj));
		}

		if (!vehicle.HasKey())
		{
			return CFCloud_Bridge_Outcome(false, "Sperren abgelehnt: kein Schlüssel gepaart, das Fahrzeug hat kein Schloss.", CFCloud_Bridge_Describe(vehicle, obj));
		}

		ExpansionVehicleLockState state = vehicle.GetLockState();
		if (state == ExpansionVehicleLockState.FORCEDLOCKED || state == ExpansionVehicleLockState.READY_TO_FORCELOCK)
		{
			return CFCloud_Bridge_Outcome(true, "Nichts zu tun: Fahrzeug ist bereits Admin-gesperrt.", CFCloud_Bridge_Describe(vehicle, obj));
		}

		// Expansion's own Market module force-locks exactly like this: set the
		// intermediate state and let UpdateLock() convert it to FORCEDLOCKED on
		// the next server tick, which also fires OnLocked() and the lock sound.
		// Setting FORCEDLOCKED directly would skip that.
		vehicle.SetLockState(ExpansionVehicleLockState.READY_TO_FORCELOCK);

		return CFCloud_Bridge_Outcome(true, "Admin-Sperre gesetzt, wird im nächsten Server-Tick zu FORCEDLOCKED.", CFCloud_Bridge_Describe(vehicle, obj));
	}
}

#endif
#endif
