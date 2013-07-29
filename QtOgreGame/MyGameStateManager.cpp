﻿#include "StdAfx.h"
#include "MyGameStateManager.h"
#include "MyGameApp.h"
#include "MyTerrain.h"
#include "CollisionTools.h"
#include "Global.h"
#include "MyUIUpdater.h"
#include "MyTransformer.h"
MyGameStateManager* MyGameStateManager::m_pSingleton=0;
MyGameStateManager::MyGameStateManager(void)
	:m_iChoose1(-1),m_iChoose2(-1),m_pCurrentPlayer(0),m_pCurrentState(0),m_pNextState(0)
{
	if (m_pSingleton)
	{
		throw "singleton already exist";
	}
	m_pSingleton=this;

	m_nPlayerCount=QString(MyPageManager::GetSingleton().GetPage("GameSettingPage")->GetWindow()->getChild("Background/PlayerCount")->getText().c_str()).toInt();
	m_PlayerVector.resize(m_nPlayerCount+1);
	for (int i=0;i<m_nPlayerCount+1;i++)
	{
		m_PlayerVector[i]=new MyPlayer(i);
	}
	m_PlayerVector[0]->SetAreaCount(MyTerrain::GetSingleton().GetAreaCount());

	int nCutArea;
	int nCut;
	if (m_nPlayerCount<=4)
	{
		nCutArea=9;
		nCut=3;
	}
	else if (m_nPlayerCount<=9)
	{
		nCutArea=25;
		nCut=5;
	}
	int playerid=1;
	for (int i=0;i<nCut;i++)
	{
		for (int j=0;j<nCut;j++)
		{
			if(i%2==0&&j%2==0)
			{
				float fx=rand()%100/100.0/nCut;
				float fy=rand()%100/100.0/nCut;
				fx+=1.0*i/nCut;
				fy+=1.0*j/nCut;
				int id=MyTerrain::GetSingleton().GetAreaIDFromImageF(fx,fy);
				MyArea *area=MyTerrain::GetSingleton().GetArea(id);
				area->SetAreaBelong(playerid);
				area->SetArmyCount(5);


				playerid++;
			}
			if (playerid>m_nPlayerCount)
			{
				break;
			}
		}
		if (playerid>m_nPlayerCount)
		{
			break;
		}
	}


	MyGameState *GameStartState=new MyGameStartState("GameStartState");
	AddRootState(GameStartState);
	MyGameState *GamePlayerChangeState=new MyGamePlayerChangeState("GamePlayerChangeState");
	AddRootState(GamePlayerChangeState);
	MyGameState *GamePlayState=new MyGamePlayState("GamePlayState");
	AddRootState(GamePlayState);
// 	MyGameState *GameDiceState=new MyGameDiceState("GameDiceState");
// 	AddRootState(GameDiceState);
	MyGameState *GameAttackState=new MyGameAttackState("GameAttackState");
	AddRootState(GameAttackState);

	GameStartState->SetNextState(GamePlayerChangeState);
	GamePlayerChangeState->SetNextState(GamePlayState);
	GameAttackState->SetNextState(GamePlayState);
	new MyTransformerManager;
}


MyGameStateManager::~MyGameStateManager(void)
{
	m_pSingleton=0;
	
}

MyGameStateManager& MyGameStateManager::GetSingleton()
{
	return *m_pSingleton;
}

void MyGameStateManager::InitMap()
{

}

MyPlayer * MyGameStateManager::GetPlayer( int id )
{
	return m_PlayerVector[id];
}

MyGameState * MyGameStateManager::GetCurrentState()
{
	return m_pCurrentState;
}

void MyGameStateManager::AddRootState( MyGameState *state )
{
	m_RootStateMap[state->GetName()]=state;
}


MyPlayer *MyGameStateManager::GetCurrentPlayer()
{
	return m_pCurrentPlayer;
}

void MyGameStateManager::SetCurrentPlayerID( int id )
{
	m_pCurrentPlayer=GetPlayer(id);
	SetNextState("GamePlayerChangeState");
	MyUIUpdater::GetSingleton().on_CardChange();
	m_pCurrentPlayer->SetMoveTimes(m_pCurrentPlayer->GetMaxMoveTimes());
}

void MyGameStateManager::ChooseArea( int id )
{
	if (m_iChoose1<0)
	{
		m_iChoose1=id;
		MyGameApp::GetSingleton().GetMyTerrain()->SetAreaHighLight(m_iChoose1,true);
	}
	else if(m_iChoose1==id)
	{
		m_iChoose2=id;
	}
	else
	{
		MyArea *area=MyTerrain::GetSingleton().GetArea(m_iChoose1);
		if (m_iChoose2<0&&area->GetAreaBelong()==GetCurrentPlayer()->GetID())
		{
			if (area->IsAdjacencyArea(MyTerrain::GetSingleton().GetArea(id)))
			{
				m_iChoose2=id;
			}
			else
			{
				MyGameApp::GetSingleton().GetMyTerrain()->SetAreaHighLight(m_iChoose1,false);
				m_iChoose1=id;
				MyGameApp::GetSingleton().GetMyTerrain()->SetAreaHighLight(m_iChoose1,true);
			}
		}
		else
		{
			MyGameApp::GetSingleton().GetMyTerrain()->SetAreaHighLight(m_iChoose1,false);
			MyGameApp::GetSingleton().GetMyTerrain()->SetAreaHighLight(m_iChoose2,false);
			m_iChoose1=id;
			MyGameApp::GetSingleton().GetMyTerrain()->SetAreaHighLight(m_iChoose1,true);
			m_iChoose2=-1;
		}
	}
	
	MyUIUpdater::GetSingleton().on_AreaChoose();
}

void MyGameStateManager::ClearChoose()
{
	if (m_iChoose1>=0)
	{
		MyGameApp::GetSingleton().GetMyTerrain()->SetAreaHighLight(m_iChoose1,false);
		m_iChoose1=-1;
	}
	if (m_iChoose2>=0)
	{
		MyGameApp::GetSingleton().GetMyTerrain()->SetAreaHighLight(m_iChoose2,false);
		m_iChoose2=-1;
	}
	CEGUI::WindowManager &winMgr=CEGUI::WindowManager::getSingleton();
	CEGUI::Window *win=MyPageManager::GetSingleton().GetPage("GamePlayingPage")->GetWindow()->getChild("AreaInfo1");
	win->setVisible(false);
	win=MyPageManager::GetSingleton().GetPage("GamePlayingPage")->GetWindow()->getChild("AreaInfo2");
	win->setVisible(false);
	win=MyPageManager::GetSingleton().GetPage("GamePlayingPage")->GetWindow()->getChild("Action");
	win->setVisible(false);
}

int MyGameStateManager::GetChooseID( int id )
{
	if (id==1)
	{
		return m_iChoose1;
	}
	else if (id==2)
	{
		return m_iChoose2;
	}
	throw "error id";
	return -1;
}

MyGameState * MyGameStateManager::GetRootState( std::string name )
{
	if (m_RootStateMap[name]==0)
	{
		m_RootStateMap.erase(name);
		return 0;
	}
	else
	{
		return m_RootStateMap[name];
	}
}


void MyGameStateManager::TurnNextPlayer()
{
	int id=m_pCurrentPlayer->GetID()+1;
	if (id>m_nPlayerCount)
	{
		id=1;
	}
	SetCurrentPlayerID(id);
	
}

void MyGameStateManager::DefineInLua( lua_State *L )
{
	lua_tinker::class_add<MyGameStateManager>(L,"MyGameStateManager");
	lua_tinker::class_def<MyGameStateManager>(L,"ChooseArea",&MyGameStateManager::ChooseArea);
	lua_tinker::class_def<MyGameStateManager>(L,"ClearChoose",&MyGameStateManager::ClearChoose);
	lua_tinker::class_def<MyGameStateManager>(L,"GetChooseID",&MyGameStateManager::GetChooseID);
	lua_tinker::class_def<MyGameStateManager>(L,"GetChooseArea",&MyGameStateManager::GetChooseArea);
	lua_tinker::class_def<MyGameStateManager>(L,"GetCurrentPlayer",&MyGameStateManager::GetCurrentPlayer);
	lua_tinker::class_def<MyGameStateManager>(L,"GetPlayer",&MyGameStateManager::GetPlayer);
}

bool MyGameStateManager::frameStarted( const Ogre::FrameEvent& evt )
{
	MyTransformerManager::GetSingleton().Update(evt.timeSinceLastFrame);
	if(m_pNextState)
	{
		while(m_pCurrentState)
		{
			m_pCurrentState->on_State_Exit();
			m_pCurrentState=m_pCurrentState->GetLastState();
			if(m_pCurrentState)
				Ogre::LogManager::getSingleton().logMessage("ReturnState:"+m_pCurrentState->GetName());
		}
		m_pCurrentState=m_pNextState;
		Ogre::LogManager::getSingleton().logMessage("TurnToState:"+m_pCurrentState->GetName());
		m_pNextState=0;
		m_pCurrentState->on_State_Entry();
	}
	if(!m_pCurrentState)
		return true;
	bool re=m_pCurrentState->frameStarted(evt);
	if (m_pCurrentState->GetGoInState())
	{
		MyGameState *laststate=m_pCurrentState;
		m_pCurrentState=m_pCurrentState->GetGoInState();
		Ogre::LogManager::getSingleton().logMessage("EnterState:"+m_pCurrentState->GetName());
		m_pCurrentState->SetLastState(laststate);
		m_pCurrentState->on_State_Entry();
	}
	else if(m_pCurrentState->IsGoNext())
	{
		m_pCurrentState->on_State_Exit();
		m_pCurrentState=m_pCurrentState->GetNextState();
		Ogre::LogManager::getSingleton().logMessage("TurnToState:"+m_pCurrentState->GetName());
		m_pCurrentState->on_State_Entry();
	}
	else if(m_pCurrentState->IsReturn())
	{
		m_pCurrentState->on_State_Exit();
		MyGameState *laststate=m_pCurrentState;;
		m_pCurrentState=m_pCurrentState->GetLastState();
		Ogre::LogManager::getSingleton().logMessage("ReturnState:"+m_pCurrentState->GetName());
		delete laststate;
	}
	return re;
}

void MyGameStateManager::SetNextState( std::string name )
{
	m_pNextState=m_RootStateMap[name];
	
}

bool MyGameStateManager::keyPressed( const OIS::KeyEvent &arg )
{
	if (m_pCurrentState)
	{
		m_pCurrentState->keyPressed(arg);
	}
	return true;
}

bool MyGameStateManager::keyReleased( const OIS::KeyEvent &arg )
{
	if (m_pCurrentState)
	{
		m_pCurrentState->keyReleased(arg);
	}
	return true;
}

bool MyGameStateManager::mouseMoved( const OIS::MouseEvent &arg )
{
	if (m_pCurrentState)
	{
		m_pCurrentState->mouseMoved(arg);
	}
	return true;
}

bool MyGameStateManager::mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	if (m_pCurrentState)
	{
		m_pCurrentState->mousePressed(arg,id);
	}
	return true;
}

bool MyGameStateManager::mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	if (m_pCurrentState)
	{
		m_pCurrentState->mouseReleased(arg,id);
	}
	return true;
}

QDataStream	& MyGameStateManager::GetDataStream()
{
	return m_DataStream;
}

MyArea * MyGameStateManager::GetChooseArea( int id )
{
	int areaid=GetChooseID(id);
	if(areaid<0)
		return 0;
	return MyTerrain::GetSingleton().GetArea(areaid);
}

MyPlayer::MyPlayer( int id ):m_ID(id),m_nAreaCount(0),m_nArmyCount(0),m_nGold(0),m_MoveTimes(0),m_MaxMoveTimes(0)
{
	
}

void MyPlayer::SetAreaCount(int n)
{
	int movetimes[14]={0,1,3,6,10,15,21,28,36,45,55,66,78,91};
	m_nAreaCount=n;
	for (int i=0;i<14;i++)
	{
		if (n>=movetimes[i])
		{
			m_MaxMoveTimes=i;
		}
		else
		{
			break;
		}
	}
}

int MyPlayer::GetAreaCount()
{
	return m_nAreaCount;
}

void MyPlayer::SetMoveTimes( int n )
{
	m_MoveTimes=n;
	MyUIUpdater::GetSingleton().on_MoveTimesChange();
}

int MyPlayer::GetMoveTimes()
{
	return m_MoveTimes;
}

int MyPlayer::GetID()
{
	return m_ID;
}

CEGUI::String MyPlayer::GetName()
{
	return m_Name;
}

void MyPlayer::SetID( int id )
{
	m_ID=id;
}

void MyPlayer::SetName(char *name)
{
	m_Name=name;
}

int MyPlayer::GetMaxMoveTimes()
{
	return m_MaxMoveTimes;
}

void MyPlayer::SetMaxMoveTimes( int n )
{
	m_MaxMoveTimes=n;
}

std::set<MyArea*> MyPlayer::GetAreaSet()
{
	return m_AreaSet;
}

void MyPlayer::AddCard( MyCard *card,bool update/*=true*/ )
{
	m_CardVector.push_back(card);
	if(update)
	{
		if(this==MyGameStateManager::GetSingleton().GetCurrentPlayer())
		{
			MyUIUpdater::GetSingleton().on_CardChange();
		}
	}
}

void MyPlayer::DefineInLua( lua_State *L )
{
	lua_tinker::class_add<MyPlayer>(L,"MyPlayer");
	lua_tinker::class_def<MyPlayer>(L,"AddCard",&MyPlayer::AddCard);
	lua_tinker::class_def<MyPlayer>(L,"GetAreaCount",&MyPlayer::GetAreaCount);
	lua_tinker::class_def<MyPlayer>(L,"GetID",&MyPlayer::GetID);
	lua_tinker::class_def<MyPlayer>(L,"GetMaxMoveTimes",&MyPlayer::GetMaxMoveTimes);
	lua_tinker::class_def<MyPlayer>(L,"GetMoveTimes",&MyPlayer::GetMoveTimes);
	lua_tinker::class_def<MyPlayer>(L,"GetName",&MyPlayer::GetName);
	lua_tinker::class_def<MyPlayer>(L,"SetAreaCount",&MyPlayer::SetAreaCount);
	lua_tinker::class_def<MyPlayer>(L,"SetID",&MyPlayer::SetID);
	lua_tinker::class_def<MyPlayer>(L,"SetMaxMoveTimes",&MyPlayer::SetMaxMoveTimes);
	lua_tinker::class_def<MyPlayer>(L,"SetMoveTimes",&MyPlayer::SetMoveTimes);
	lua_tinker::class_def<MyPlayer>(L,"SetName",&MyPlayer::SetName);
}

MyCard *MyPlayer::GetCardByID( int id )
{
	return m_CardVector[id];
}

int MyPlayer::GetCardCount()
{
	return m_CardVector.size();
}

void MyPlayer::RemoveCard( MyCard *card,bool update/*=true*/ )
{
	for(std::vector<MyCard*>::iterator it=m_CardVector.begin();it!=m_CardVector.end();it++)
	{
		if(*it==card)
		{
			m_CardVector.erase(it);
			break;
		}
	}
	if (update)
	{
		if(this==MyGameStateManager::GetSingleton().GetCurrentPlayer())
		{
			MyUIUpdater::GetSingleton().on_CardChange();
		}
	}
}

MyGameState::MyGameState( std::string name ):m_pNextState(0),m_pLastState(0),m_pGoInState(0),m_bGoNext(0),m_bReturn(0),m_Stage("Start")
{
	m_Name=name;
}

bool MyGameState::frameStarted( const Ogre::FrameEvent& evt )
{
	return true;
}

bool MyGameState::keyPressed( const OIS::KeyEvent &arg )
{
	return true;
}

bool MyGameState::keyReleased( const OIS::KeyEvent &arg )
{
	return true;
}

bool MyGameState::mouseMoved( const OIS::MouseEvent &arg )
{
	return true;
}

bool MyGameState::mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	return true;
}

bool MyGameState::mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	return true;
}

std::string MyGameState::GetName()
{
	return m_Name;
}

void MyGameState::on_State_Entry()
{
	if(m_pLastState)
		m_pLastState->m_pGoInState=0;
}

void MyGameState::on_State_Exit()
{
	m_bGoNext=0;
	m_bReturn=0;
	m_pGoInState=0;
}

std::string MyGameState::GetStage()
{
	return m_Stage;
}

void MyGameState::SetStage( std::string state )
{
	m_Stage=state;
}

MyGameState * MyGameState::GetNextState()
{
	return m_pNextState;
}

MyGameState * MyGameState::GetLastState()
{
	return m_pLastState;
}

MyGameState * MyGameState::GetGoInState()
{
	return m_pGoInState;
}

void MyGameState::SetNextState( MyGameState *state )
{
	m_pNextState=state;
}

void MyGameState::SetLastState( MyGameState *state )
{
	m_pLastState=state;
}

void MyGameState::SetGoInState( MyGameState *state,std::string nextStage )
{
	m_pGoInState=state;
	m_Stage=nextStage;
}

void MyGameState::ReturnLastState()
{
	m_bReturn=true;
}

bool MyGameState::IsReturn()
{
	return m_bReturn;
}

void MyGameState::GoNextState()
{
	m_bGoNext=true;
}

bool MyGameState::IsGoNext()
{
	return m_bGoNext;
}

MyGameStartState::MyGameStartState( std::string name ):MyGameState(name)
{
	

	m_pWindow=MyPageManager::GetSingleton().GetPage("GamePlayingPage")->GetWindow()->createChild("OgreTray/StaticImage","GameStart");
	m_pWindow->setArea(CEGUI::UVector2(CEGUI::UDim(0.4,0),CEGUI::UDim(0.3,0)),CEGUI::USize(CEGUI::UDim(0.3,0),CEGUI::UDim(0.3,0)));
	m_pWindow->setProperty("FrameEnabled","false");
	m_pWindow->setProperty("BackgroundEnabled","false");
	CEGUI::AnimationManager &aniMgr=CEGUI::AnimationManager::getSingleton();

	m_pGameStartAnim=aniMgr.instantiateAnimation(aniMgr.getAnimation("GameStartAnim"));
	m_pGameStartAnim->setTarget(m_pWindow);
	

	m_pGameStartMoveAnim=aniMgr.instantiateAnimation(aniMgr.getAnimation("GameStartMoveAnim"));
	m_pGameStartMoveAnim->setTarget(m_pWindow);
}

void MyGameStartState::on_State_Entry()
{
	MyGameState::on_State_Entry();
	m_pGameStartAnim->start();
	m_pGameStartMoveAnim->start();
}

bool MyGameStartState::frameStarted(const Ogre::FrameEvent& evt)
{
	if(!m_pGameStartMoveAnim->isRunning())
	{
		GoNextState();
	}
	return true;
}

void MyGameStartState::on_State_Exit()
{
	
	CEGUI::AnimationManager::getSingleton().destroyAnimationInstance(m_pGameStartAnim);
	CEGUI::AnimationManager::getSingleton().destroyAnimationInstance(m_pGameStartMoveAnim);
	CEGUI::WindowManager::getSingleton().destroyWindow(m_pWindow);
	MyGameState::on_State_Exit();
}




MyGamePlayerChangeState::MyGamePlayerChangeState( std::string name ):MyGameState(name)
{
	m_pWindow=MyPageManager::GetSingleton().GetPage("GamePlayingPage")->GetWindow()->createChild("OgreTray/StaticText","PlayerChange");
	m_pWindow->setArea(CEGUI::UVector2(CEGUI::UDim(-0.4,0),CEGUI::UDim(0.3,0)),CEGUI::USize(CEGUI::UDim(0.3,0),CEGUI::UDim(0.3,0)));
	m_pWindow->setProperty("FrameEnabled","false");
	m_pWindow->setProperty("BackgroundEnabled","false");
	m_pWindow->setFont("hwxk40");
	m_pWindow->setProperty("TextColours","tl:FFFFF263 tr:FFFFF263 bl:FFFFF263 br:FFFFF263");
	m_pWindow->setVisible(false);
	CEGUI::AnimationManager &aniMgr=CEGUI::AnimationManager::getSingleton();
	m_pMoveAnim=aniMgr.instantiateAnimation(aniMgr.getAnimation("GameStartMoveAnim"));
	m_pMoveAnim->setTarget(m_pWindow);
}

void MyGamePlayerChangeState::on_State_Entry()
{
	MyGameState::on_State_Entry();
	MyPlayer *player=MyGameStateManager::GetSingleton().GetCurrentPlayer();
	m_pWindow->setText((CEGUI::utf8*)(QStringLiteral("玩家")+QString::number(player->GetID())+QStringLiteral("行动")).toUtf8().data());
	m_pWindow->setVisible(true);
	m_pMoveAnim->start();
}

bool MyGamePlayerChangeState::frameStarted( const Ogre::FrameEvent& evt )
{
	if (!m_pMoveAnim->isRunning())
	{
		GoNextState();
	}
	return true;
}

void MyGamePlayerChangeState::on_State_Exit()
{
	
	m_pWindow->setVisible(false);
	m_pMoveAnim->stop();
	MyGameState::on_State_Exit();
}

MyGamePlayState::MyGamePlayState( std::string name ):MyGameState(name)
{

}

bool MyGamePlayState::mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	if(CEGUI::System::getSingleton().getDefaultGUIContext().getWindowContainingMouse()->getType()!="DefaultWindow")
		return true;
	Ogre::Ray ray;
	int x=arg.state.X.abs;
	int y=arg.state.Y.abs;
	MyGameApp::GetSingleton().GetMainCamera()->getCameraToViewportRay(x/(float)arg.state.width,y/(float)arg.state.height,&ray);
	int areaid=MyGameApp::GetSingleton().GetMyTerrain()->GetSelectAreaID(ray);
	if(areaid>=0)
	{
		MyGameStateManager::GetSingleton().ChooseArea(areaid);
	}
	return true;
}


MyGameDiceState::MyGameDiceState( std::string name ,int diceCount ):MyGameState(name)
{
	m_pRenderSystem=MyTerrain::GetSingleton().GetRenderSystem();
	m_pMyTerrain=&MyTerrain::GetSingleton();
	for (int i=0;i<8;i++)
	{
		dice[i].cam=m_pMyTerrain->GetSceneManager()->createCamera(("DiceCamera"+QString::number(i)).toStdString());
		dice[i].cam->setNearClipDistance(0.1);
		dice[i].cam->setFarClipDistance(10000);
// 		dice[i].cam->setPosition(Ogre::Vector3(50,1000,50));
// 		//dice[i].cam->setDirection(Ogre::Vector3(0,-1,0));
// 		dice[i].cam->lookAt(Ogre::Vector3(0,0,0));
	}
}

bool MyGameDiceState::frameStarted( const Ogre::FrameEvent& evt )
{
	using namespace std;
	time+=evt.timeSinceLastFrame;
	m_pMyTerrain->Update(evt);
	bool stop=true;
	for (int i=0;i<nDices;i++)
	{
		dice[i].cam->setPosition(dice[i].body->getNode()->getPosition()+Ogre::Vector3(10,200,10));
		dice[i].cam->lookAt(dice[i].body->getNode()->getPosition());
		if (time>5)
		{
			float v1=dice[i].body->getAngularVelocity().normalise();
			float v2=dice[i].body->getLinearVelocity().normalise();
			if(v1>1||v2>2)
			{
				stop=false;
			}
		}
		else
		{
			stop=false;
		}
	}
	
	if (stop)
	{
		Ogre::RenderWindow *window=MyGameApp::GetSingleton().GetRenderWindow();
		MOC::CollisionTools	tool(m_pMyTerrain->GetSceneManager());
		for (int i=0;i<nDices;i++)
		{
			Ogre::Vector3 result;
			int fid;
			Ogre::Entity *entity;
			float dis;
			Ogre::Vector3 start=dice[i].body->getNode()->getPosition();
			start.y+=200;
			tool.raycast(Ogre::Ray(start,Ogre::Vector3(0,-1,0)),result,fid,entity,dis);
			if(fid==0||fid==1)
			{
				dice[i].diceNum=1;
			}
			else if(fid==10||fid==11)
			{
				dice[i].diceNum=2;
			}
			else if(fid==2||fid==3)
			{
				dice[i].diceNum=3;
			}
			else if(fid==4||fid==5)
			{
				dice[i].diceNum=4;
			}
			else if(fid==8||fid==9)
			{
				dice[i].diceNum=5;
			}
			else if(fid==6||fid==7)
			{
				dice[i].diceNum=6;
			}
			window->removeViewport(i+1);
			Ogre::SceneNode::ObjectIterator it=dice[i].body->getNode()->getSceneNode()->getAttachedObjectIterator();
			while (it.hasMoreElements())
			{
				MyTerrain::GetSingleton().GetSceneManager()->destroyEntity((Ogre::Entity*)it.getNext());
			}
			m_pRenderSystem->destroyBody(dice[i].body);
		}
		ReturnLastState();
	}
	
	
	return true;
}

void MyGameDiceState::on_State_Entry()
{
	MyGameState::on_State_Entry();
	Ogre::RenderWindow *window=MyGameApp::GetSingleton().GetRenderWindow();
	for (int i=0;i<nDices;i++)
	{
		int size=m_pMyTerrain->GetTerrainGroup()->getTerrain(0,0)->getWorldSize();
		dice[i].bodyDescription.mMass=40.0f;
		dice[i].bodyDescription.mNode=m_pRenderSystem->createNode();
		srand(clock());
		dice[i].body=m_pRenderSystem->createBody(NxOgre::BoxDescription(40),NxOgre::Vec3(size/4+rand()%(size/2)+i%2*i/2*200,1000,size/4+rand()%(size/2)+(i+1)%2*i/2*100), "shaizi.mesh", dice[i].bodyDescription);
		dice[i].body->getNode()->setScale(Ogre::Vector3(60));
		window->addViewport(dice[i].cam,i+1,i%3*1/3.0,i/3/3.0,1/3.0,1/3.0);
	}
	time=0;
}


MyGameAttackState::MyGameAttackState( std::string name ):MyGameState(name)
{
	
}

void MyGameAttackState::on_State_Entry()
{
	MyGameState::on_State_Entry();
	m_pSoureceArea=MyGameStateManager::GetSingleton().GetChooseArea(1);
	m_pDestinationArea=MyGameStateManager::GetSingleton().GetChooseArea(2);

	MyGameStateManager &stateMgr=MyGameStateManager::GetSingleton();
	stateMgr.ClearChoose();
}

bool MyGameAttackState::frameStarted( const Ogre::FrameEvent& evt )
{
	if(m_Stage=="Start")
	{
		MyGameStateManager &stateMgr=MyGameStateManager::GetSingleton();
		if (m_pDestinationArea->GetArmyCount()==0)
		{
			m_pSceneNodeTransformer=MySceneNodeTransformer::Create(m_pSoureceArea->GetArmySceneNode());
			m_pGameWaitingState=new MyGameWaitingState(m_Name+".Move",m_pSceneNodeTransformer);
			Ogre::Vector3 dir=m_pDestinationArea->GetArmySceneNode()->getPosition()-m_pSoureceArea->GetArmySceneNode()->getPosition();
			dir.normalise();
			Ogre::Vector3 pos=m_pDestinationArea->GetArmySceneNode()->getPosition()-dir*Global::MoveDistance;
			m_pSceneNodeTransformer->SetAutoTurn(true);
			m_pSceneNodeTransformer->SetForwardVector(Ogre::Vector3(0,0,1));
			m_pSceneNodeTransformer->SetOnTerrain(true,Global::HeightOffsetForSinbadOnTerrain);
			m_pSceneNodeTransformer->MoveToAtSpeed(pos,Global::MoveSpeed);
			SetGoInState(m_pGameWaitingState,"AttackAnimation");
		}
		else
		{
			m_Stage="OffensiveSideDice";
		}
	}
	else if(m_Stage=="AttackAnimation")
	{
		m_pEntityTransformer=MyEntityTransformer::Create(m_pSoureceArea->GetArmyEntity());
		
	}

	return true;
}

void MyGameAttackState::on_State_Exit()
{
	
	
	MyGameState::on_State_Exit();
}

MyGameMoveState::MyGameMoveState( std::string name ,MyArea *src,MyArea *dest)
	:MyGameState(name),m_pSourceArea(src),m_pDestinationArea(dest)
{
	//m_pTransformer=new MySceneNodeTransformer(src->)
}

bool MyGameMoveState::frameStarted( const Ogre::FrameEvent& evt )
{
	return true;
}

void MyGameMoveState::on_State_Entry()
{

}

void MyGameMoveState::on_State_Exit()
{

}

MyGameAttackAnimationState::MyGameAttackAnimationState( std::string name,MyArea *src,MyArea *dest )
	:MyGameState(name),m_pSourceArea(src),m_pDestinationArea(dest)
{

}

bool MyGameAttackAnimationState::frameStarted( const Ogre::FrameEvent& evt )
{
	return true;
}

void MyGameAttackAnimationState::on_State_Entry()
{

}

void MyGameAttackAnimationState::on_State_Exit()
{

}

MyGameWaitingState::MyGameWaitingState( std::string name,MyTransformer *transformer )
	:MyGameState(name),m_pTransformer(transformer)
{

}

bool MyGameWaitingState::frameStarted( const Ogre::FrameEvent& evt )
{
	if(m_pTransformer->IsFinished())
	{
		ReturnLastState();
	}
	return true;
}
