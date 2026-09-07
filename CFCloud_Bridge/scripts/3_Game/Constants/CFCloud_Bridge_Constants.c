// Declared in 3_Game, the lowest layer this mod uses.
// Higher script modules (4_World, 5_Mission) inherit these symbols - do NOT
// list this folder in more than one files[] entry of config.cpp, that would
// declare every constant a second time ("Multiple declaration of variable").

const string CFCLOUD_BRIDGE_NAME = "CFCloud_Bridge";
const string CFCLOUD_BRIDGE_VERSION = "0.4.0";

// $profile: resolves to the -profiles folder on the server
// and to %localappdata%\dayz on the client.

const string CFCLOUD_BRIDGE_ROOT_FOLDER = "$profile:CFCloud_Bridge\\";
const string CFCLOUD_BRIDGE_CONFIG_DIR = CFCLOUD_BRIDGE_ROOT_FOLDER + "Config\\";
const string CFCLOUD_BRIDGE_CONFIG_FILE = CFCLOUD_BRIDGE_CONFIG_DIR + "Settings.json";
const string CFCLOUD_BRIDGE_LOG_DIR = CFCLOUD_BRIDGE_ROOT_FOLDER + "Logs\\";
