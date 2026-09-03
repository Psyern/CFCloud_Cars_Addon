class CfgPatches
{
	class CFCloud_Bridge_Scripts
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data"};
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
		version = "0.1.0";
		author = "Deadman's Echo";
		authorID = "0";
		extra = 0;
		type = "mod";
		defines[] = {};
		dependencies[] = {"Game", "Mission"};

		class defs
		{
			class gameScriptModule
			{
				value = "";
				files[] = {"CFCloud_Bridge/scripts/Common", "CFCloud_Bridge/scripts/3_Game"};
			};
			class missionScriptModule
			{
				value = "";
				files[] = {"CFCloud_Bridge/scripts/Common", "CFCloud_Bridge/scripts/5_Mission"};
			};
		};
	};
};
