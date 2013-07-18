#include "stdafx.h"
#include "Global.h"
float Global::Gravity;
float Global::DiceSpeed;
float Global::MoveSpeed;
float Global::CardWidthScale;
float Global::CardChooseHeight;
using namespace Global;
void Global::LoadGlobleSetting()
{
	lua_State* L = lua_open();
	luaopen_base(L);
	luaL_openlibs(L);
	lua_tinker::dofile(L,"./media/lua/Global.lua");

	Gravity=lua_tinker::get<float>(L,"Gravity");
	DiceSpeed=lua_tinker::get<float>(L,"DiceSpeed");
	MoveSpeed=lua_tinker::get<float>(L,"MoveSpeed");
	CardWidthScale=lua_tinker::get<float>(L,"CardWidthScale");
	CardChooseHeight=lua_tinker::get<float>(L,"CardChooseHeight");

	lua_close(L);
}