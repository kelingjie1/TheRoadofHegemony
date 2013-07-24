#pragma once
#include "stdafx.h"
class MyPlayer;
class MyArea;

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
