#include "stdafx.h"
#include "Global.h"
float Global::Gravity;
float Global::DiceSpeed;
float Global::MoveSpeed;
float Global::CardWidthScale;
float Global::CardChooseHeight;
float Global::HeightOffsetForSinbadOnTerrain;
float Global::MoveDistance;
int Global::UseHydrax;
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
	MoveDistance=lua_tinker::get<float>(L,"MoveDistance");
	HeightOffsetForSinbadOnTerrain=lua_tinker::get<float>(L,"HeightOffsetForSinbadOnTerrain");
	CardWidthScale=lua_tinker::get<float>(L,"CardWidthScale");
	CardChooseHeight=lua_tinker::get<float>(L,"CardChooseHeight");
	UseHydrax=lua_tinker::get<float>(L,"UseHydrax");
	lua_close(L);
}

void Global::DefineOgreInLua( lua_State *L )
{
	lua_tinker::class_add<Ogre::Vector3>(L,"Ogre_Vector3");
	lua_tinker::class_con<Ogre::Vector3>(L,lua_tinker::constructor<float,float,float>);
	lua_tinker::class_def<Ogre::Vector3>(L,"absDotProduct",&Ogre::Vector3::absDotProduct);
	lua_tinker::class_def<Ogre::Vector3>(L,"angleBetween",&Ogre::Vector3::angleBetween);
	lua_tinker::class_def<Ogre::Vector3>(L,"crossProduct",&Ogre::Vector3::crossProduct);
	lua_tinker::class_def<Ogre::Vector3>(L,"directionEquals",&Ogre::Vector3::directionEquals);
	lua_tinker::class_def<Ogre::Vector3>(L,"distance",&Ogre::Vector3::distance);
	lua_tinker::class_def<Ogre::Vector3>(L,"dotProduct",&Ogre::Vector3::dotProduct);
	lua_tinker::class_def<Ogre::Vector3>(L,"getRotationTo",&Ogre::Vector3::getRotationTo);
	lua_tinker::class_def<Ogre::Vector3>(L,"isNaN",&Ogre::Vector3::isNaN);
	lua_tinker::class_def<Ogre::Vector3>(L,"isZeroLength",&Ogre::Vector3::isZeroLength);
	lua_tinker::class_def<Ogre::Vector3>(L,"length",&Ogre::Vector3::length);
	lua_tinker::class_def<Ogre::Vector3>(L,"makeCeil",&Ogre::Vector3::makeCeil);
	lua_tinker::class_def<Ogre::Vector3>(L,"makeFloor",&Ogre::Vector3::makeFloor);
	lua_tinker::class_def<Ogre::Vector3>(L,"midPoint",&Ogre::Vector3::midPoint);
	lua_tinker::class_def<Ogre::Vector3>(L,"normalise",&Ogre::Vector3::normalise);
	lua_tinker::class_def<Ogre::Vector3>(L,"normalisedCopy",&Ogre::Vector3::normalisedCopy);


	lua_tinker::class_add<Ogre::Camera>(L,"Ogre_Camera");
	lua_tinker::class_def<Ogre::Camera>(L,"setAutoTracking",&Ogre::Camera::setAutoTracking);


	lua_tinker::class_add<Ogre::AnimationTrack>(L,"Ogre_AnimationTrack");
	lua_tinker::class_def<Ogre::AnimationTrack>(L,"createKeyFrame",&Ogre::AnimationTrack::createKeyFrame);

	lua_tinker::class_add<Ogre::KeyFrame>(L,"Ogre_KeyFrame");
	//lua_tinker::class_add<Ogre::KeyFrame>(L,"",Ogre::KeyFrame);

}
