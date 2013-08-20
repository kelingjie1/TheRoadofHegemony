#pragma once
#include "stdafx.h"

class MyArea;
class MyPlayer;
class MyBuffType
{
	friend class MyBuffManager;
	friend class MyBuff;
	std::string m_Name;
	std::string m_Description;
	std::string m_ImageName;
	lua_State *L;

public:
	MyBuffType(const char *name);
	~MyBuffType();

	const char *GetImageName();
	const char *GetName();
	const char *GetDescription();
};
class MyBuff
{
	friend class MyBuffManager;
	MyBuff();
	MyBuff(const MyBuff&){}

	MyBuffType* m_pType;
	MyArea		*m_pOwnerArea;
	MyPlayer	*m_pOwnerPlayer;
public:
	//����������֮��Ӧ��ɾ��������true

	MyBuffType *GetBuffType();
	
	void SetOwnerArea(MyArea *area);
	void SetOwnerPlayer(MyPlayer *player);
	MyArea *GetOwnerArea();
	MyPlayer *GetOwnerPlayer();

	void Register();
	bool Trigger(const char *event);

	static void DefineInLua(lua_State *l);
};
class MyBuffManager
{
public:
	/*
	���еĴ����¼��б����¼�֮ǰ��Before��֮���After��
	�������¼�X�������BeforeX��AfterX
	����л���PlayerChange
	��Ƭ��ʹ�ã�CardUse
	�����ӣ�Dice
	ս����ʼ��Fight
	ս��ʤ����FightWin
	ս��ʧ�ܣ�FightLose
	�ܵ�������UnderAttack
	����������Attack
	������Move
	���죺Build
	��Ϸʤ����GameWin
	��Ϸʧ�ܣ�GameLose
	*/
// 	enum TriggerEvent
// 	{
// 		BeforePlayerChange=0,
// 		BeforeCardUse=1,
// 		BeforeDice=2,
// 		BeforeFight=3,
// 		BeforeFightWin=4,
// 		BeforeFightLose=5,
// 		BeforeUnderAttack=6,
// 		BeforeAttack=7,
// 		BeforeMove=8,
// 		BeforeBuild=9,
// 		BeforeGameWin=10,
// 		BeforeGameLose=11,
// 
// 		AfterPlayerChange=100,
// 		AfterCardUse=101,
// 		AfterDice=102,
// 		AfterFight=103,
// 		AfterFightWin=104,
// 		AfterFightLose=105,
// 		AfterUnderAttack=106,
// 		AfterAttack=107,
// 		AfterMove=108,
// 		AfterBuild=109,
// 		AfterGameWin=110,
// 		AfterGameLose=111
// 	};
protected:
	static MyBuffManager						*m_pSingleton;
	std::map<std::string,MyBuffType*>			m_BuffTypeMap;
	std::list<MyBuff*>							m_BuffList;
	std::map<std::string,std::list<MyBuff*>>	m_BuffRegisterMap;
public:
	
	

	static MyBuffManager& GetSingleton();

	MyBuffManager();
	~MyBuffManager();
	void AddNewBuffType(const char *name);
	MyBuffType *GetBuffType(const char *name);
	
	void RegisterStateTrigger(MyBuff *buff,char *event);
	MyBuff *CreateNewBuff(const char *bufftype);
	
	void StateTrigger(char *event);

	static void DefineInLua(lua_State *L);
};
