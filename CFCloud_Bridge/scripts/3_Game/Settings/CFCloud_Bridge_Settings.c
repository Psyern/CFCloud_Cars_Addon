class CFCloud_Bridge_Settings
{
	// 0 = none, 1 = error, 2 = warning, 3 = info, 4 = debug
	int m_LogLevel = 3;

	// Master switches per action. Checked in Execute(), not at registration
	// time - GLActionRegisterHook() runs from the MissionServer constructor,
	// before these settings are loaded.
	bool m_AllowUnlock = true;
	bool m_AllowLock = true;

	// Reassigning ownership reaches deep, so it stays off until an admin
	// deliberately turns it on.
	bool m_AllowSetOwner = false;

	// Terje player-state actions (only registered when TerjeCore is loaded).
	// Setting a stat and the full heal both change a player's condition, but
	// only within the ranges Terje's own admin tools allow, so they start on.
	bool m_AllowTerjeSetStat = true;
	bool m_AllowTerjeHeal = true;

	// Where action results are reported. The CFCloud interface cannot display
	// them - GameLabs only implements the "webhook" response type. Empty means
	// the server log stays the only output.
	string m_DiscordWebhookUrl = "";

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
	}

	static CFCloud_Bridge_Settings Load()
	{
		CFCloud_Bridge_Settings settings = new CFCloud_Bridge_Settings();
		settings.MakeDirectoryIfNotExists();

		if (FileExist(CFCLOUD_BRIDGE_CONFIG_FILE))
		{
			JsonFileLoader<CFCloud_Bridge_Settings>.JsonLoadFile(CFCLOUD_BRIDGE_CONFIG_FILE, settings);
			settings.Validate();

			// Write the file back so it always matches the current schema.
			// Fields added since the file was created appear with their
			// defaults, fields that no longer exist disappear. Without this an
			// admin edits keys the mod stopped reading, or never sees the new
			// ones at all.
			settings.Save();
			return settings;
		}

		settings.Save();
		return settings;
	}
}
