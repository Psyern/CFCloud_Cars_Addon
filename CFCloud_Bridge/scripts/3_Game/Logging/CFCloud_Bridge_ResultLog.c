// Writes every action result, complete and unshortened, to
// $profile:CFCloud_Bridge\Logs\Actions_YYYY-MM-DD.log.
//
// The one output that needs no network and no configuration: the webhook can
// be unset or fail, and the server log cuts every Print() at 1024 bytes
// (vanilla RestApi.c:78 notes it), which used to swallow the tail of every
// longer list. Open this file from the server's file manager to read what an
// action returned.
//
// The block goes through PsyCore_FS.AppendLines: append mode, one FPrintln per
// entry - the same bytes the former own writer produced. Not PsyCore_Log.EnableFile:
// that puts the time in front of every file line and copies each line into the
// server log, while the file name "Actions_<date>.log" and the block layout are
// what admins already read.
class CFCloud_Bridge_ResultLog
{
	static string GetPath()
	{
		return CFCLOUD_BRIDGE_LOG_DIR + "Actions_" + PsyCore_Time.LocalDate() + ".log";
	}

	static void Write(string actionName, bool success, string headline, string detail)
	{
		PsyCore_Paths.EnsureDir(CFCLOUD_BRIDGE_LOG_DIR);

		string outcome = "REFUSED";
		if (success)
			outcome = "OK";

		array<string> block = new array<string>;
		block.Insert("[" + PsyCore_Time.LocalClock() + "] " + actionName + " - " + outcome);
		block.Insert(headline);

		array<string> lines = new array<string>;
		detail.Split("\n", lines);
		foreach (string line : lines)
		{
			if (line != "")
				block.Insert("    " + line);
		}

		block.Insert("");

		if (!PsyCore_FS.AppendLines(GetPath(), block))
			CFCloud_Bridge_Logger.Error("Could not open the result file " + GetPath());
	}
}
