#ifdef GAMELABS
#ifdef EXPANSIONMODCORE

// Gives an online player one of the server's Expansion loadouts - the JSON
// files in $profile:ExpansionMod\Loadouts\ that spawn selection, AI and
// quests draw from.
//
// Equipping goes through Expansion's own ExpansionHumanLoadout.Apply()
// (Core/Scripts/4_World/.../Loadout/ExpansionLoadout.c:3199), the call the
// spawn selection makes for a fresh character. Apply() never removes gear: an
// occupied slot makes that item and everything inside it silently skip, and
// cargo that finds no room is not created at all. That is why the action asks
// what should happen to the gear the player already wears:
//   replace - destroy it first (PlayerBase.RemoveAllItems, vanilla)
//   drop    - put it on the ground first (PlayerBase.DropAllItems, vanilla)
//   add     - keep it, the loadout only fills free slots
//
// The dropdown is read once, at registration - loadouts are files that do not
// change while the server runs. A loadout added later can still be given by
// typing its name into the override field.
class CFCloud_Bridge_ExpansionLoadout extends CFCloud_Bridge_PlayerActionBase
{
	static const string MODE_REPLACE = "replace";
	static const string MODE_DROP = "drop";
	static const string MODE_ADD = "add";

	void CFCloud_Bridge_ExpansionLoadout()
	{
		actionCode = "CFCloudBridge_ExpansionLoadout";
		actionName = "Expansion-Loadout geben";
		actionIcon = "tshirt";
		actionColour = "warning";
		actionContext = "player";

		GameLabsActionParameter loadout = new GameLabsActionParameter("Loadout", "Datei aus $profile:ExpansionMod\\Loadouts\\.", "options");
		array<string> names = CFCloud_Bridge_ListLoadouts();
		foreach (string name : names)
		{
			GameLabsActionParameter loadoutOption = new GameLabsActionParameter(name, "", "string");
			loadoutOption.valueString = name;
			loadout.options.Insert(name, loadoutOption);
		}

		GameLabsActionParameter mode = new GameLabsActionParameter("Vorhandene Ausruestung", "Was mit der Ausruestung passiert, die der Spieler gerade traegt.", "options");
		CFCloud_Bridge_AddOption(mode, MODE_REPLACE, "Loeschen, dann Loadout anziehen");
		CFCloud_Bridge_AddOption(mode, MODE_DROP, "Auf den Boden legen, dann Loadout anziehen");
		CFCloud_Bridge_AddOption(mode, MODE_ADD, "Behalten, Loadout fuellt nur freie Plaetze");

		parameters.Insert("loadout", loadout);
		parameters.Insert("loadoutName", GameLabsActionParameter("Loadout-Name (optional)", "Ueberschreibt die Auswahl, z.B. fuer ein nach dem Serverstart angelegtes Loadout.", "string"));
		parameters.Insert("mode", mode);
	}

	protected void CFCloud_Bridge_AddOption(GameLabsActionParameter param, string value, string displayName)
	{
		GameLabsActionParameter option = new GameLabsActionParameter(displayName, "", "string");
		option.valueString = value;
		param.options.Insert(value, option);
	}

	//! Loadout names without ".json", the way Apply() takes them. Init()
	//! first so that Expansion's default loadouts exist on a fresh profile;
	//! it is guarded and the same call Expansion's MissionServer makes.
	static array<string> CFCloud_Bridge_ListLoadouts()
	{
		ExpansionHumanLoadout.Init();

		array<string> names = new array<string>;
		array<string> files = ExpansionStatic.FindFilesInLocation(EXPANSION_LOADOUT_FOLDER, ExpansionHumanLoadout.FILETYPE);
		if (!files)
			return names;

		foreach (string file : files)
		{
			names.Insert(ExpansionString.StripExtension(file, ExpansionHumanLoadout.FILETYPE));
		}

		names.Sort();
		return names;
	}

	override bool Execute(GameLabsActionContext context)
	{
		PlayerBase player;
		if (!CFCloud_Bridge_ResolvePlayer(context, player))
			return false;

		string label = CFCloud_Bridge_PlayerLabel(player);

		CFCloud_Bridge_Settings settings = CFCloud_Bridge_Manager.GetInstance().GetSettings();
		if (!settings.m_AllowExpansionLoadout)
		{
			return CFCloud_Bridge_Outcome(false, "Loadout abgelehnt: in Settings.json deaktiviert.", label);
		}

		string name = "";
		GameLabsActionParameter nameParam = CFCloud_Bridge_Param(context, "loadoutName");
		if (nameParam)
			name = nameParam.GetString().Trim();

		if (name == "")
		{
			GameLabsActionParameter loadoutParam = CFCloud_Bridge_Param(context, "loadout");
			if (loadoutParam)
				name = loadoutParam.GetString().Trim();
		}

		if (name == "")
		{
			return CFCloud_Bridge_Outcome(false, "Loadout abgelehnt: kein Loadout gewaehlt.", label);
		}

		name = ExpansionString.StripExtension(name, ExpansionHumanLoadout.FILETYPE);

		// Checked before any gear is touched - Apply() only reports an
		// unknown loadout after the player has already been stripped.
		// Same path ExpansionPrefab.GetPath() builds (ExpansionPrefab.c:53),
		// spelled out because that name is also an instance overload.
		string path = EXPANSION_LOADOUT_FOLDER + name + ExpansionHumanLoadout.FILETYPE;
		if (!FileExist(path))
		{
			return CFCloud_Bridge_Outcome(false, "Loadout abgelehnt: '" + name + "' existiert nicht in " + EXPANSION_LOADOUT_FOLDER + ".", label);
		}

		if (player.IsInVehicle())
		{
			return CFCloud_Bridge_Outcome(false, "Loadout abgelehnt: " + label + " sitzt in einem Fahrzeug.", "Loadout: " + name);
		}

		string mode = MODE_REPLACE;
		GameLabsActionParameter modeParam = CFCloud_Bridge_Param(context, "mode");
		if (modeParam && modeParam.GetString() != "")
			mode = modeParam.GetString();

		string modeText;
		if (mode == MODE_DROP)
		{
			player.DropAllItems();
			modeText = "alte Ausruestung auf den Boden gelegt";
		}
		else if (mode == MODE_ADD)
		{
			modeText = "alte Ausruestung behalten";
		}
		else
		{
			player.RemoveAllItems();
			modeText = "alte Ausruestung geloescht";
		}

		if (!ExpansionHumanLoadout.Apply(player, name, false))
		{
			return CFCloud_Bridge_Outcome(false, "Loadout '" + name + "' konnte nicht angewendet werden (" + modeText + ").", label);
		}

		return CFCloud_Bridge_Outcome(true, "Loadout '" + name + "' an " + label + " gegeben.", "Modus: " + modeText + ".");
	}
}

#endif
#endif
