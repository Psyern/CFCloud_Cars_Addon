#ifdef GAMELABS

// Root of every action in this mod, whatever its context. Holds only what
// does not depend on Expansion or Terje: the outcome reporting and small
// formatting helpers. Lives in 4_World so that the context-specific bases in
// this folder (Vehicle*, Terje*) and the actions in 5_Mission are guaranteed
// to see it - within one layer files compile in alphabetical order, and
// "ActionBase" sorts before both.
class CFCloud_Bridge_ActionBase extends GameLabsContextAction
{
	//! Sends the outcome to all three outputs: the result file (always, in
	//! full), the server log, and the webhook if one is configured. detail may
	//! span several lines - list actions put one entry per line. Returns the
	//! value Execute() should return, so call sites stay one line.
	protected bool CFCloud_Bridge_Outcome(bool success, string headline, string detail)
	{
		CFCloud_Bridge_ResultLog.Write(actionName, success, headline, detail);
		CFCloud_Bridge_Logger.Warning(actionName + ": " + headline + "\n" + detail);
		CFCloud_Bridge_Report.Send(actionName, success, headline, detail);

		return success;
	}

	//! Appends one entry to a multi-line list for CFCloud_Bridge_Outcome.
	protected string CFCloud_Bridge_AddLine(string list, string line)
	{
		if (list == "")
			return line;

		return list + "\n" + line;
	}

	protected string CFCloud_Bridge_YesNo(bool value)
	{
		if (value)
			return "yes";

		return "no";
	}

	//! Reads a parameter without tripping over a missing entry. GameLabs
	//! fills the map from the incoming order, so a parameter the interface
	//! did not send is simply absent rather than empty.
	protected GameLabsActionParameter CFCloud_Bridge_Param(GameLabsActionContext context, string key)
	{
		if (!context.parameters)
			return null;

		GameLabsActionParameter param;
		if (!context.parameters.Find(key, param))
			return null;

		return param;
	}
}

#endif
