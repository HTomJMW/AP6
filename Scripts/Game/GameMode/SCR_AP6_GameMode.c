[EntityEditorProps(category: "GameScripted/AP6", description: "AP6 GameMode")]
class SCR_AP6_GameModeClass : SCR_BaseGameModeClass
{
};

class SCR_AP6_GameMode : SCR_BaseGameMode
{
	[Attribute(defvalue: "true", uiwidget: UIWidgets.CheckBox, desc: "Enables defender AIs near flags.", category: "AP6 GameMode")]
	protected bool m_EnableAIs;
	
	protected string version = "0.7.51";
	
	protected ref array<SCR_AP6_Flag> m_flags = {};
	
	protected ref map<int, int> m_playersDeathTimes = new map<int, int>();
	
	//------------------------------------------------------------------------------------------------
	protected override void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);
	}

	//------------------------------------------------------------------------------------------------
	static SCR_AP6_GameMode GetInstance()
	{
		return SCR_AP6_GameMode.Cast(GetGame().GetGameMode());
	}
	
	//------------------------------------------------------------------------------------------------
	void enableAIs(bool enable)
	{
		m_EnableAIs = enable;
	}
	
	//------------------------------------------------------------------------------------------------
	bool isAIsEnabled()
	{
		return m_EnableAIs;
	}

	//------------------------------------------------------------------------------------------------
	protected override void OnGameStart()
	{
		super.OnGameStart();

		m_flags.Insert(SCR_AP6_Flag.Cast(GetGame().GetWorld().FindEntityByName("Flag_1")));
		m_flags.Insert(SCR_AP6_Flag.Cast(GetGame().GetWorld().FindEntityByName("Flag_2")));
		m_flags.Insert(SCR_AP6_Flag.Cast(GetGame().GetWorld().FindEntityByName("Flag_3")));
		m_flags.Insert(SCR_AP6_Flag.Cast(GetGame().GetWorld().FindEntityByName("Flag_4")));
		m_flags.Insert(SCR_AP6_Flag.Cast(GetGame().GetWorld().FindEntityByName("Flag_5")));
		
		PrintFormat("AP6 :: Flags array: %1", m_flags);
		
		if (!m_RplComponent.IsProxy())
		{
			StartGameMode();
			PrintFormat("AP6 :: GameMode running: %1", IsRunning().ToString());
			Print("AP6 :: Mission version: " + version);

			if (RplSession.Mode() == RplMode.Dedicated)
			{
				Print("AP6 :: Server: Dedicated");
			} else {
				Print("AP6 :: Server: Player-Hosted");
			}
		}
	}
	
	//------------------------------------------------------------------------------------------------
	array<SCR_AP6_Flag> getAP6Flags()
	{
		return m_flags;
	}

	//------------------------------------------------------------------------------------------------
	override void OnPlayerDisconnected(int playerId, KickCauseCode cause, int timeout)
	{
		super.OnPlayerDisconnected(playerId, cause, timeout);
		
		m_playersDeathTimes.Remove(playerId);
	}

	//------------------------------------------------------------------------------------------------
	override void OnPlayerKilled(int playerId, IEntity playerEntity, IEntity killerEntity, notnull Instigator killer)
	{
		super.OnPlayerKilled(playerId, playerEntity, killerEntity, killer);
		
		int lastDeath;
		m_playersDeathTimes.Find(playerId, lastDeath);
		int currentMissionTime = GetElapsedTime();
		m_playersDeathTimes.Set(playerId, currentMissionTime);

		SCR_RespawnTimerComponent rtc = SCR_RespawnTimerComponent.Cast(this.FindComponent(SCR_RespawnTimerComponent));
		if (rtc)
		{
			switch (true)
			{
				case (currentMissionTime - lastDeath < 15): rtc.SetRespawnTime(playerId, 30); break;
				case (currentMissionTime - lastDeath < 30): rtc.SetRespawnTime(playerId, 20); break;
				default: rtc.SetRespawnTime(playerId, 15); break;
			}
		}
	}

	//------------------------------------------------------------------------------------------------
	void SCR_AP6_GameMode(IEntitySource src, IEntity parent)
	{
	}

	//------------------------------------------------------------------------------------------------
	void ~SCR_AP6_GameMode()
	{
		if (m_flags) m_flags.Clear();
		m_flags = null;

		if (m_playersDeathTimes) m_playersDeathTimes.Clear();
		m_playersDeathTimes = null;
	}
};