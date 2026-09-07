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

		int registered = 0;

#ifdef EXPANSIONMODVEHICLE
		CFCloud_Bridge_VehicleUnlock().Register();
		CFCloud_Bridge_VehicleLock().Register();
		CFCloud_Bridge_VehicleStatus().Register();
		CFCloud_Bridge_VehicleSetOwner().Register();
		CFCloud_Bridge_PlayerVehicles().Register();
		registered = registered + 5;
#else
		CFCloud_Bridge_Logger.Warning("DayZExpansion Vehicles is missing - no vehicle actions registered.");
#endif

#ifdef TERJE_CORE_MOD
		CFCloud_Bridge_TerjeStatus().Register();
		CFCloud_Bridge_TerjeSetStat().Register();
		CFCloud_Bridge_TerjeHeal().Register();
		registered = registered + 3;

		// One action per registered Terje stat - the same list Terje's COT
		// module turns into sliders. Safe to read here: PluginManagerInit()
		// already ran in the MissionBase constructor, so Terje's settings
		// plugin is initialised (see CFCloud_Bridge_TerjeStatAction.c).
		array<ref TerjeAdmintoolSupport_PlayerStat> terjeStats = new array<ref TerjeAdmintoolSupport_PlayerStat>;
		GetTerjeAdmintoolSupport().GetPlayerStats(terjeStats);
		foreach (TerjeAdmintoolSupport_PlayerStat terjeStat : terjeStats)
		{
			CFCloud_Bridge_TerjeStatAction terjeAction = new CFCloud_Bridge_TerjeStatAction(terjeStat);
			terjeAction.Register();
			registered = registered + 1;
		}

		CFCloud_Bridge_Logger.Info("Terje: " + terjeStats.Count().ToString() + " stats registered as actions.");
#else
		CFCloud_Bridge_Logger.Info("TerjeCore is missing - no Terje actions registered.");
#endif

		// Proof in the server log that registration happened before GameLabs
		// posts its action list to /v1/auth/register - that POST is fired a few
		// lines further down in the same GameLabs constructor.
		int actionCount = GetGameLabs().GetGameLabsActions().Count();
		CFCloud_Bridge_Logger.Info("Registered " + registered.ToString() + " actions, " + actionCount.ToString() + " known to GameLabs in total.");
	}
#endif
}
