#include "stdafx.h"
#include "MyTransformer.h"
#include "MyTerrain.h"
MyTransformerManager *MyTransformerManager::m_pSingleton=0;
MyTransformer::MyTransformer():m_bFinish(true),m_bEnable(true)
{
	MyTransformerManager::GetSingleton().AddNewTransformer(this);
}

MyTransformer::~MyTransformer()
{
}

bool MyTransformer::IsFinished()
{
	return m_bFinish;
}

void MyTransformer::Release()
{
	MyTransformerManager::GetSingleton().RemoveTransformer(this);
}

void MyTransformer::SetEnable( bool enable/*=true*/ )
{
	m_bEnable=enable;
}

bool MyTransformer::GetEnable()
{
	return m_bEnable;
}

MyWindowTransformer::MyWindowTransformer( CEGUI::Window *window ):MyTransformer(),m_pWindow(window)
{
	m_Position=window->getPosition();
}

void MyWindowTransformer::MoveToInTime( CEGUI::UVector2 dest,int msec )
{
	CEGUI::UVector2 speed;
	speed.d_x.d_offset=(dest.d_x-m_Position.d_x).d_offset/msec;
	speed.d_x.d_scale=(dest.d_x-m_Position.d_x).d_scale/msec;
	speed.d_y.d_offset=(dest.d_y-m_Position.d_y).d_offset/msec;
	speed.d_y.d_scale=(dest.d_y-m_Position.d_y).d_scale/msec;

	MoveToAtSpeed(dest,speed);
}

void MyWindowTransformer::MoveToAtSpeed( CEGUI::UVector2 dest,CEGUI::UVector2 speed )
{
	m_bFinish=false;
	m_Destination=dest;
	m_Speed=speed;
}

void MyWindowTransformer::Update( float timeDelta )
{
	if (m_bFinish)
	{
		return;
	}
	CEGUI::UVector2 destance=m_Destination-m_Position;
	if(
		(
		abs(destance.d_x.d_offset)<=abs((m_Speed*timeDelta).d_x.d_offset)&&
		abs(destance.d_x.d_scale)<=abs((m_Speed*timeDelta).d_x.d_scale)
		)||(
		abs(destance.d_y.d_offset)<=abs((m_Speed*timeDelta).d_y.d_offset)&&
		abs(destance.d_y.d_scale)<=abs((m_Speed*timeDelta).d_y.d_scale)
		)
		)
	{
		m_Position=m_Destination;
		m_bFinish=true;
	}
	else
	{
		m_Position+=m_Speed*timeDelta;
	}
	m_pWindow->setPosition(m_Position);
}

CEGUI::Window * MyWindowTransformer::GetWindow()
{
	return m_pWindow;
}

MyWindowTransformer * MyWindowTransformer::Create( CEGUI::Window *window )
{
	MyWindowTransformer *transformer=new MyWindowTransformer(window);
	return transformer;
}

MyTransformerManager::MyTransformerManager()
{
	if (m_pSingleton)
	{
		throw "Error";
	}
	m_pSingleton=this;
}

MyTransformerManager::~MyTransformerManager()
{
	for(std::set<MyTransformer*>::iterator it=m_TransformerSet.begin();it!=m_TransformerSet.end();it++)
	{
		delete (*it);
	}
}

MyTransformerManager & MyTransformerManager::GetSingleton()
{
	return *m_pSingleton;
}

void MyTransformerManager::AddNewTransformer( MyTransformer *transform )
{
	m_TransformerSet.insert(transform);
}

void MyTransformerManager::RemoveTransformer( MyTransformer *transform )
{
	m_TransformerSet.erase(transform);
	delete transform;
}

void MyTransformerManager::Update( float timeDelta )
{
	for (std::set<MyTransformer*>::iterator it=m_TransformerSet.begin();it!=m_TransformerSet.end();it++)
	{
		(*it)->Update(timeDelta);
	}
}

MySceneNodeTransformer::MySceneNodeTransformer( Ogre::SceneNode *node ):MyTransformer(),m_pNode(node)
{
	m_Position=node->getPosition();
	m_Orientation=node->getOrientation();
}

MySceneNodeTransformer * MySceneNodeTransformer::Create( Ogre::SceneNode *node )
{
	MySceneNodeTransformer *tran=new MySceneNodeTransformer(node);
	return tran;
}

void MySceneNodeTransformer::MoveToInTime( Ogre::Vector3 dest,float time )
{
	float speed=(dest-m_Position).length()/time;
	MoveToAtSpeed(dest,speed);
}

void MySceneNodeTransformer::MoveToAtSpeed( Ogre::Vector3 dest,float speed )
{
	m_Destination=dest;
	m_Speed=speed;
	m_Direction=m_Destination-m_Position;
	m_Direction.normalise();
	m_bFinish=false;
	if (m_bAutoTurn)
	{
		Ogre::Vector3 dir=m_Direction;
		if(m_bOnTerrain)
		{
			dir.y=0;
		}
		m_pNode->setOrientation(m_Forward.getRotationTo(dir));
	}
}

Ogre::SceneNode * MySceneNodeTransformer::GetSceneNode()
{
	return m_pNode;
}

void MySceneNodeTransformer::Update( float timeDelta )
{
	if(!m_bEnable)
		return;
	if((m_Destination-m_Position).length()<=m_Speed*timeDelta)
	{
		m_Position=m_Destination;
		m_bFinish=true;
	}
	else
	{
		m_Position+=m_Speed*m_Direction*timeDelta;
	}
	Ogre::Vector3 pos=m_Position;
	if(m_bOnTerrain)
		pos.y=MyTerrain::GetSingleton().GetTerrainGroup()->getHeightAtWorldPosition(pos.x,1000,pos.z)+m_fHeightOffset;
	m_pNode->setPosition(pos);
}

void MySceneNodeTransformer::SetOnTerrain( bool onfloor/*=true*/,float heightOffset/*=0*/ )
{
	m_bOnTerrain=onfloor;
	m_fHeightOffset=heightOffset;
}

void MySceneNodeTransformer::SetAutoTurn( bool autoTurn/*=true*/ )
{
	m_bAutoTurn=autoTurn;
}

void MySceneNodeTransformer::SetForwardVector( Ogre::Vector3 forward )
{
	m_Forward=forward;
}

Ogre::Vector3 MySceneNodeTransformer::GetDirection()
{
	return m_Direction;
}

MyEntityTransformer::MyEntityTransformer( Ogre::Entity *entity )
	:MyTransformer(),m_pEntity(entity),m_fRestTime(0)
{
	
}

MyEntityTransformer * MyEntityTransformer::Create( Ogre::Entity *entity )
{
	MyEntityTransformer *tran=new MyEntityTransformer(entity);
	return tran;
}

void MyEntityTransformer::AddAnimation( char *animName,bool loop/*=true*/ )
{
	Ogre::AnimationState *state=m_pEntity->getAnimationState(animName);
	state->setLoop(loop);
	state->setEnabled(true);
	m_pAnimaStateSet.insert(state);
}

void MyEntityTransformer::Update( float timeDelta )
{
	if(m_fRestTime>0)
	{
		m_fRestTime-=timeDelta;
		for(std::set<Ogre::AnimationState*>::iterator it=m_pAnimaStateSet.begin();it!=m_pAnimaStateSet.end();it++)
		{
			(*it)->addTime(timeDelta);
		}
	}
	else
	{
		m_bFinish=true;
	}
}

void MyEntityTransformer::Start( bool frombegin/*=true*/ )
{
	for(std::set<Ogre::AnimationState*>::iterator it=m_pAnimaStateSet.begin();it!=m_pAnimaStateSet.end();it++)
	{
		(*it)->setEnabled(true);
		if(frombegin)
			(*it)->setTimePosition(0);
	}
	m_bFinish=false;
}

void MyEntityTransformer::Stop( bool setbegin/*=true*/ )
{
	for(std::set<Ogre::AnimationState*>::iterator it=m_pAnimaStateSet.begin();it!=m_pAnimaStateSet.end();it++)
	{
		(*it)->setEnabled(false);
		if(setbegin)
			(*it)->setTimePosition(0);
	}
}

void MyEntityTransformer::PlayInTime( float time )
{
	m_fRestTime=time;
	Start();
}

void MyEntityTransformer::RemoveAnimation( char *animName )
{
	Ogre::AnimationState *state=m_pEntity->getAnimationState(animName);
	state->setEnabled(false);
	m_pAnimaStateSet.erase(state);
}

void MyEntityTransformer::RemoveAllAnimations()
{
	for(std::set<Ogre::AnimationState*>::iterator it=m_pAnimaStateSet.begin();it!=m_pAnimaStateSet.end();it++)
	{
		(*it)->setEnabled(false);
	}
	m_pAnimaStateSet.clear();
}