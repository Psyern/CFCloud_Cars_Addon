// JSON field names and the path are unchanged ($profile:CFCloud_Bridge\Config\Settings.json).
// No version field exists, so the config base runs with version 0 (no migration).
class CFCloud_Bridge_Settings : PsyCore_ConfigBase
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

	// Expansion loadout for a player (only registered with Expansion Core).
	// Can destroy the player's current gear, depending on the chosen mode.
	bool m_AllowExpansionLoadout = true;

	// Expansion territories (only registered with Expansion BaseBuilding).
	// Deleting cannot be undone, so it stays off until an admin turns it on.
	bool m_AllowTerritoryMembers = true;
	bool m_AllowTerritoryRefresh = true;
	bool m_AllowTerritoryDelete = false;

	// Teleport player X to online player Y. Needs only GameLabs.
	bool m_AllowTeleportToPlayer = true;

	// Online-player dropdowns (teleport target). Seconds to wait after a
	// connect/disconnect before the list is rebuilt - a burst of joins after
	// a restart then ends in one rebuild.
	int m_PlayerDropdownDelay = 30;

	// GameLabs sends its action list - and with it the dropdown - to CFCloud
	// only at startup and when it re-authorises. When true, the mod triggers
	// that re-authorisation itself after the player list changed, at most once
	// per m_PlayerDropdownRepublishInterval seconds. When false, the dropdown
	// only reaches CFCloud whenever GameLabs re-registers on its own; the
	// SteamID64 field of the teleport action works either way.
	bool m_PlayerDropdownRepublish = true;
	int m_PlayerDropdownRepublishInterval = 300;

	// Where action results are reported. The CFCloud interface cannot display
	// them - GameLabs only implements the "webhook" response type. Empty means
	// the server log stays the only output.
	string m_DiscordWebhookUrl = "";

	void MakeDirectoryIfNotExists()
	{
		PsyCore_Paths.EnsureDir(CFCLOUD_BRIDGE_CONFIG_DIR);
	}

	void Save()
	{
		PsyCore_ConfigLoader<CFCloud_Bridge_Settings>.Save(CFCLOUD_BRIDGE_CONFIG_FILE, this, CFCLOUD_BRIDGE_NAME);
	}

	override void OnValidate()
	{
		Validate();
	}

	// Clamps values that would break the mod if edited by hand in the JSON.
	void Validate()
	{
		if (m_LogLevel < CFCloud_Bridge_Logger.LEVEL_NONE)
			m_LogLevel = CFCloud_Bridge_Logger.LEVEL_NONE;

		if (m_LogLevel > CFCloud_Bridge_Logger.LEVEL_DEBUG)
			m_LogLevel = CFCloud_Bridge_Logger.LEVEL_DEBUG;

		if (m_PlayerDropdownDelay < 5)
			m_PlayerDropdownDelay = 5;

		// Every re-registration briefly swaps GameLabs' auth header, so it is
		// never allowed to run more often than once a minute.
		if (m_PlayerDropdownRepublishInterval < 60)
			m_PlayerDropdownRepublishInterval = 60;
	}

	// Load -> validate -> write back, as before: the file is always rewritten so
	// it matches the current schema (fields added since the file was created
	// appear with their defaults, fields that no longer exist disappear). A
	// missing file is created with the defaults. New with the core: a file that
	// cannot be parsed is no longer overwritten - the mod runs on defaults until
	// the admin fixes it.
	static CFCloud_Bridge_Settings Load()
	{
		CFCloud_Bridge_Settings settings;
		PsyCore_ConfigLoader<CFCloud_Bridge_Settings>.Load(CFCLOUD_BRIDGE_CONFIG_FILE, settings, "", CFCLOUD_BRIDGE_NAME, true);
		return settings;
	}
}
