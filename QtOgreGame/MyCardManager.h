#pragma once
#include "stdafx.h"
class MyPlayer;
class MyArea;
class MyBuffType
{
	std::string m_Name;
	std::string m_Description;
	lua_State *L;
	std::string m_BuffImage;
public:
	MyBuffType(const char *name);
	~MyBuffType();
};
class MyCardType
{
	friend class MyCard;
	std::string m_Name;
	std::string m_Description;
	std::string m_ImageName;
	lua_State *L;
public:
	MyCardType(const char *name);
	~MyCardType();

	std::string GetImageName();
};
class MyBuff
{
	MyBuff(){}
	MyBuff(const MyBuff&){}

	MyBuffType* m_pType;
public:
	//如果触发完毕之后应该删除，返回true
	bool Trigger();
};
class MyCard
{
	friend class MyCardManager;
	MyCard();
	MyCard(const MyCard&){}

	bool m_bChoosed;
	MyPlayer	*m_pOwener;
	MyCardType	*m_pType;
public:
	bool Use();
	MyCardType* GetCardType();
	bool IsChoosed();
	void SetChoosed(bool choose=true,bool update=true);
	void SetOwenerByID(int id);
	static void DefineInLua(lua_State *L);
};
class MyCardManager
{
	static MyCardManager			*m_pSingleton;
	std::map<QString,MyCardType*>	m_CardTypeMap;
	std::list<MyCard*>				m_CardList;
public:
	static MyCardManager& GetSingleton();

	MyCardManager();
	~MyCardManager();

	std::list<MyCard*> &GetCardList();
	void AddNewCardType(const char *name);
	MyCard *CreateNewCard(const char *TypeName);
	MyCardType *GetCardType(const char *name);
	void ShowPlayerCard(MyPlayer *player);

	static void DefineInLua(lua_State *L);
};
class MyBuffManager
{
	static MyBuffManager *m_pSingleton;
	std::map<QString,MyBuffType*> m_BuffTypeMap;
	std::map<QString,std::list<MyBuff*>> m_BuffList;
public:
	static MyBuffManager& GetSingleton();

	MyBuffManager();
	~MyBuffManager();
	void AddNewBuffType(const char *name);
	MyBuffType *GetBuffType(const char *name);
	
	
	/*
	所有的触发事件列表：在事件之前加Before，之后加After，
	比如有事件X，则存在BeforeX，AfterX
	玩家切换：PlayerChange
	卡片被使用：CardUse
	扔骰子：Dice
	战斗开始：Fight
	战斗胜利：FightWin
	战斗失败：FightLose
	受到攻击：UnderAttack
	发动攻击：Attack
	调兵：Move
	建造：Build
	游戏胜利：GameWin
	游戏失败：GameLose
	*/
	void StateTrigger(const char *name);

	static void DefineInLua(lua_State *L);
};
