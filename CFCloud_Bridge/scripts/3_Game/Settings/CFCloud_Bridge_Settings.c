class CFCloud_Bridge_Settings
{
	// 0 = none, 1 = error, 2 = warning, 3 = info, 4 = debug
	int m_LogLevel = 3;

	// Master switch for the bridge. Off until the endpoint is configured.
	bool m_Enabled = false;

	// Seconds between two bridge ticks.
	int m_UpdateIntervalSeconds = 60;

	void MakeDirectoryIfNotExists()
	{
		if (!FileExist(CFCLOUD_BRIDGE_ROOT_FOLDER))
			MakeDirectory(CFCLOUD_BRIDGE_ROOT_FOLDER);

		if (!FileExist(CFCLOUD_BRIDGE_CONFIG_DIR))
			MakeDirectory(CFCLOUD_BRIDGE_CONFIG_DIR);
	}

	void Save()
	{
		MakeDirectoryIfNotExists();
		JsonFileLoader<CFCloud_Bridge_Settings>.JsonSaveFile(CFCLOUD_BRIDGE_CONFIG_FILE, this);
	}

	// Clamps values that would break the mod if edited by hand in the JSON.
	void Validate()
	{
		if (m_LogLevel < CFCloud_Bridge_Logger.LEVEL_NONE)
			m_LogLevel = CFCloud_Bridge_Logger.LEVEL_NONE;

		if (m_LogLevel > CFCloud_Bridge_Logger.LEVEL_DEBUG)
			m_LogLevel = CFCloud_Bridge_Logger.LEVEL_DEBUG;

		if (m_UpdateIntervalSeconds < 5)
			m_UpdateIntervalSeconds = 5;
	}

	static CFCloud_Bridge_Settings Load()
	{
		CFCloud_Bridge_Settings settings = new CFCloud_Bridge_Settings();
		settings.MakeDirectoryIfNotExists();

		if (FileExist(CFCLOUD_BRIDGE_CONFIG_FILE))
		{
			JsonFileLoader<CFCloud_Bridge_Settings>.JsonLoadFile(CFCLOUD_BRIDGE_CONFIG_FILE, settings);
			settings.Validate();
			return settings;
		}

		settings.Save();
		return settings;
	}
}
