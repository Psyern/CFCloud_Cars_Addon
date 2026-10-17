#ifdef GAMELABS

// Logs what the webhook answered. Discord answers 204 on success; everything
// else used to vanish inside GameLabs' empty _Callback.
class CFCloud_Bridge_WebhookCallback extends RestCallback
{
	protected string m_ActionName;
	protected bool m_Done;

	void CFCloud_Bridge_WebhookCallback(string actionName)
	{
		m_ActionName = actionName;
		m_Done = false;
	}

	bool IsDone()
	{
		return m_Done;
	}

	override void OnError(int errorCode)
	{
		m_Done = true;
		string state = typename.EnumToString(ERestResultState, errorCode);
		CFCloud_Bridge_Logger.Error("Webhook for '" + m_ActionName + "' failed: " + state + " (" + errorCode.ToString() + "). Check m_DiscordWebhookUrl; the full result is in " + CFCLOUD_BRIDGE_LOG_DIR);
	}

	override void OnTimeout()
	{
		m_Done = true;
		CFCloud_Bridge_Logger.Error("Webhook for '" + m_ActionName + "' timed out. The full result is in " + CFCLOUD_BRIDGE_LOG_DIR);
	}

	override void OnSuccess(string data, int dataSize)
	{
		m_Done = true;
		CFCloud_Bridge_Logger.Debug("Webhook for '" + m_ActionName + "' delivered.");
	}
}

// Sends action results somewhere an admin actually looks.
//
// The CFCloud interface cannot show them. GameLabsActionResponse.Execute()
// (classes.c) implements exactly one responseType, "webhook"; every other value
// returns false and does nothing. The "output" type named in its comment has no
// implementation at all. So a webhook is the only channel that exists.
//
// This posts Discord-shaped embeds, built from GameLabs' own payload classes
// and following the same RestContext pattern its reporting module uses
// (5_Mission/reporting.c:123,171). Leave the URL empty in Settings.json and
// nothing is sent - the result file and the server log still get everything
// (CFCloud_Bridge_ResultLog).
//
// Two things made list results disappear before, and both are handled here:
//  * Discord rejects a message whose embed description exceeds 4096
//    characters or whose embeds exceed 6000 in total - with an HTTP 400 that
//    GameLabs' _Callback swallows. Long results are therefore split at line
//    boundaries, one embed per message, sent a moment apart so the webhook's
//    rate limit (5 requests per 2 s) is never hit.
//  * Every failure is logged now, with the engine's REST state.
//
// Deliberately not PsyCore_Discord: its callback logs failures as WARN with a
// fixed text (an m_LogLevel of 1 would hide them, and the hint to the settings
// key and the result file would be gone), and the payload follows GameLabs' own
// classes (username/avatar) on purpose.
class CFCloud_Bridge_Report
{
	private static const int COLOUR_SUCCESS = 3066993;   // Discord green
	private static const int COLOUR_REFUSED = 15105570;  // Discord orange

	// Well below Discord's 4096 per description and 6000 per message, which
	// also counts title and fields.
	private static const int MAX_DESCRIPTION = 3800;
	private static const int MAX_MESSAGES = 10;
	private static const int MESSAGE_GAP_MS = 700;

	private static ref CFCloud_Bridge_Report s_Instance;

	private ref array<ref CFCloud_Bridge_WebhookCallback> m_Callbacks;

	void CFCloud_Bridge_Report()
	{
		m_Callbacks = new array<ref CFCloud_Bridge_WebhookCallback>;
	}

	static CFCloud_Bridge_Report GetInstance()
	{
		if (!s_Instance)
			s_Instance = new CFCloud_Bridge_Report();

		return s_Instance;
	}

	//! headline: one line. detail: any length, lines separated by "\n".
	static void Send(string actionName, bool success, string headline, string detail)
	{
		CFCloud_Bridge_Settings settings = CFCloud_Bridge_Manager.GetInstance().GetSettings();
		if (!settings)
			return;

		string url = settings.m_DiscordWebhookUrl;
		if (url == "")
			return;

		array<string> chunks = Split(headline + "\n" + detail, MAX_DESCRIPTION);
		int total = chunks.Count();
		if (total > MAX_MESSAGES)
		{
			total = MAX_MESSAGES;
			chunks[total - 1] = chunks[total - 1] + "\n... gekuerzt - vollstaendig in " + CFCLOUD_BRIDGE_LOG_DIR;
		}

		int colour = COLOUR_REFUSED;
		string outcome = "Nicht ausgefuehrt";
		if (success)
		{
			colour = COLOUR_SUCCESS;
			outcome = "Ausgefuehrt";
		}

		CFCloud_Bridge_Report reporter = GetInstance();

		for (int i = 0; i < total; i++)
		{
			_Payload_DiscordWebHookEmbed embed = new _Payload_DiscordWebHookEmbed();
			embed.SetColor(colour);
			embed.SetDescription(chunks[i]);

			if (i == 0)
			{
				embed.SetTitle(actionName);
				embed.AddField("Ergebnis", outcome, true);
				embed.AddField("Server", CFCLOUD_BRIDGE_NAME + " " + CFCLOUD_BRIDGE_VERSION, true);
			}
			else
			{
				int part = i + 1;
				embed.SetTitle(actionName + " (Teil " + part.ToString() + "/" + total.ToString() + ")");
			}

			_Payload_DiscordWebHook webhook = new _Payload_DiscordWebHook();
			webhook.AddEmbed(embed);
			string json = webhook.ToJson();

			if (i == 0)
				reporter.Post(url, json, actionName);
			else
				g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(reporter.Post, i * MESSAGE_GAP_MS, false, url, json, actionName);
		}
	}

	void Post(string url, string json, string actionName)
	{
		RestContext ctx = GetRestApi().GetRestContext(url);
		if (!ctx)
		{
			CFCloud_Bridge_Logger.Error("Webhook: could not open a REST context for the configured URL.");
			return;
		}

		ctx.SetHeader("application/json");

		// The engine gets the callback as a plain pointer; keeping a strong
		// reference here guarantees it still exists when the answer arrives.
		// Finished ones are dropped on the next post rather than from inside
		// their own callback.
		for (int i = m_Callbacks.Count() - 1; i >= 0; i--)
		{
			if (m_Callbacks[i].IsDone())
				m_Callbacks.Remove(i);
		}

		CFCloud_Bridge_WebhookCallback callback = new CFCloud_Bridge_WebhookCallback(actionName);
		m_Callbacks.Insert(callback);

		ctx.POST(callback, "", json);
	}

	//! Splits at line breaks into pieces of at most maxLength characters; a
	//! single longer line is cut hard.
	static array<string> Split(string text, int maxLength)
	{
		array<string> chunks = new array<string>;
		array<string> lines = new array<string>;
		text.Split("\n", lines);

		string current = "";
		foreach (string entry : lines)
		{
			string line = entry;
			while (line.Length() > maxLength)
			{
				if (current != "")
				{
					chunks.Insert(current);
					current = "";
				}

				chunks.Insert(line.Substring(0, maxLength));
				line = line.Substring(maxLength, line.Length() - maxLength);
			}

			if (current != "" && current.Length() + 1 + line.Length() > maxLength)
			{
				chunks.Insert(current);
				current = "";
			}

			if (current == "")
				current = line;
			else
				current = current + "\n" + line;
		}

		if (current != "" || chunks.Count() == 0)
			chunks.Insert(current);

		return chunks;
	}
}

#endif
