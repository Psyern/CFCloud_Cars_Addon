class CFCloud_Bridge_Logger
{
	static const int LEVEL_NONE = 0;
	static const int LEVEL_ERROR = 1;
	static const int LEVEL_WARNING = 2;
	static const int LEVEL_INFO = 3;
	static const int LEVEL_DEBUG = 4;

	static int s_Level = LEVEL_INFO;

	static void SetLevel(int level)
	{
		if (level < LEVEL_NONE)
			level = LEVEL_NONE;

		if (level > LEVEL_DEBUG)
			level = LEVEL_DEBUG;

		s_Level = level;
	}

	static void Error(string message)
	{
		LogMessage(LEVEL_ERROR, "ERROR", message);
	}

	static void Warning(string message)
	{
		LogMessage(LEVEL_WARNING, "WARN", message);
	}

	static void Info(string message)
	{
		LogMessage(LEVEL_INFO, "INFO", message);
	}

	static void Debug(string message)
	{
		LogMessage(LEVEL_DEBUG, "DEBUG", message);
	}

	static void LogMessage(int level, string levelName, string message)
	{
		if (level > s_Level)
			return;

		string timeStamp = GetTimeString();
		PrintFormat("%1 [%2] [%3] %4", timeStamp, CFCLOUD_BRIDGE_NAME, levelName, message);
	}

	static string GetTimeString()
	{
		int hour;
		int minute;
		int second;
		GetHourMinuteSecond(hour, minute, second);

		return hour.ToStringLen(2) + ":" + minute.ToStringLen(2) + ":" + second.ToStringLen(2);
	}
}
