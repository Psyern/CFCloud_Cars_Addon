// Thin wrapper around PsyCore_Log (tag "CFCloud_Bridge"). Kept because the
// settings store the level as 0-4 (m_LogLevel) and because multi-line results
// are logged one line per entry - PsyCore_Log chunks long lines itself.
class CFCloud_Bridge_Logger
{
	static const int LEVEL_NONE = 0;
	static const int LEVEL_ERROR = 1;
	static const int LEVEL_WARNING = 2;
	static const int LEVEL_INFO = 3;
	static const int LEVEL_DEBUG = 4;

	static PsyCore_Log Log()
	{
		return PsyCore_Log.Get(CFCLOUD_BRIDGE_NAME);
	}

	//! 0 = none, 1 = error, 2 = warning, 3 = info, 4 = debug (Settings.json m_LogLevel)
	static void SetLevel(int level)
	{
		if (level < LEVEL_NONE)
			level = LEVEL_NONE;

		if (level > LEVEL_DEBUG)
			level = LEVEL_DEBUG;

		int coreLevel = PsyCore_LogLevel.INFO;
		switch (level)
		{
			case LEVEL_NONE:
				coreLevel = PsyCore_LogLevel.NONE;
				break;
			case LEVEL_ERROR:
				coreLevel = PsyCore_LogLevel.ERROR;
				break;
			case LEVEL_WARNING:
				coreLevel = PsyCore_LogLevel.WARN;
				break;
			case LEVEL_INFO:
				coreLevel = PsyCore_LogLevel.INFO;
				break;
			case LEVEL_DEBUG:
				coreLevel = PsyCore_LogLevel.DEBUG;
				break;
		}

		Log().SetLevel(coreLevel);
	}

	static void Error(string message)
	{
		LogMessage(PsyCore_LogLevel.ERROR, message);
	}

	static void Warning(string message)
	{
		LogMessage(PsyCore_LogLevel.WARN, message);
	}

	static void Info(string message)
	{
		LogMessage(PsyCore_LogLevel.INFO, message);
	}

	static void Debug(string message)
	{
		LogMessage(PsyCore_LogLevel.DEBUG, message);
	}

	static void LogMessage(int coreLevel, string message)
	{
		PsyCore_Log logger = Log();
		if (!logger.IsEnabled(coreLevel))
			return;

		// One log line per result line, as before; empty lines are skipped.
		array<string> lines = new array<string>;
		message.Split("\n", lines);

		foreach (string line : lines)
		{
			if (line != "")
				logger.Write(coreLevel, line);
		}
	}

	static string GetTimeString()
	{
		return PsyCore_Time.LocalClock();
	}
}
