class CfgPatches
{
	class CFCloud_Bridge_Scripts
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		// GameLabs_Scripts is NOT a "nice to have" here - it pins the load order.
		// This mod does "modded class MissionServer { override void
		// GLActionRegisterHook() }", and that method only exists once GameLabs'
		// own modded MissionServer has been applied. Without this entry our class
		// is merged first and the server dies with
		//   "Function 'GLActionRegisterHook' is marked as override, but there is
		//    no function with this name in the base class".
		// GameLabs uses the exact same anchor for its own Z_Dependencies addon.
		requiredAddons[] = {"DZ_Data", "GameLabs_Scripts"};
	};
};

class CfgMods
{
	class CFCloud_Bridge
	{
		dir = "CFCloud_Bridge";
		picture = "";
		action = "";
		hideName = 0;
		hidePicture = 1;
		name = "CFCloud Bridge";
		credits = "Deadman's Echo";
		version = "0.2.1";
		author = "Deadman's Echo";
		authorID = "0";
		extra = 0;
		type = "mod";
		defines[] = {};
		dependencies[] = {"Game", "World", "Mission"};

		class defs
		{
			class gameScriptModule
			{
				value = "";
				files[] = {"CFCloud_Bridge/scripts/3_Game"};
			};
			class worldScriptModule
			{
				value = "";
				files[] = {"CFCloud_Bridge/scripts/4_World"};
			};
			class missionScriptModule
			{
				value = "";
				files[] = {"CFCloud_Bridge/scripts/5_Mission"};
			};
		};
	};
};
