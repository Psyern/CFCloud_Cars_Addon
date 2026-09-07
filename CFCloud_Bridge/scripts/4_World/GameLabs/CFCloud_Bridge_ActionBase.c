#ifdef GAMELABS

// Root of every action in this mod, whatever its context. Holds only what
// does not depend on Expansion or Terje: the outcome reporting and small
// formatting helpers. Lives in 4_World so that the context-specific bases in
// this folder (Vehicle*, Terje*) and the actions in 5_Mission are guaranteed
// to see it - within one layer files compile in alphabetical order, and
// "ActionBase" sorts before both.
class CFCloud_Bridge_ActionBase extends GameLabsContextAction
{
	//! Logs the outcome and mirrors it to the configured webhook. Returns the
	//! value Execute() should return, so call sites stay one line.
	protected bool CFCloud_Bridge_Outcome(bool success, string headline, string detail)
	{
		string text = headline + " " + detail;

		CFCloud_Bridge_Logger.Warning(text);
		CFCloud_Bridge_Report.Send(actionName, success, text);

		return success;
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
