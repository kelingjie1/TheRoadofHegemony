#include "StdAfx.h"
#include "MyCardManager.h"
#include "MyBuffManager.h"
#include "MyGameStateManager.h"
#include "MyUIUpdater.h"
#include "MyTerrain.h"
#include "Global.h"
MyCardManager *MyCardManager::m_pSingleton=0;



MyCardType::MyCardType( const char *name )
{
	L = lua_open();
	luaopen_base(L);
	luaL_openlibs(L);
	MyPlayer::DefineInLua(L);
	MyArea::DefineInLua(L);
	MyTerrain::DefineInLua(L);
	MyGameStateManager::DefineInLua(L);
	MyBuff::DefineInLua(L);
	MyBuffManager::DefineInLua(L);
	MyCard::DefineInLua(L);
	MyCardManager::DefineInLua(L);

	lua_tinker::set(L,"Terrain",&MyTerrain::GetSingleton());
	lua_tinker::set(L,"GameStateManager",&MyGameStateManager::GetSingleton());
	lua_tinker::set(L,"BuffManager",&MyBuffManager::GetSingleton());
	lua_tinker::set(L,"CardManager",&MyCardManager::GetSingleton());

	lua_tinker::dofile(L, ("./media/lua/Card/"+std::string(name)+".lua").c_str());
	m_Name=lua_tinker::get<char*>(L,"Name");
	m_Description=lua_tinker::get<char*>(L,"Description");
	m_ImageName=lua_tinker::get<char*>(L,"ImageName");
}

MyCardType::~MyCardType()
{
	lua_close(L);
}

std::string MyCardType::GetImageName()
{
	return m_ImageName;
}

MyCard::MyCard():m_bChoosed(0),m_pOwener(0)
{

}

bool MyCard::Use()
{
	lua_State *L=m_pType->L;
	int re=lua_tinker::call<int>(L,"Use");
	if(re)
	{
		MyGameStateManager::GetSingleton().GetCurrentPlayer()->RemoveCard(this);
	}
	return re;
}
MyCardType* MyCard::GetCardType()
{
	return m_pType;
}

void MyCard::DefineInLua( lua_State *L )
{
	lua_tinker::class_add<MyCard>(L,"MyCard");
	lua_tinker::class_def<MyCard>(L,"Use",&MyCard::Use);
	lua_tinker::class_def<MyCard>(L,"GetCardType",&MyCard::GetCardType);
	lua_tinker::class_def<MyCard>(L,"IsChoosed",&MyCard::IsChoosed);
	lua_tinker::class_def<MyCard>(L,"SetChoosed",&MyCard::SetChoosed);
	lua_tinker::class_def<MyCard>(L,"SetOwenerByID",&MyCard::SetOwenerByID);
}

bool MyCard::IsChoosed()
{
	return m_bChoosed;
}

void MyCard::SetChoosed( bool choose/*=true*/,bool update/*=true*/ )
{
	m_bChoosed=choose;
	if (update)
	{
		MyUIUpdater::GetSingleton().on_CardChange();
	}
	
}

void MyCard::SetOwenerByID( int id )
{
	MyPlayer *player=MyGameStateManager::GetSingleton().GetCurrentPlayer();
	MyPlayer *newplayer=MyGameStateManager::GetSingleton().GetPlayer(id);
	player->RemoveCard(this,false);
	newplayer->AddCard(this);
}



MyCardManager::MyCardManager()
{
	if(m_pSingleton)
		throw "Error";
	else
		m_pSingleton=this;
	
	lua_State* L = lua_open();
	luaopen_base(L);
	luaL_openlibs(L);
	MyCardManager::DefineInLua(L);
	lua_tinker::set(L,"CardManager",this);
	lua_tinker::dofile(L, "./media/lua/Card/Type.lua");
	lua_close(L);
}


MyCardManager::~MyCardManager()
{
	for (std::map<QString,MyCardType*>::iterator it=m_CardTypeMap.begin();it!=m_CardTypeMap.end();it++)
	{
		delete it->second;
	}
	for (std::list<MyCard*>::iterator it=m_CardList.begin();it!=m_CardList.end();it++)
	{
		delete *it;
	}
}

MyCardManager& MyCardManager::GetSingleton()
{
	return *m_pSingleton;
}

void MyCardManager::AddNewCardType( const char *name )
{
	m_CardTypeMap[name]=new	MyCardType(name);
}

void MyCardManager::DefineInLua(lua_State *L)
{
	lua_tinker::class_add<MyCardManager>(L,"MyCardManager");
	lua_tinker::class_def<MyCardManager>(L,"AddNewCardType",&MyCardManager::AddNewCardType);
	lua_tinker::class_def<MyCardManager>(L,"CreateNewCard",&MyCardManager::CreateNewCard);
	lua_tinker::class_def<MyCardManager>(L,"GetCardType",&MyCardManager::GetCardType);
}

void MyCardManager::ShowPlayerCard( MyPlayer *player )
{

}

MyCardType * MyCardManager::GetCardType( const char *name )
{
	return m_CardTypeMap[name];
}

std::list<MyCard*> & MyCardManager::GetCardList()
{
	return m_CardList;
}

MyCard * MyCardManager::CreateNewCard( const char *TypeName )
{
	MyCard *card=new MyCard;
	card->m_pType=m_CardTypeMap[TypeName];
	m_CardList.push_back(card);
	return card;
}



