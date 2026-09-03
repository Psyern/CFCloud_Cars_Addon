#ifdef GAMELABS
#ifdef EXPANSIONMODVEHICLE

// Lives in 4_World, not next to the actions in 5_Mission, for two reasons:
// it needs nothing above 4_World (ExpansionVehicle is a 4_World Core class),
// and a lower layer is guaranteed to be compiled before its subclasses. In
// 5_Mission this file would sort alphabetically AFTER the actions deriving
// from it.
//
// Shared plumbing for every vehicle-context action in this mod:
// resolve the GameLabs context down to Expansion's lock object, and build one
// consistent log line. ExpansionVehicle.Get() is the same resolver Expansion's
// own unlock action uses, and it covers CarScript, BoatScript and
// ExpansionVehicleBase alike.
class CFCloud_Bridge_VehicleActionBase extends GameLabsContextAction
{
	protected bool CFCloud_Bridge_Resolve(GameLabsActionContext context, out Object obj, out ExpansionVehicle vehicle)
	{
		obj = null;
		vehicle = null;

		_Vehicle glVehicle;
		if (!_Vehicle.CastTo(glVehicle, context.GetReferencedObject()))
		{
			CFCloud_Bridge_Logger.Error(actionCode + ": context is not a vehicle.");
			return false;
		}

		obj = glVehicle.Ref();
		if (!obj)
		{
			CFCloud_Bridge_Logger.Error(actionCode + ": the referenced vehicle no longer exists.");
			return false;
		}

		if (!ExpansionVehicle.Get(vehicle, obj))
		{
			CFCloud_Bridge_Logger.Error(actionCode + ": " + obj.GetType() + " is not an Expansion vehicle.");
			return false;
		}

		return true;
	}

	protected string CFCloud_Bridge_YesNo(bool value)
	{
		if (value)
			return "yes";

		return "no";
	}

	protected string CFCloud_Bridge_Owner(ExpansionVehicle vehicle)
	{
		if (!vehicle.HasOwner())
			return "none";

		return vehicle.GetOwnerName() + " (" + vehicle.GetOwnerUID() + ")";
	}

	protected string CFCloud_Bridge_Describe(ExpansionVehicle vehicle, Object obj)
	{
		string lockState = typename.EnumToString(ExpansionVehicleLockState, vehicle.GetLockState());
		vector position = obj.GetPosition();

		string text = obj.GetType();
		text = text + " pos=" + position.ToString();
		text = text + " lock=" + lockState;
		text = text + " key=" + CFCloud_Bridge_YesNo(vehicle.HasKey());
		text = text + " id=" + vehicle.GetPersistentIDString();
		text = text + " owner=" + CFCloud_Bridge_Owner(vehicle);
		return text;
	}
}

#endif
#endif
