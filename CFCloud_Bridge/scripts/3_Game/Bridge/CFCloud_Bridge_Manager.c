// Central entry point of the mod. Lives in 3_Game (manager layer) and is driven
// from 5_Mission by MissionServer / MissionGameplay.
// Note: 3_Game must not reference 4_World types (PlayerBase, ItemBase, EntityAI).
// Entity-facing work belongs in a future 4_World class that calls into this one.
class CFCloud_Bridge_Manager
{
	private static ref CFCloud_Bridge_Manager s_Instance;

	private ref CFCloud_Bridge_Settings m_Settings;
	private bool m_Initialized;
	private bool m_IsServer;
	private float m_Accumulator;

	void CFCloud_Bridge_Manager()
	{
		m_Initialized = false;
		m_IsServer = false;
		m_Accumulator = 0;
	}

	static CFCloud_Bridge_Manager GetInstance()
	{
		if (!s_Instance)
			s_Instance = new CFCloud_Bridge_Manager();

		return s_Instance;
	}

	CFCloud_Bridge_Settings GetSettings()
	{
		return m_Settings;
	}

	bool IsInitialized()
	{
		return m_Initialized;
	}

	void Init(bool isServer)
	{
		if (m_Initialized)
			return;

		m_IsServer = isServer;
		m_Settings = CFCloud_Bridge_Settings.Load();
		CFCloud_Bridge_Logger.SetLevel(m_Settings.m_LogLevel);

		m_Initialized = true;

		string side = "CLIENT";
		if (m_IsServer)
			side = "SERVER";

		CFCloud_Bridge_Logger.Info("Initialized on " + side + " - version " + CFCLOUD_BRIDGE_VERSION);

		if (!m_Settings.m_Enabled)
			CFCloud_Bridge_Logger.Warning("Bridge is disabled in Settings.json - nothing will be transferred.");
	}

	void OnUpdate(float timeslice)
	{
		if (!m_Initialized)
			return;

		if (!m_Settings.m_Enabled)
			return;

		m_Accumulator = m_Accumulator + timeslice;

		if (m_Accumulator < m_Settings.m_UpdateIntervalSeconds)
			return;

		m_Accumulator = 0;
		OnTick();
	}

	// Called once per m_UpdateIntervalSeconds while the bridge is enabled.
	void OnTick()
	{
		CFCloud_Bridge_Logger.Debug("Tick");
	}

	void Shutdown()
	{
		if (!m_Initialized)
			return;

		CFCloud_Bridge_Logger.Info("Shutting down.");

		m_Initialized = false;
		m_Settings = null;
	}
}
