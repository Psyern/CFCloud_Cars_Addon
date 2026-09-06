#ifdef GAMELABS

// Sends action results somewhere an admin actually looks.
//
// The CFCloud interface cannot show them. GameLabsActionResponse.Execute()
// (classes.c) implements exactly one responseType, "webhook"; every other value
// returns false and does nothing. The "output" type named in its comment has no
// implementation at all. So a webhook is the only channel that exists.
//
// This posts a Discord-shaped embed, built from GameLabs' own payload classes
// and following the same RestContext pattern its reporting module uses
// (5_Mission/reporting.c:123,171). Leave the URL empty in Settings.json and
// nothing is sent - the server log stays the only output, as before.
class CFCloud_Bridge_Report
{
	private static const int COLOUR_SUCCESS = 3066993;   // Discord green
	private static const int COLOUR_REFUSED = 15105570;  // Discord orange

	static void Send(string actionName, bool success, string detail)
	{
		CFCloud_Bridge_Settings settings = CFCloud_Bridge_Manager.GetInstance().GetSettings();
		if (!settings)
			return;

		string url = settings.m_DiscordWebhookUrl;
		if (url == "")
			return;

		int colour = COLOUR_REFUSED;
		string outcome = "Nicht ausgeführt";
		if (success)
		{
			colour = COLOUR_SUCCESS;
			outcome = "Ausgeführt";
		}

		RestContext ctx = GetRestApi().GetRestContext(url);
		if (!ctx)
		{
			CFCloud_Bridge_Logger.Error("Could not open a REST context for the configured webhook URL.");
			return;
		}

		ctx.SetHeader("application/json");

		_Payload_DiscordWebHookEmbed embed = new _Payload_DiscordWebHookEmbed();
		embed.SetTitle(actionName);
		embed.SetColor(colour);
		embed.SetDescription(detail);
		embed.AddField("Ergebnis", outcome, true);
		embed.AddField("Server", CFCLOUD_BRIDGE_NAME + " " + CFCLOUD_BRIDGE_VERSION, true);

		_Payload_DiscordWebHook webhook = new _Payload_DiscordWebHook();
		webhook.AddEmbed(embed);

		ctx.POST(new _Callback(), "", webhook.ToJson());
	}
}

#endif
