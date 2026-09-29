// Writes every action result, complete and unshortened, to
// $profile:CFCloud_Bridge\Logs\Actions_YYYY-MM-DD.log.
//
// The one output that needs no network and no configuration: the webhook can
// be unset or fail, and the server log cuts every Print() at 1024 bytes
// (vanilla RestApi.c:78 notes it), which used to swallow the tail of every
// longer list. Open this file from the server's file manager to read what an
// action returned.
//
// Stays a file writer of its own (not PsyCore_Log.EnableFile): the file name
// "Actions_<date>.log" and the block layout are what admins already read.
class CFCloud_Bridge_ResultLog
{
	static string GetPath()
	{
		return CFCLOUD_BRIDGE_LOG_DIR + "Actions_" + PsyCore_Time.LocalDate() + ".log";
	}

	static void Write(string actionName, bool success, string headline, string detail)
	{
		PsyCore_Paths.EnsureDir(CFCLOUD_BRIDGE_LOG_DIR);

		FileHandle file = OpenFile(GetPath(), FileMode.APPEND);
		if (!file)
		{
			CFCloud_Bridge_Logger.Error("Could not open the result file " + GetPath());
			return;
		}

		string outcome = "REFUSED";
		if (success)
			outcome = "OK";

		FPrintln(file, "[" + PsyCore_Time.LocalClock() + "] " + actionName + " - " + outcome);
		FPrintln(file, headline);

		array<string> lines = new array<string>;
		detail.Split("\n", lines);
		foreach (string line : lines)
		{
			if (line != "")
				FPrintln(file, "    " + line);
		}

		FPrintln(file, "");
		CloseFile(file);
	}
}
