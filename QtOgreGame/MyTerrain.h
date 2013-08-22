#pragma once
#include "stdafx.h"
class MyBuff;
class MyPlayer;
class MyArea
{
	friend class MyTerrain;
protected:
	int				m_iAreaBelong;
	int				m_nArmyCount;
	int				m_id;
	int				m_bHighLight;

	Ogre::Vector2	m_MidPoint;
	Ogre::SceneNode	*m_Node;
	Ogre::Entity	*m_FlagEntity;
	Ogre::SceneNode	*m_FlagNode;
	Ogre::Entity	*m_ArmyEntity;
	Ogre::SceneNode	*m_ArmyNode;
	Ogre::Entity	*m_Sword1,*m_Sword2;


	std::set<MyArea*> m_AdjacencySet;

	std::vector<MyBuff*> m_BuffVector;

public: 
	MyArea();
	
	int GetID();
	void SetAreaBelongID(int id);
	int GetAreaBelongID();
	void SetAreaBelong(MyPlayer *player);
	MyPlayer *GetAreaBelong();
	void SetArmyCount(int n);
	int GetArmyCount();
	Ogre::SceneNode *GetSceneNode();
	Ogre::SceneNode *GetArmySceneNode();
	Ogre::Entity	*GetArmyEntity();
	Ogre::Entity	*GetSword(int id);

	void AddBuff(MyBuff *buff);
	void RemoveBuff(MyBuff *buff);
	MyBuff *GetBuffByID(int id);
	int GetBuffCount();
	bool IsAdjacencyArea(MyArea *area);

	static void DefineInLua(lua_State *L);
	
};
class MyTerrain
{
	static MyTerrain			*m_pSingleton;


	Ogre::SceneManager			*m_pSceneMgr;
	Ogre::TerrainGlobalOptions	*m_pTerrainGlobals;  
	Ogre::TerrainGroup			*m_pTerrainGroup;
	Ogre::Light					*l;

	NxOgre::World				*m_pWorld;
	NxOgre::Scene				*m_pScene;
	Critter::RenderSystem		*m_pRenderSystem;
	Hydrax::Hydrax				*m_pHydrax;


	int							m_nAreaCount;
	std::vector<MyArea*>		m_AreaVector;

	QImage						m_EdgeImage;
	QImage						m_AreaImage;

	void UpdateTexture();
	void InitTerrain();
	void InitHydrax();
	void InitBlendMap(Ogre::Terrain *terrain);
	void InitArea();
	void InitNxOgre();



	NxOgre::SceneGeometry* InitNxOgreTerrain(Ogre::Terrain* terrain);
	void ReadAreaRelationship();
public:
	MyTerrain(Ogre::SceneManager *mgr);
	static MyTerrain&	GetSingleton();


	void SetAreaHighLight(int id,bool enable=true);
	int GetSelectAreaID(Ogre::Ray ray);
	MyArea *GetArea(int id);
	int	GetAreaCount();
	
	//x=0~1,y=0~1
	int GetAreaIDFromImageF(float x,float y);
	Ogre::Vector2 GetWorldPointFromImagePoint(Ogre::Vector2 point); 
	float GetWorldXFromImageX(float x); 
	float GetWorldYFromImageY(float y); 
	float GetImageXFromWorldX(float x); 
	float GetImageYFromWorldY(float y); 
	Critter::RenderSystem *GetRenderSystem();

	Ogre::SceneManager	*GetSceneManager();
	Ogre::TerrainGroup	*GetTerrainGroup();

	float Update(const Ogre::FrameEvent& evt);
	void InitArmy();
	static void DefineInLua(lua_State *L);
	~MyTerrain();
	
};

