modded class MissionServer
{
	override void OnInit()
	{
		super.OnInit();

		// Loads Settings.json and sets the log level. The actions themselves
		// were already registered by then - see GLActionRegisterHook below.
		CFCloud_Bridge_Manager.GetInstance();
	}

#ifdef GAMELABS
	// GameLabs calls this from the MissionServer CONSTRUCTOR (mission.c:208),
	// long before OnInit(). Action constructors may therefore only set their
	// metadata - no world objects, no settings.
	//
	// The real signature is "void GLActionRegisterHook()". The official example
	// in examples/dynamic_actions/mission_server.c omits the return type, which
	// does not compile in Enforce Script.
	override void GLActionRegisterHook()
	{
		// Without this, all of GameLabs' own built-in actions disappear.
		super.GLActionRegisterHook();

#ifdef EXPANSIONMODVEHICLE
		CFCloud_Bridge_VehicleUnlock().Register();
		CFCloud_Bridge_VehicleLock().Register();
		CFCloud_Bridge_VehicleStatus().Register();
		CFCloud_Bridge_VehicleSetOwner().Register();
		CFCloud_Bridge_PlayerVehicles().Register();

		// Proof in the server log that registration happened before GameLabs
		// posts its action list to /v1/auth/register - that POST is fired a few
		// lines further down in the same GameLabs constructor.
		int actionCount = GetGameLabs().GetGameLabsActions().Count();
		CFCloud_Bridge_Logger.Info("Registered 5 actions, " + actionCount.ToString() + " known to GameLabs in total.");
#else
		CFCloud_Bridge_Logger.Warning("DayZExpansion Vehicles is missing - no actions registered.");
#endif
	}
#endif
}
