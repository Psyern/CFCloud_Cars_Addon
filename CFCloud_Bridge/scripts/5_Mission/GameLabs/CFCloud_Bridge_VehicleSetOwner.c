#ifdef GAMELABS
#ifdef EXPANSIONMODVEHICLE

// Reassigns vehicle ownership.
//
// The target player has to be ONLINE. Expansion stores the owner as
// PlayerIdentity.GetId() - the DayZ UID - and that value cannot be derived
// from a SteamID64 by script; only a connected PlayerIdentity carries both
// (GetId() and GetPlainId()). Writing a SteamID64 into the owner field would
// produce an owner that IsOwner() never matches, so this action resolves a
// real identity or refuses.
class CFCloud_Bridge_VehicleSetOwner extends CFCloud_Bridge_VehicleActionBase
{
	void CFCloud_Bridge_VehicleSetOwner()
	{
		actionCode = "CFCloudBridge_VehicleSetOwner";
		actionName = "Fahrzeug-Besitzer setzen";
		actionIcon = "user-edit";
		actionColour = "warning";
		actionContext = "vehicle";

		parameters.Insert("steamId", GameLabsActionParameter("SteamID64", "Neuer Besitzer. Der Spieler muss online sein.", "string"));
	}

	override bool Execute(GameLabsActionContext context)
	{
		Object obj;
		ExpansionVehicle vehicle;

		if (!CFCloud_Bridge_Resolve(context, obj, vehicle))
			return false;

		CFCloud_Bridge_Settings settings = CFCloud_Bridge_Manager.GetInstance().GetSettings();
		if (!settings.m_AllowSetOwner)
		{
			CFCloud_Bridge_Logger.Warning("Set owner refused, disabled in Settings.json. " + CFCloud_Bridge_Describe(vehicle, obj));
			return false;
		}

		string steamId = context.parameters.Get("steamId").GetString();
		if (steamId == "")
		{
			CFCloud_Bridge_Logger.Warning("Set owner refused, no SteamID64 given. " + CFCloud_Bridge_Describe(vehicle, obj));
			return false;
		}

		array<Man> players = new array<Man>;
		g_Game.GetPlayers(players);

		PlayerIdentity target = null;
		foreach (Man candidate : players)
		{
			PlayerIdentity identity = candidate.GetIdentity();
			if (!identity)
				continue;

			if (identity.GetPlainId() == steamId)
			{
				target = identity;
				break;
			}
		}

		if (!target)
		{
			CFCloud_Bridge_Logger.Warning("Set owner refused, no online player with SteamID64 " + steamId + ". " + CFCloud_Bridge_Describe(vehicle, obj));
			return false;
		}

		// When a keychain is attached, the owner lives on the keychain and the
		// vehicle's own getters delegate to it. Writing only one of the two
		// leaves the game showing the old owner. Expansion's ResetKeyPairing()
		// branches the same way.
		ExpansionKeyChainBase keychain = ExpansionKeyChainBase.Cast(vehicle.GetEntity().GetAttachmentByType(ExpansionKeyChainBase));
		if (keychain)
			keychain.Expansion_AssignOwner(target);
		else
			vehicle.AssignOwner(target);

		CFCloud_Bridge_Logger.Warning("Owner set to " + target.GetName() + " (" + target.GetId() + "). " + CFCloud_Bridge_Describe(vehicle, obj));
		return true;
	}
}

#endif
#endif
