#ifdef GAMELABS
#ifdef EXPANSIONMODVEHICLE

// GameLabs only tracks CarScript and BoatScript (!CarScript.c, !BoatScript.c),
// so Expansion helicopters and planes never appear in CFCloud - neither in the
// vehicle context nor in the object one, because object targets are explicitly
// registered _Event instances too.
//
// Registering them here makes them show up as ordinary vehicles, which means
// every vehicle action in this mod works on them unchanged.
//
// The deprecated ExpansionHelicopterScript inherits from CarScript and is
// already tracked by GameLabs itself. That branch is disjoint from this one,
// so nothing is registered twice.
modded class ExpansionVehicleBase
{
	private ref _Vehicle m_CFCloud_Bridge_GLVehicle;

	override void EEInit()
	{
		super.EEInit();

		if (!GetGameLabs())
			return;

		if (!GetGameLabs().IsServer())
			return;

		// Objects still being created report the world origin. Skip them,
		// same guard the GameLabs container hook uses.
		vector position = GetPosition();
		if (position[0] <= 0 && position[1] <= 0 && position[2] <= 0)
			return;

		string icon = "helicopter";
		if (IsInherited(ExpansionVehiclePlaneBase))
			icon = "plane";

		// vehicleType stays "car" on purpose: _Vehicle.SetVehicleType() accepts
		// any string, but whether CFCloud renders an unknown one is not
		// documented anywhere we can check. "car" cannot break. The icon is
		// what tells them apart in the interface.
		m_CFCloud_Bridge_GLVehicle = new _Vehicle(this, icon, "car");
		GetGameLabs().RegisterVehicle(m_CFCloud_Bridge_GLVehicle);

		CFCloud_Bridge_Logger.Debug("Registered aircraft with GameLabs: " + GetType());
	}

	override void EEDelete(EntityAI parent)
	{
		super.EEDelete(parent);

		if (!GetGameLabs())
			return;

		if (!GetGameLabs().IsServer())
			return;

		if (m_CFCloud_Bridge_GLVehicle)
			GetGameLabs().RemoveVehicle(m_CFCloud_Bridge_GLVehicle);
	}
}

#endif
#endif
