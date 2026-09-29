#ifdef GAMELABS

// Keeps "pick an online player" dropdowns in CFCloud current.
//
// GameLabs only knows static dropdowns: a parameter of dataType "options"
// whose entries are filled once, in the action constructor
// (give_player_loadout.c, spawn_vehicle.c). CFCloud receives that list as part
// of the action definitions in POST /v1/auth/register, which GameLabs sends
// ONCE at startup - from the MissionServer constructor, when no player is
// online yet - and again only when it re-authorises after a poll error
// (5_Mission/definitions.c, _Callback_ServerPoll2.OnError).
//
// So this class does two things:
//  1. After a player connects or disconnects it rebuilds the option map of
//     every registered dropdown from the players online right now. GameLabs
//     serialises the live action objects on every register call, so the new
//     list travels with the next one.
//  2. Optionally (Settings.json, m_PlayerDropdownRepublish) it triggers that
//     next register call itself via GameLabsAPI.RegisterAsync() - the same
//     re-authorisation GameLabs runs on its own - throttled to at most one
//     per m_PlayerDropdownRepublishInterval seconds.
//
// Risk of (2), taken from GameLabs' own code: _Callback_RegisterAsync calls
// GameLabsAPI.Disable() on an error or timeout, and nothing in GameLabs turns
// the API back on afterwards. This class therefore watches for exactly that
// state and retries the register on its next run once the API was up before.
//
// The execute side never trusts the list: the chosen SteamID64 is looked up
// again and refused if that player has left.
class CFCloud_Bridge_PlayerDropdown
{
	private static ref CFCloud_Bridge_PlayerDropdown s_Instance;

	// Weak on purpose - the parameters are owned by their actions, which
	// GameLabs keeps in its own action array for the whole session.
	private ref array<GameLabsActionParameter> m_Parameters;

	private string m_LastSignature;
	private int m_LastRepublishTime;
	private bool m_HasRepublished;
	private bool m_ApiWasEnabled;

	void CFCloud_Bridge_PlayerDropdown()
	{
		m_Parameters = new array<GameLabsActionParameter>;
		m_LastSignature = "";
		m_LastRepublishTime = 0;
		m_HasRepublished = false;
		m_ApiWasEnabled = false;
	}

	static CFCloud_Bridge_PlayerDropdown GetInstance()
	{
		if (!s_Instance)
			s_Instance = new CFCloud_Bridge_PlayerDropdown();

		return s_Instance;
	}

	//! Creates a dropdown parameter that this class keeps filled. Called from
	//! action constructors, i.e. from GLActionRegisterHook().
	static GameLabsActionParameter CreateParameter(string displayName, string description)
	{
		GameLabsActionParameter param = new GameLabsActionParameter(displayName, description, "options");
		GetInstance().Track(param);
		return param;
	}

	void Track(GameLabsActionParameter param)
	{
		m_Parameters.Insert(param);
		Fill(param, CollectPlayers());
	}

	//! Debounced: a burst of connects after a restart ends in one rebuild.
	void Schedule()
	{
		int delay = 30;
		CFCloud_Bridge_Settings settings = CFCloud_Bridge_Manager.GetInstance().GetSettings();
		if (settings)
			delay = settings.m_PlayerDropdownDelay;

		g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(Rebuild);
		g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(Rebuild, delay * 1000, false);
	}

	void Rebuild()
	{
		map<string, string> players = CollectPlayers();

		foreach (GameLabsActionParameter param : m_Parameters)
		{
			if (param)
				Fill(param, players);
		}

		string signature = Signature(players);
		if (signature == m_LastSignature && ApiIsHealthy())
			return;

		m_LastSignature = signature;
		CFCloud_Bridge_Logger.Debug("Player dropdown rebuilt: " + players.Count().ToString() + " online.");

		Republish();
	}

	//! SteamID64 -> in-game name of every player with an identity.
	protected map<string, string> CollectPlayers()
	{
		map<string, string> result = new map<string, string>;

		if (!g_Game)
			return result;

		array<PlayerBase> players = new array<PlayerBase>;
		PsyCore_Players.GetOnline(players);

		foreach (PlayerBase player : players)
		{
			PlayerIdentity identity = player.GetIdentity();
			if (!identity)
				continue;

			result.Set(identity.GetPlainId(), identity.GetName());
		}

		return result;
	}

	protected void Fill(GameLabsActionParameter param, map<string, string> players)
	{
		param.options.Clear();

		if (players.Count() == 0)
		{
			// An empty dropdown gives the admin no hint why it is empty.
			GameLabsActionParameter placeholder = new GameLabsActionParameter("(niemand online - Liste wird aktualisiert)", "", "string");
			placeholder.valueString = "";
			param.options.Insert("none", placeholder);
			return;
		}

		foreach (string steamId, string name : players)
		{
			GameLabsActionParameter option = new GameLabsActionParameter(name + " (" + steamId + ")", "", "string");
			option.valueString = steamId;
			param.options.Insert(steamId, option);
		}
	}

	protected string Signature(map<string, string> players)
	{
		array<string> ids = new array<string>;
		foreach (string steamId, string name : players)
		{
			ids.Insert(steamId + "=" + name);
		}

		ids.Sort();

		string signature = "";
		foreach (string entry : ids)
		{
			signature = signature + entry + ";";
		}

		return signature;
	}

	protected bool ApiIsHealthy()
	{
		if (!GetGameLabs() || !GetGameLabs().GetApi())
			return true;

		if (GetGameLabs().GetApi().IsEnabled())
		{
			m_ApiWasEnabled = true;
			return true;
		}

		// Never up at all means wrong credentials or no grant - re-registering
		// cannot fix that, so it does not count as "needs a retry".
		return !m_ApiWasEnabled;
	}

	protected void Republish()
	{
		CFCloud_Bridge_Settings settings = CFCloud_Bridge_Manager.GetInstance().GetSettings();
		if (!settings || !settings.m_PlayerDropdownRepublish)
			return;

		if (!GetGameLabs() || !GetGameLabs().GetApi())
			return;

		ApiIsHealthy();
		if (!m_ApiWasEnabled)
			return;

		int now = g_Game.GetTime();
		int minGap = settings.m_PlayerDropdownRepublishInterval * 1000;

		if (m_HasRepublished && now - m_LastRepublishTime < minGap)
		{
			// Too soon - come back when the gap is over. Whatever changed in
			// the meantime is picked up by that run.
			int wait = minGap - (now - m_LastRepublishTime);
			g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(Rebuild);
			g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(Rebuild, wait, false);

			// Forget the signature so that run does not return early.
			m_LastSignature = "";
			return;
		}

		m_HasRepublished = true;
		m_LastRepublishTime = now;

		CFCloud_Bridge_Logger.Info("Re-registering with GameLabs to publish the online player list.");
		GetGameLabs().GetApi().RegisterAsync();
	}
}

#endif
