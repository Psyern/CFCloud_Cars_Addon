class CFCloud_Bridge_Logger
{
	static const int LEVEL_NONE = 0;
	static const int LEVEL_ERROR = 1;
	static const int LEVEL_WARNING = 2;
	static const int LEVEL_INFO = 3;
	static const int LEVEL_DEBUG = 4;

	// Leaves room for the prefix below Print()'s 1024-byte cut-off.
	static const int MAX_LINE = 900;

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

		// Print() drops everything past 1024 bytes, which used to cut long
		// lists off mid-entry. One Print per line, long lines in pieces.
		array<string> lines = new array<string>;
		message.Split("\n", lines);

		foreach (string entry : lines)
		{
			string line = entry;
			while (line.Length() > MAX_LINE)
			{
				PrintFormat("%1 [%2] [%3] %4", timeStamp, CFCLOUD_BRIDGE_NAME, levelName, line.Substring(0, MAX_LINE));
				line = line.Substring(MAX_LINE, line.Length() - MAX_LINE);
			}

			if (line != "")
				PrintFormat("%1 [%2] [%3] %4", timeStamp, CFCLOUD_BRIDGE_NAME, levelName, line);
		}
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
