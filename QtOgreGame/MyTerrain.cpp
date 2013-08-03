#include "StdAfx.h"
#include "MyTerrain.h"
#include "MyGameStateManager.h"
#include "Global.h"
#define TERRAIN_WORLD_SIZE 2560.0f
#define TERRAIN_SIZE 257
#define TERRAIN_FILE_PREFIX Ogre::String("testTerrain")
#define TERRAIN_FILE_SUFFIX Ogre::String("dat")

MyTerrain* MyTerrain::m_pSingleton=0;
MyTerrain::MyTerrain(Ogre::SceneManager *mgr)
{
	if (m_pSingleton)
	{
		throw "singleton already exist";
	}
	m_pSingleton=this;
	m_pSceneMgr=mgr;


	InitTerrain();
	InitArea();
	ReadAreaRelationship();
	InitNxOgre();

// 	Ogre::SceneNode *node=m_pSceneMgr->getRootSceneNode()->createChildSceneNode("test");
// 	node->attachObject(m_pSceneMgr->createEntity("head","ogrehead.mesh"));

	m_pSceneMgr->setSkyBox(true, "Examples/EveningSkyBox");
	m_pSceneMgr->setShadowTechnique(Ogre::ShadowTechnique::SHADOWTYPE_TEXTURE_MODULATIVE);


}


MyTerrain::~MyTerrain()
{
	delete m_pTerrainGroup;
	delete m_pTerrainGlobals;
	m_pWorld->destroyWorld();
	m_pSingleton=0;
}


int MyTerrain::GetSelectAreaID( Ogre::Ray ray )
{
	Ogre::TerrainGroup::RayResult result=m_pTerrainGroup->rayIntersects(ray);
	if(!result.hit)
		return -1;
	//m_pSceneMgr->getSceneNode("test")->setPosition(result.position);
	bool nosign=false;
	Ogre::Terrain* terrain = m_pTerrainGroup->getTerrain(0,0);


	int size= terrain->getLayerBlendMapSize();

	int x=result.position.x*m_EdgeImage.width()/(float)terrain->getWorldSize();
	int y=result.position.z*m_EdgeImage.height()/(float)terrain->getWorldSize();

	QRgb resultrgb=m_AreaImage.pixel(x,y);
	int gray=qGray(m_EdgeImage.pixel(x,y));
	if(gray!=255)
	{
		return -1;
	}
	else
	{
		int id=qGreen(resultrgb)*256+qBlue(resultrgb);
		if(id<65535)
			return id;
		else
			return -1;
	}
}

void MyTerrain::SetAreaHighLight( int id,bool enable/*=true*/ )
{
	if(id<0||id==65535)
		return;
	m_AreaVector[id]->m_bHighLight=enable;
	UpdateTexture();
}

void MyTerrain::UpdateTexture()
{
	Ogre::Terrain* terrain = m_pTerrainGroup->getTerrain(0,0);
	int size= terrain->getLayerBlendMapSize();
	Ogre::TerrainLayerBlendMap* blendMap3 = terrain->getLayerBlendMap(4);
	float* pBlend3 = blendMap3->getBlendPointer();

	for (Ogre::uint16 y = 0; y < terrain->getLayerBlendMapSize(); ++y)
	{
		for (Ogre::uint16 x = 0; x < terrain->getLayerBlendMapSize(); ++x)
		{
			float tx, ty;

			blendMap3->convertImageToTerrainSpace(x, y, &tx, &ty);
			QRgb rgb=m_AreaImage.pixel(x*m_AreaImage.width()/size,y*m_AreaImage.height()/size);
			int gray=qGray(m_EdgeImage.pixel(x*m_AreaImage.width()/size,y*m_AreaImage.height()/size));
			int r=qRed(rgb);
			int g=qGreen(rgb);
			int b=qBlue(rgb);
			int id=g*256+b;
			if(gray==255&&rgb!=qRgb(255,255,255))
			{
				if(m_AreaVector[id]->m_bHighLight)
				{
					*pBlend3=0.2;
				}
				else
				{
					*pBlend3=0;
				}
			}
			else
			{
				*pBlend3=0;
			}
			pBlend3++;


		}
	}
	blendMap3->dirty();
	blendMap3->update();
}

Ogre::Vector2 MyTerrain::GetWorldPointFromImagePoint( Ogre::Vector2 point )
{
	Ogre::Vector2 v;
	Ogre::Terrain* terrain = m_pTerrainGroup->getTerrain(0,0);
	v.x=point.x*m_EdgeImage.width()/(float)terrain->getWorldSize();
	v.y=point.y*m_EdgeImage.height()/(float)terrain->getWorldSize();
	return v;
}

float MyTerrain::GetWorldXFromImageX( float x )
{
	Ogre::Terrain* terrain = m_pTerrainGroup->getTerrain(0,0);
	return x*terrain->getWorldSize()/(float)m_EdgeImage.width();
}

float MyTerrain::GetWorldYFromImageY( float y )
{
	Ogre::Terrain* terrain = m_pTerrainGroup->getTerrain(0,0);
	return y*terrain->getWorldSize()/(float)m_EdgeImage.height();
}

float MyTerrain::GetImageXFromWorldX( float x )
{
	Ogre::Terrain* terrain = m_pTerrainGroup->getTerrain(0,0);
	return x*m_EdgeImage.width()/(float)terrain->getWorldSize();
}

float MyTerrain::GetImageYFromWorldY( float y )
{
	Ogre::Terrain* terrain = m_pTerrainGroup->getTerrain(0,0);
	return y*m_EdgeImage.height()/(float)terrain->getWorldSize();
}

void MyTerrain::InitBlendMap( Ogre::Terrain *terrain)
{
	QImage img("edge.png");
	Ogre::TerrainLayerBlendMap* blendMap0 = terrain->getLayerBlendMap(1);
	Ogre::TerrainLayerBlendMap* blendMap1 = terrain->getLayerBlendMap(2);
	Ogre::TerrainLayerBlendMap* blendMap2 = terrain->getLayerBlendMap(3);
	Ogre::TerrainLayerBlendMap* blendMap3 = terrain->getLayerBlendMap(4);
	float* pBlend0 = blendMap0->getBlendPointer();
	float* pBlend1 = blendMap1->getBlendPointer();
	float* pBlend2 = blendMap2->getBlendPointer();
	float* pBlend3 = blendMap3->getBlendPointer();
	int size= terrain->getLayerBlendMapSize();
	for (Ogre::uint16 y = 0; y < terrain->getLayerBlendMapSize(); ++y)
	{
		for (Ogre::uint16 x = 0; x < terrain->getLayerBlendMapSize(); ++x)
		{
			float tx, ty;

			blendMap0->convertImageToTerrainSpace(x, y, &tx, &ty);
			float height = terrain->getHeightAtTerrainPosition(tx, ty);
			if(height<200)
			{
				*pBlend1=1;
				*pBlend2=0;
			}
			else if (height<400)
			{
				*pBlend1=1-(height-200)/200.0;
				*pBlend2=(height-200)/200.0;;
			}
			else
			{
				*pBlend1=0;
				*pBlend2=1;
			}
			if(m_pTerrainGroup->getTerrain(0,0)==terrain)
			{
				int gray=qGray(img.pixel(x*img.width()/size,y*img.height()/size));
				float g=gray/255.0;
				*pBlend0=1-g;
				*pBlend1=g*(*pBlend1);
				*pBlend2=g*(*pBlend2); 
			}

			*pBlend3=0;
			pBlend0++;
			pBlend1++;
			pBlend2++;
			pBlend3++;

		}
	}
	blendMap0->dirty();
	blendMap1->dirty();
	blendMap2->dirty();

	blendMap0->update();
	blendMap1->update();
	blendMap2->update();
	
}

MyTerrain& MyTerrain::GetSingleton()
{
	return *m_pSingleton;
}

int MyTerrain::GetAreaIDFromImageF( float x,float y )
{
	QRgb rgb=m_AreaImage.pixel(x*m_AreaImage.width(),y*m_AreaImage.height());
	int blue=qBlue(rgb);
	int green=qGreen(rgb);
	return green*256+blue;
}

MyArea * MyTerrain::GetArea( int id )
{
	return m_AreaVector[id];
}

void MyTerrain::InitArea()
{
	using namespace std;
	fstream input;
	input.open("midpoint.txt",ios::in);
	int n;
	input>>n;

	m_nAreaCount=n;
	m_AreaVector.resize(m_nAreaCount);
	for (int i=0;i<n;i++)
	{
		int id;
		input>>id;



		m_AreaVector[id]=new MyArea;
		m_AreaVector[id]->m_id=id;
		input>>m_AreaVector[id]->m_MidPoint.x>>m_AreaVector[id]->m_MidPoint.y;

		m_AreaVector[id]->m_FlagEntity=m_pSceneMgr->createEntity("qizi.mesh");
		m_AreaVector[id]->m_ArmyEntity=m_pSceneMgr->createEntity("sinbad.mesh");
		m_AreaVector[id]->m_Sword1 = m_pSceneMgr->createEntity("Sword.mesh");
		m_AreaVector[id]->m_Sword2 = m_pSceneMgr->createEntity("Sword.mesh");
		m_AreaVector[id]->m_ArmyEntity->attachObjectToBone("Sheath.L", m_AreaVector[id]->m_Sword1);
		m_AreaVector[id]->m_ArmyEntity->attachObjectToBone("Sheath.R", m_AreaVector[id]->m_Sword2);


		m_AreaVector[id]->m_FlagEntity->setCastShadows(true);
		m_AreaVector[id]->m_ArmyEntity->setCastShadows(true);

		m_AreaVector[id]->m_Node=m_pSceneMgr->getRootSceneNode()->createChildSceneNode(("Area"+QString::number(id)+"MidPoint").toStdString());
		int x=GetWorldXFromImageX(m_AreaVector[id]->m_MidPoint.x);
		int y=GetWorldYFromImageY(m_AreaVector[id]->m_MidPoint.y);
		float height=m_pTerrainGroup->getHeightAtWorldPosition(Ogre::Vector3(x,0,y));
		m_AreaVector[id]->m_Node->setPosition(x,height,y);

		m_AreaVector[id]->m_FlagNode=m_AreaVector[id]->m_Node->createChildSceneNode(("Area"+QString::number(id)+"Flag").toStdString(),Ogre::Vector3(0,60,0));
		m_AreaVector[id]->m_FlagNode->attachObject(m_AreaVector[id]->m_FlagEntity);
		m_AreaVector[id]->m_FlagNode->setScale(Ogre::Vector3(30));


		m_AreaVector[id]->m_ArmyNode=m_pSceneMgr->getRootSceneNode()->createChildSceneNode(("Area"+QString::number(id)+"Army").toStdString(),m_AreaVector[id]->m_Node->getPosition()+Ogre::Vector3(0,Global::HeightOffsetForSinbadOnTerrain,0));
		m_AreaVector[id]->m_ArmyNode->attachObject(m_AreaVector[id]->m_ArmyEntity);
		m_AreaVector[id]->m_ArmyNode->setScale(Ogre::Vector3(5));
		m_AreaVector[id]->m_FlagEntity->setMaterialName("qiziMaterial00");
	}
}

void MyTerrain::ReadAreaRelationship()
{
	using namespace std;
	fstream input;
	input.open("relationship.txt",ios::in);
	int n;
	input>>n;
	for (int i=0;i<n;i++)
	{
		int m;
		input>>m;
		for (int j=0;j<m;j++)
		{
			int t;
			input>>t;
			m_AreaVector[i]->m_AdjacencySet.insert(m_AreaVector[t]);
		}
	}
	input.close();
}

void MyTerrain::InitTerrain()
{
	
	using namespace std;




	m_EdgeImage=QImage("edge.png");
	m_AreaImage=QImage("map.png");
// 	int maxcolor=0;
// 	for (int x=0;x<m_AreaImage.width();x++)
// 	{
// 		for (int y=0;y<m_AreaImage.height();y++)
// 		{
// 			QRgb resultrgb=m_AreaImage.pixel(x,y);
// 			int gray=qGray(m_EdgeImage.pixel(x,y));
// 			if(gray==255)
// 			{
// 				int blue=qBlue(resultrgb);
// 				if(blue>maxcolor)
// 					maxcolor=blue;
// 			}
// 		}
// 	}
// 	m_nAreaCount=maxcolor+1;
// 	m_AreaVector.resize(m_nAreaCount);





	m_pTerrainGlobals = new Ogre::TerrainGlobalOptions();   
	m_pTerrainGroup = OGRE_NEW Ogre::TerrainGroup(m_pSceneMgr, Ogre::Terrain::ALIGN_X_Z, TERRAIN_SIZE, TERRAIN_WORLD_SIZE);
	m_pTerrainGroup->setFilenameConvention(TERRAIN_FILE_PREFIX, TERRAIN_FILE_SUFFIX);
	m_pTerrainGroup->setOrigin(Ogre::Vector3(TERRAIN_WORLD_SIZE/2,0,TERRAIN_WORLD_SIZE/2));
	m_pTerrainGlobals->setLightMapSize(16);

	Ogre::Vector3 lightdir(0.55, -0.3, 0.75);
	lightdir.normalise();
	l = m_pSceneMgr->createLight("tstLight");
	l->setType(Ogre::Light::LT_DIRECTIONAL);
	l->setDirection(lightdir);
	l->setDiffuseColour(Ogre::ColourValue::White);
	l->setCastShadows(true);
	//l->setSpecularColour(ColourValue(0.4, 0.4, 0.4));

	m_pSceneMgr->setAmbientLight(Ogre::ColourValue(0.4, 0.4, 0.4));

	m_pTerrainGlobals->setMaxPixelError(8);
	m_pTerrainGlobals->setCompositeMapDistance(999999);//距离镜头超过3000部分使用地图合成（CompositeMap）模式表现
	m_pTerrainGlobals->setLightMapDirection(l->getDerivedDirection());//地图光照方向（和实时阴影生成相关）
	m_pTerrainGlobals->setCompositeMapAmbient(m_pSceneMgr->getAmbientLight());
	m_pTerrainGlobals->setCompositeMapDiffuse(l->getDiffuseColour());

	Ogre::Terrain::ImportData& defaultimp = m_pTerrainGroup->getDefaultImportSettings();
	defaultimp.terrainSize = TERRAIN_SIZE;//不太了解，调试中，这个值越小，地图边缘锯齿现象越严重，太小的话，运行起来程序会跑死、出错
	defaultimp.worldSize = TERRAIN_WORLD_SIZE;//假设为a，那么地图大小为 a x a
	defaultimp.inputScale = 600;//决定地图最大落差（高度），即位图中白色和黑色部分的高度差
	defaultimp.minBatchSize = 33;
	defaultimp.maxBatchSize = 65;

	defaultimp.layerList.resize(5);
	defaultimp.layerList[0].worldSize = 100; 
	defaultimp.layerList[0].textureNames.push_back("dirt_grayrocky_diffusespecular.dds");
	defaultimp.layerList[0].textureNames.push_back("dirt_grayrocky_normalheight.dds");
	defaultimp.layerList[1].worldSize = 2570;
	defaultimp.layerList[1].textureNames.push_back("edgecolor.png");
	defaultimp.layerList[1].textureNames.push_back("edgecolor.png");
	defaultimp.layerList[2].worldSize = 200;
	defaultimp.layerList[2].textureNames.push_back("grass_green-01_diffusespecular.dds");
	defaultimp.layerList[2].textureNames.push_back("grass_green-01_normalheight.dds");
	defaultimp.layerList[3].worldSize = 200;
	defaultimp.layerList[3].textureNames.push_back("growth_weirdfungus-03_diffusespecular.dds");
	defaultimp.layerList[3].textureNames.push_back("growth_weirdfungus-03_normalheight.dds");
	defaultimp.layerList[4].worldSize = 2570;
	defaultimp.layerList[4].textureNames.push_back("choosecolor.png");
	defaultimp.layerList[4].textureNames.push_back("choosecolor.png");

	Ogre::String filename = m_pTerrainGroup->generateFilename(0, 0);
	if (Ogre::ResourceGroupManager::getSingleton().resourceExists(m_pTerrainGroup->getResourceGroup(), filename))
	{
		m_pTerrainGroup->defineTerrain(0, 0);
	}
	else
	{

		// 		for (int i=-1;i<=1;i++)
		// 		{
		// 			for (int j=-1;j<=1;j++)
		// 			{
		// 				Ogre::Image img;
		// 				img.load("t.bmp", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
		// 				if(i!=0)
		// 					img.flipAroundY();
		// 				if (j!=0)
		// 					img.flipAroundX();
		// 				m_pTerrainGroup->defineTerrain(i, j, &img);
		// 			}
		// 		}
		// 		
		Ogre::Image img;
		img.load("t.png", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
		m_pTerrainGroup->defineTerrain(0, 0, &img);
	}
	m_pTerrainGroup->loadAllTerrains(true);
	Ogre::TerrainGroup::TerrainIterator it=m_pTerrainGroup->getTerrainIterator();
	while(it.hasMoreElements())
	{
		Ogre::Terrain *terrain = it.getNext()->instance;
		InitBlendMap(terrain);
	}

	m_pTerrainGroup->update();
}

void MyTerrain::InitNxOgre()
{
	m_pWorld=NxOgre::World::createWorld();
	NxOgre::ResourceSystem::getSingleton()->openProtocol(new Critter::OgreResourceProtocol());

	m_pWorld->getRemoteDebugger()->connect();

	NxOgre::SceneDescription scene_description;
	scene_description.mGravity = NxOgre::Constants::MEAN_EARTH_GRAVITY;
	scene_description.mUseHardware = false;

	m_pScene = m_pWorld->createScene(scene_description);

	m_pScene->getMaterial(0)->setAll(0.1f, 0.9f, 0.5f);
	m_pScene->getMaterial(0)->setRestitution(0.1f);
	m_pScene->getMaterial(0)->setDynamicFriction(0.9);
	m_pScene->getMaterial(0)->setStaticFriction(0.5);
	m_pScene->setGravity(NxOgre::Vec3(0,Global::Gravity,0));

	m_pScene->createSceneGeometry(NxOgre::PlaneGeometryDescription());
	Ogre::TerrainGroup::TerrainIterator ti = m_pTerrainGroup->getTerrainIterator();

	while(ti.hasMoreElements())
	{
		Ogre::Terrain* t = ti.getNext()->instance;
		InitNxOgreTerrain(t);
	}



	m_pRenderSystem=new Critter::RenderSystem(m_pScene,m_pSceneMgr);
	


	//m_pRenderSystem->createBody(NxOgre::BoxDescription(),v3+NxOgre::Vec3(0,35,0), "shaizi.mesh", bodyDescription);

	
}

NxOgre::SceneGeometry* MyTerrain::InitNxOgreTerrain(Ogre::Terrain* terrain)
{

	NxOgre::HeightFieldData data;
	data.mNbColumns = terrain->getSize();
	data.mNbRows = terrain->getSize();

	const float normMin =-32768.0f;
	const float normMax =32767.0f;
	const float minHeight = terrain->getMinHeight();
	const float maxHeight = terrain->getMaxHeight();
	const float heightDiff = maxHeight - minHeight;
	const float normDiff = normMax - normMin;
	const unsigned short size = terrain->getSize();

	float* ter_data = terrain->getHeightData();

	NxOgre::HeightFieldSample sample;

	sample.mMaterial0 =0;
	sample.mMaterial1 =0;
	sample.mTessellationFlag =NxOgre::Enums::HeightFieldTesselation_NW_SE;

	NxOgre_DebugPrint_HeightFields("Attempting to sample data from heightfield, size = "<< size);

	data.mSamples.resize(size * size);

	for(int x =0; x < size;++x)
	{
		sample.mTessellationFlag =NxOgre::Enums::HeightFieldTesselation_NW_SE;

		for(int z = size -1; z >=0;--z)
		{
			float height = ter_data[(size * z)+ x];
			sample.mHeight =(short)(((height - minHeight)/ heightDiff)* normDiff + normMin);
			data.mSamples.push_back(sample);
			sample.mTessellationFlag =!sample.mTessellationFlag;
		}
	}

	NxOgre_DebugPrint_HeightFields("Attempting to cook heightfield from Ogre terrain");
	NxOgre::HeightField* hf = data.cookQuickly();

	const float hf_size =float(terrain->getWorldSize())+(float(terrain->getWorldSize())/float(size));
	const float hf_height =(maxHeight - minHeight)/2.0f;
	NxOgre::Vec3 pose(terrain->getPosition());
	//pose.x -= float(size) / 2.0f;
	pose.x -=float(terrain->getWorldSize())/2.0f;
	pose.y +=(maxHeight + minHeight)/2.0f;
	//pose.z -= float(size) / 2.0f;
	pose.z -=float(terrain->getWorldSize())/2.0f;

	NxOgre::HeightFieldGeometryDescription hf_desc;
	hf_desc.mHeightField = hf;
	//hf_desc.mLocalPose.set(pose);
	hf_desc.mDimensions.set(hf_size, hf_height, hf_size);


	NxOgre_DebugPrint_HeightFields("Attempting to create scene geometry from Ogre terrain");

	return m_pScene->createSceneGeometry(hf_desc, pose);
}

float MyTerrain::Update( const Ogre::FrameEvent& evt )
{
	m_pWorld->advance(evt.timeSinceLastFrame*Global::DiceSpeed);
	NxOgre::TimeStep timestep=m_pScene->getTimeStep();
	return timestep.getModified();
}

Critter::RenderSystem * MyTerrain::GetRenderSystem()
{
	return m_pRenderSystem;
}

Ogre::SceneManager	* MyTerrain::GetSceneManager()
{
	return m_pSceneMgr;
}

Ogre::TerrainGroup	* MyTerrain::GetTerrainGroup()
{
	return m_pTerrainGroup;
}

void MyTerrain::InitArmy()
{
	lua_State* L = lua_open();
	luaopen_base(L);
	MyTerrain::DefineInLua(L);
	MyArea::DefineInLua(L);

	luaL_openlibs(L);
	lua_tinker::set(L, "terrain", this);
	lua_tinker::dofile(L, "./media/lua/TerrainInit.lua");
	lua_close(L);

// 	srand(clock());
// 	for (int i=0;i<m_nAreaCount;i++)
// 	{
// 		if (m_AreaVector[i]->GetAreaBelong()==0)
// 		{
// 			m_AreaVector[i]->SetArmyCount(rand()%8);
// 		}
// 	}
}

int MyTerrain::GetAreaCount()
{
	return m_nAreaCount;
}

void MyTerrain::DefineInLua( lua_State *L )
{
	lua_tinker::class_add<MyTerrain>(L, "MyTerrain");
	lua_tinker::class_def<MyTerrain>(L,"GetAreaCount",&MyTerrain::GetAreaCount);
	lua_tinker::class_def<MyTerrain>(L,"GetArea",&MyTerrain::GetArea);
	lua_tinker::class_def<MyTerrain>(L,"SetAreaHighLight",&MyTerrain::SetAreaHighLight);
}


MyArea::MyArea():m_bHighLight(0),m_iAreaBelong(0),m_nArmyCount(0)
{
	
}

void MyArea::SetAreaBelong( int id )
{
	if (id!=m_iAreaBelong)
	{
		MyPlayer *player=MyGameStateManager::GetSingleton().GetPlayer(m_iAreaBelong);
		player->m_AreaSet.erase(this);
		player->m_nArmyCount-=m_iAreaBelong;
		player->SetAreaCount(player->GetAreaCount()-1);

		player=MyGameStateManager::GetSingleton().GetPlayer(id);
		player->m_AreaSet.insert(this);
		player->m_nArmyCount+=m_iAreaBelong;
		player->SetAreaCount(player->GetAreaCount()+1);

	}
	m_iAreaBelong=id;
	QString str=QString::number(m_iAreaBelong)+QString::number(m_nArmyCount);
	m_FlagEntity->setMaterialName(("qiziMaterial"+str).toStdString());
}

int MyArea::GetAreaBelong()
{
	return m_iAreaBelong;
}

void MyArea::SetArmyCount( int n )
{
	MyGameStateManager::GetSingleton().GetPlayer(m_iAreaBelong)->m_nArmyCount+=n-m_iAreaBelong;
	m_nArmyCount=n;
	QString str=QString::number(m_iAreaBelong)+QString::number(m_nArmyCount);
	m_FlagEntity->setMaterialName(("qiziMaterial"+str).toStdString());
}

int MyArea::GetArmyCount()
{
	return m_nArmyCount;
}

void MyArea::DefineInLua( lua_State *L )
{
	lua_tinker::class_add<MyArea>(L,"MyArea");
	lua_tinker::class_def<MyArea>(L,"GetAreaBelong",&MyArea::GetAreaBelong);
	lua_tinker::class_def<MyArea>(L,"GetArmyCount",&MyArea::GetArmyCount);
	lua_tinker::class_def<MyArea>(L,"SetAreaBelong",&MyArea::SetAreaBelong);
	lua_tinker::class_def<MyArea>(L,"SetArmyCount",&MyArea::SetArmyCount);
	lua_tinker::class_def<MyArea>(L,"AddBuff",&MyArea::AddBuff);
	lua_tinker::class_def<MyArea>(L,"RemoveBuff",&MyArea::RemoveBuff);
}

void MyArea::AddBuff( MyBuff *buff )
{
	m_BuffVector.push_back(buff);
}

void MyArea::RemoveBuff( MyBuff *buff )
{
	std::vector<MyBuff*>::iterator it=find(m_BuffVector.begin(),m_BuffVector.end(),buff);
	if(it!=m_BuffVector.end())
		m_BuffVector.erase(it);
}

bool MyArea::IsAdjacencyArea( MyArea *area )
{
	if (m_AdjacencySet.find(area)==m_AdjacencySet.end())
		return false;
	else
		return true;
}

Ogre::SceneNode * MyArea::GetSceneNode()
{
	return m_Node;
}

Ogre::SceneNode * MyArea::GetArmySceneNode()
{
	return m_ArmyNode;
}

Ogre::Entity	* MyArea::GetArmyEntity()
{
	return m_ArmyEntity;
}

Ogre::Entity	* MyArea::GetSword( int id )
{
	if(id==1)
		return m_Sword1;
	else if(id==2)
		return m_Sword2;
	else
		return 0;
}

MyBuff * MyArea::GetBuffByID( int id )
{
	return m_BuffVector[id];
}

int MyArea::GetBuffCount()
{
	return m_BuffVector.size();
}

