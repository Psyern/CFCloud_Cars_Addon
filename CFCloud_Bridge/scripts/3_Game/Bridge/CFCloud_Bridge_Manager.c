// Holds the mod's settings and nothing else. There is no tick and no polling:
// the mod only ever reacts to orders that arrive through GameLabs.
//
// GLActionRegisterHook() runs from the MissionServer constructor, which is
// earlier than MissionServer.OnInit(). An action could therefore ask for
// settings before Init() ever ran, so GetInstance() initialises on first use.
class CFCloud_Bridge_Manager
{
	private static ref CFCloud_Bridge_Manager s_Instance;

	private ref CFCloud_Bridge_Settings m_Settings;
	private bool m_Initialized;

	void CFCloud_Bridge_Manager()
	{
		m_Initialized = false;
	}

	static CFCloud_Bridge_Manager GetInstance()
	{
		if (!s_Instance)
			s_Instance = new CFCloud_Bridge_Manager();

		if (!s_Instance.IsInitialized())
			s_Instance.Init();

		return s_Instance;
	}

	bool IsInitialized()
	{
		return m_Initialized;
	}

	CFCloud_Bridge_Settings GetSettings()
	{
		return m_Settings;
	}

	void Init()
	{
		if (m_Initialized)
			return;

		m_Settings = CFCloud_Bridge_Settings.Load();
		CFCloud_Bridge_Logger.SetLevel(m_Settings.m_LogLevel);

		m_Initialized = true;

		CFCloud_Bridge_Logger.Info("Initialized - version " + CFCLOUD_BRIDGE_VERSION);
	}
}
