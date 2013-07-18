#include "StdAfx.h"
#include "MyCardManager.h"
#include "MyGameStateManager.h"
#include "MyUIUpdater.h"
MyCardManager *MyCardManager::m_pSingleton=0;
MyBuffManager *MyBuffManager::m_pSingleton=0;

MyBuffType::MyBuffType( const char *name )
{
	L = lua_open();
	luaopen_base(L);
	luaL_openlibs(L);
	lua_tinker::dofile(L, ("./media/lua/Buff/"+std::string(name)+".lua").c_str());
	m_Name=lua_tinker::get<char*>(L,"Name");
	m_Description=lua_tinker::get<char*>(L,"Description");
}

MyBuffType::~MyBuffType()
{
	lua_close(L);
}


MyCardType::MyCardType( const char *name )
{
	L = lua_open();
	luaopen_base(L);
	luaL_openlibs(L);
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

bool MyBuff::Trigger()
{
	return false;
}

MyCard::MyCard():m_bChoosed(0)
{

}

void MyCard::Use()
{

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
}

bool MyCard::IsChoosed()
{
	return m_bChoosed;
}

void MyCard::SetChoosed( bool choose/*=true*/ )
{
	m_bChoosed=choose;
	MyUIUpdater::GetSingleton().on_CardChange();
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




MyBuffManager::MyBuffManager()
{
	if(m_pSingleton)
		throw "Error";
	else
		m_pSingleton=this;
}

MyBuffManager::~MyBuffManager()
{

}

MyBuffManager& MyBuffManager::GetSingleton()
{
	return *m_pSingleton;
}

void MyBuffManager::StateTrigger( const char *name )
{
	std::list<MyBuff*> &bufflist=m_BuffList[name];
	std::list<MyBuff*>::iterator it=bufflist.begin();
	while (it!=bufflist.end())
	{
		if((*it)->Trigger())
		{
			bufflist.erase(it++);
		}
	}
}

void MyBuffManager::DefineInLua( lua_State *L )
{
	lua_tinker::class_add<MyBuffManager>(L,"MyBuffManager");
	lua_tinker::class_def<MyBuffManager>(L,"AddNewBuffType",&MyBuffManager::AddNewBuffType);

}

void MyBuffManager::AddNewBuffType( const char *name )
{
	m_BuffTypeMap[name]=new MyBuffType(name);
}

MyBuffType * MyBuffManager::GetBuffType( const char *name )
{
	return m_BuffTypeMap[name];
}

