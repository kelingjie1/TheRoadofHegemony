#pragma once
#include "stdafx.h"
class MyTransformer
{
protected:
	bool			m_bFinish;
	bool			m_bEnable;
public:
	MyTransformer();
	~MyTransformer();
	void SetEnable(bool enable=true);
	bool GetEnable();
	bool IsFinished();
	virtual void Update(float timeDelta)=0;
	void Release();
};

class MyCameraTransformer:public MyTransformer
{

};
class MyWindowTransformer:public MyTransformer
{
protected:
	MyWindowTransformer(MyWindowTransformer&){}
	MyWindowTransformer(CEGUI::Window *window);
	CEGUI::Window *m_pWindow;
	CEGUI::UVector2 m_Position;
	CEGUI::UVector2 m_Destination;
	CEGUI::UVector2 m_Speed;
public:
	static MyWindowTransformer *Create(CEGUI::Window *window);
	void MoveToInTime(CEGUI::UVector2 dest,int msec);
	void MoveToAtSpeed(CEGUI::UVector2 dest,CEGUI::UVector2 speed);
	CEGUI::Window *GetWindow();
	
	virtual void Update(float timeDelta);
};
class MySceneNodeTransformer:public MyTransformer
{
protected:
	MySceneNodeTransformer(Ogre::SceneNode *node);
	MySceneNodeTransformer(MySceneNodeTransformer&){}
	Ogre::SceneNode			*m_pNode;
	Ogre::Vector3			m_Destination;
	Ogre::Vector3			m_Direction;
	Ogre::Vector3			m_Forward;
	Ogre::Vector3			m_Position;
	Ogre::Quaternion		m_Orientation;
	float					m_fHeightOffset;
	float					m_Speed;
	bool					m_bOnTerrain;
	bool					m_bAutoTurn;
public:
	static MySceneNodeTransformer *Create(Ogre::SceneNode *node);
	void SetOnTerrain(bool onfloor=true,float heightOffset=0);
	void SetAutoTurn(bool autoTurn=true);
	void SetForwardVector(Ogre::Vector3 forward);
	void MoveToInTime(Ogre::Vector3 dest,float time);
	void MoveToAtSpeed(Ogre::Vector3 dest,float speed);
	Ogre::SceneNode *GetSceneNode();
	virtual void Update(float timeDelta);
};

class MyEntityTransformer:public MyTransformer
{
protected:
	MyEntityTransformer(Ogre::Entity *entity);
	Ogre::Entity						*m_pEntity;
	std::list<Ogre::AnimationState*>	m_pAnimaStateList;
	float								m_fRestTime;
public:
	static MyEntityTransformer *Create(Ogre::Entity *entity);
	void PlayAnimation(char *animName,bool loop=true);
	void PlayInTime(float time);
	void Start(bool frombegin=true);
	void Stop(bool setbegin=true);
	virtual void Update(float timeDelta);
};

class MyTransformerManager
{
	static MyTransformerManager *m_pSingleton;
	std::set<MyTransformer*> m_TransformerSet;
public:
	MyTransformerManager();
	~MyTransformerManager();

	static MyTransformerManager &GetSingleton();

	void AddNewTransformer(MyTransformer *transform);
	void RemoveTransformer(MyTransformer *transform);
	void Update(float timeDelta);
};
