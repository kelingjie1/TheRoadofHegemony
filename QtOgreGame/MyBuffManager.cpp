#include "stdafx.h"
#include "MyBuffManager.h"
#include "MyTerrain.h"
#include "MyCardManager.h"
#include "MyGameStateManager.h"
MyBuffManager *MyBuffManager::m_pSingleton=0;

MyBuffType::MyBuffType( const char *name )
{
	L = lua_open();
	luaopen_base(L);
	luaL_openlibs(L);

	MyPlayer::DefineInLua(L);
	MyArea::DefineInLua(L);
	MyTerrain::DefineInLua(L);
	MyEventInfo::DefineInLua(L);
	MyGameStateManager::DefineInLua(L);
	MyBuff::DefineInLua(L);
	MyBuffManager::DefineInLua(L);
	MyCard::DefineInLua(L);
	MyCardManager::DefineInLua(L);

	lua_tinker::set(L,"Terrain",&MyTerrain::GetSingleton());
	lua_tinker::set(L,"EventInfo",&MyEventInfo::GetSingleton());
	lua_tinker::set(L,"GameStateManager",&MyGameStateManager::GetSingleton());
	lua_tinker::set(L,"BuffManager",&MyBuffManager::GetSingleton());
	lua_tinker::set(L,"CardManager",&MyCardManager::GetSingleton());

	lua_tinker::dofile(L, ("./media/lua/Buff/"+std::string(name)+".lua").c_str());
	m_Name=lua_tinker::get<char*>(L,"Name");
	m_Description=lua_tinker::get<char*>(L,"Description");
	m_ImageName=lua_tinker::get<char*>(L,"ImageName");
}

MyBuffType::~MyBuffType()
{
	lua_close(L);
}

const char * MyBuffType::GetImageName()
{
	return m_ImageName.c_str();
}

const char * MyBuffType::GetName()
{
	return m_Name.c_str();
}

const char * MyBuffType::GetDescription()
{
	return m_Description.c_str();
}


bool MyBuff::Trigger( const char *event )
{
	int re=lua_tinker::call<int>(m_pType->L,event);
	return re;
}

void MyBuff::Register()
{
	lua_tinker::set(m_pType->L,"Buff",this);
	lua_tinker::call<void>(m_pType->L,"Register");
}

void MyBuff::DefineInLua( lua_State *l )
{
	lua_tinker::class_add<MyBuff>(l,"MyBuff");
	lua_tinker::class_def<MyBuff>(l,"Register",&MyBuff::Register);
	lua_tinker::class_def<MyBuff>(l,"Trigger",&MyBuff::Trigger);
}

MyBuffType * MyBuff::GetBuffType()
{
	return m_pType;
}


MyBuffManager::MyBuffManager()
{
	if(m_pSingleton)
		throw "Error";
	else
		m_pSingleton=this;

	lua_State* L = lua_open();
	luaopen_base(L);
	luaL_openlibs(L);
	MyBuffManager::DefineInLua(L);
	lua_tinker::set(L,"BuffManager",this);
	lua_tinker::dofile(L, "./media/lua/Buff/Type.lua");
	lua_close(L);
}

MyBuffManager::~MyBuffManager()
{

}

MyBuffManager& MyBuffManager::GetSingleton()
{
	return *m_pSingleton;
}

void MyBuffManager::StateTrigger( char *event )
{
	std::list<MyBuff*> &bufflist=m_BuffRegisterMap[event];
	std::list<MyBuff*>::iterator it=bufflist.begin();
	while (it!=bufflist.end())
	{
		if((*it)->Trigger(event))
		{
			bufflist.erase(it++);
		}
		else
		{
			it++;
		}
	}
}

void MyBuffManager::DefineInLua( lua_State *L )
{
	lua_tinker::class_add<MyBuffManager>(L,"MyBuffManager");
	lua_tinker::class_def<MyBuffManager>(L,"AddNewBuffType",&MyBuffManager::AddNewBuffType);
	lua_tinker::class_def<MyBuffManager>(L,"CreateNewBuff",&MyBuffManager::CreateNewBuff);
	lua_tinker::class_def<MyBuffManager>(L,"GetBuffType",&MyBuffManager::GetBuffType);
	lua_tinker::class_def<MyBuffManager>(L,"RegisterStateTrigger",&MyBuffManager::RegisterStateTrigger);
}

void MyBuffManager::AddNewBuffType( const char *name )
{
	m_BuffTypeMap[name]=new MyBuffType(name);
}

MyBuffType * MyBuffManager::GetBuffType( const char *name )
{
	return m_BuffTypeMap[name];
}

void MyBuffManager::RegisterStateTrigger( MyBuff *buff,char *event )
{
	m_BuffRegisterMap[event].push_back(buff);
}

MyBuff *MyBuffManager::CreateNewBuff( const char *bufftype )
{
	MyBuff *buff=new MyBuff;
	buff->m_pType=m_BuffTypeMap[bufftype];
	m_BuffList.push_back(buff);
	buff->Register();
	return buff;
}

