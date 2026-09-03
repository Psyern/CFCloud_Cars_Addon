modded class MissionServer
{
	override void OnInit()
	{
		super.OnInit();

		CFCloud_Bridge_Manager.GetInstance().Init(true);
	}

	override void OnUpdate(float timeslice)
	{
		super.OnUpdate(timeslice);

		CFCloud_Bridge_Manager.GetInstance().OnUpdate(timeslice);
	}

	override void OnMissionFinish()
	{
		CFCloud_Bridge_Manager.GetInstance().Shutdown();

		super.OnMissionFinish();
	}
}
