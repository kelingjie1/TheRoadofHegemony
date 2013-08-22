#include "StdAfx.h"
#include "MyGameStateManager.h"
#include "MyGameApp.h"
#include "MyTerrain.h"
#include "CollisionTools.h"
#include "Global.h"
#include "MyUIUpdater.h"
#include "MyTransformer.h"
#include "MyBuffManager.h"
#include "MyCardManager.h"
MyGameStateManager* MyGameStateManager::m_pSingleton=0;
MyEventInfo *MyEventInfo::m_pSingleton=0;
MyGameStateManager::MyGameStateManager(void)
	:m_iChoose1(-1),m_iChoose2(-1),m_pCurrentPlayer(0),m_pCurrentState(0),m_pNextState(0),m_LastReturnState(0)
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
				area->SetAreaBelongID(playerid);
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
	new MyEventInfo;
}


MyGameStateManager::~MyGameStateManager(void)
{
	m_pSingleton=0;
	if(m_LastReturnState)
	{
		delete m_LastReturnState;
		m_LastReturnState=0;
	}
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
	MyUIUpdater::GetSingleton().UpdateCardBox();
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
		if (m_iChoose2<0&&area->GetAreaBelongID()==GetCurrentPlayer()->GetID())
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
	
	MyUIUpdater::GetSingleton().UpdateAreaInfo();
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
	MyUIUpdater::GetSingleton().UpdateAreaInfo();
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
	lua_tinker::class_def<MyGameStateManager>(L,"PlayAnimation",&MyGameStateManager::PlayAnimation);
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
	if (m_LastReturnState)
	{
		delete m_LastReturnState;
		m_LastReturnState=0;
	}
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
		m_LastReturnState=m_pCurrentState;
		m_pCurrentState=m_pCurrentState->GetLastState();
		Ogre::LogManager::getSingleton().logMessage("ReturnState:"+m_pCurrentState->GetName());
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

void MyGameStateManager::PlayAnimation( const char *name )
{
	if(std::string(name)=="MineBoom")
		m_pCurrentState->SetGoInState(new MyMineCardState(m_pCurrentState->GetName()+".MineBoom",MyEventInfo::GetSingleton().GetDefenderArea()));
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
	MyUIUpdater::GetSingleton().UpdateMoveTimes();
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
			MyUIUpdater::GetSingleton().UpdateCardBox();
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
			MyUIUpdater::GetSingleton().UpdateCardBox();
		}
	}
}

void MyPlayer::AddBuff( MyBuff *buff )
{
	m_BuffVector.push_back(buff);
	buff->SetOwnerPlayer(this);
}

void MyPlayer::RemoveBuff( MyBuff *buff )
{
	std::vector<MyBuff*>::iterator it=find(m_BuffVector.begin(),m_BuffVector.end(),buff);
	buff->SetOwnerPlayer(0);
	if(it!=m_BuffVector.end())
		m_BuffVector.erase(it);
}

MyBuff * MyPlayer::GetBuffByID( int id )
{
	return m_BuffVector[id];
}

int MyPlayer::GetBuffCount()
{
	return m_BuffVector.size();
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
	m_Stage="Start";
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
	if(nextStage!="")
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
	Ogre::Animation *anim=MyTerrain::GetSingleton().GetSceneManager()->createAnimation("CameraTrack",20);
	anim->setDefaultInterpolationMode(Ogre::Animation::InterpolationMode::IM_SPLINE);
	Ogre::AnimationTrack* track = anim->createNodeTrack(0,MyGameApp::GetSingleton().GetMainCamera()->getParentSceneNode());
	Ogre::TransformKeyFrame *key=dynamic_cast<Ogre::TransformKeyFrame*>(track->createKeyFrame(0));
	key->setTranslate(Ogre::Vector3(10000,100,10000));
	key=dynamic_cast<Ogre::TransformKeyFrame*>(track->createKeyFrame(8));
	key->setTranslate(Ogre::Vector3(2760,1000,2760));
	key=dynamic_cast<Ogre::TransformKeyFrame*>(track->createKeyFrame(11));
	key->setTranslate(Ogre::Vector3(-200,1000,2760));
	key=dynamic_cast<Ogre::TransformKeyFrame*>(track->createKeyFrame(14));
	key->setTranslate(Ogre::Vector3(-200,1000,-200));
	key=dynamic_cast<Ogre::TransformKeyFrame*>(track->createKeyFrame(17));
	key->setTranslate(Ogre::Vector3(2760,1000,-200));
	key=dynamic_cast<Ogre::TransformKeyFrame*>(track->createKeyFrame(20));
	key->setTranslate(Ogre::Vector3(-200,1000,-200));




	Ogre::SceneNode *node=MyTerrain::GetSingleton().GetSceneManager()->getRootSceneNode()->createChildSceneNode("CarmeraLookAt",Ogre::Vector3(1000,50,1000));
	MyGameApp::GetSingleton().GetMainCamera()->setAutoTracking(true,node);
	m_pAnimationState= MyTerrain::GetSingleton().GetSceneManager()->createAnimationState("CameraTrack");
	m_pAnimationState->setEnabled(true);
	m_pAnimationState->setLoop(false);

}

bool MyGameStartState::frameStarted(const Ogre::FrameEvent& evt)
{
	if(m_Stage=="Start")
	{
		m_pAnimationState->addTime(evt.timeSinceLastFrame);
		if(m_pAnimationState->hasEnded())
		{
			MyGameApp::GetSingleton().GetMainCamera()->setAutoTracking(false);
			m_pGameStartAnim->start();
			m_pGameStartMoveAnim->start();
			m_Stage="Move";
		}
	}
	else if(m_Stage=="Move")
	{
		if(!m_pGameStartMoveAnim->isRunning())
		{
			GoNextState();
		}
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


MyGameDiceState::MyGameDiceState( std::string name ,int diceCount )
	:MyGameState(name),m_nDices(diceCount)
{
	m_pRenderSystem=MyTerrain::GetSingleton().GetRenderSystem();
	m_pMyTerrain=&MyTerrain::GetSingleton();
	
}

bool MyGameDiceState::frameStarted( const Ogre::FrameEvent& evt )
{
	using namespace std;
	time+=evt.timeSinceLastFrame;
	bool stop=true;
	for (int i=0;i<m_nDices;i++)
	{
		m_Dice[i].cam->setPosition(m_Dice[i].body->getNode()->getPosition()+Ogre::Vector3(10,200,10));
		m_Dice[i].cam->lookAt(m_Dice[i].body->getNode()->getPosition());
		if (time>5)
		{
			float v1=m_Dice[i].body->getAngularVelocity().normalise();
			float v2=m_Dice[i].body->getLinearVelocity().normalise();
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
		for (int i=0;i<m_nDices;i++)
		{
			Ogre::Vector3 result;
			int fid;
			Ogre::Entity *entity;
			float dis;
			Ogre::Vector3 start=m_Dice[i].body->getNode()->getPosition();
			start.y+=200;
			tool.raycast(Ogre::Ray(start,Ogre::Vector3(0,-1,0)),result,fid,entity,dis);
			if(fid==0||fid==1)
			{
				m_Dice[i].diceNum=1;
			}
			else if(fid==10||fid==11)
			{
				m_Dice[i].diceNum=2;
			}
			else if(fid==2||fid==3)
			{
				m_Dice[i].diceNum=3;
			}
			else if(fid==4||fid==5)
			{
				m_Dice[i].diceNum=4;
			}
			else if(fid==8||fid==9)
			{
				m_Dice[i].diceNum=5;
			}
			else if(fid==6||fid==7)
			{
				m_Dice[i].diceNum=6;
			}
			window->removeViewport(i+1);
			Ogre::SceneNode::ObjectIterator it=m_Dice[i].body->getNode()->getSceneNode()->getAttachedObjectIterator();
			while (it.hasMoreElements())
			{
				MyTerrain::GetSingleton().GetSceneManager()->destroyEntity((Ogre::Entity*)it.getNext());
			}
			m_pRenderSystem->destroyBody(m_Dice[i].body);
		}
		ReturnLastState();
	}
	
	
	return true;
}

void MyGameDiceState::on_State_Entry()
{
	MyGameState::on_State_Entry();


	Ogre::RenderWindow *window=MyGameApp::GetSingleton().GetRenderWindow();
	for (int i=0;i<m_nDices;i++)
	{
		m_Dice[i].cam=m_pMyTerrain->GetSceneManager()->createCamera(m_Name+("DiceCamera"+QString::number(i)).toStdString());
		m_Dice[i].cam->setNearClipDistance(0.1);
		m_Dice[i].cam->setFarClipDistance(10000);
		int size=m_pMyTerrain->GetTerrainGroup()->getTerrain(0,0)->getWorldSize();
		m_Dice[i].bodyDescription.mMass=40.0f;
		m_Dice[i].bodyDescription.mNode=m_pRenderSystem->createNode();
		srand(clock());
		m_Dice[i].body=m_pRenderSystem->createBody(NxOgre::BoxDescription(40),NxOgre::Vec3(size/4+rand()%(size/2)+i%2*i/2*200,1000,size/4+rand()%(size/2)+(i+1)%2*i/2*100), "shaizi.mesh", m_Dice[i].bodyDescription);
		m_Dice[i].body->getNode()->setScale(Ogre::Vector3(60));
		window->addViewport(m_Dice[i].cam,i+1,i%3*1/3.0,i/3/3.0,1/3.0,1/3.0);
	}	
	time=0;
}

void MyGameDiceState::on_State_Exit()
{
	for (int i=0;i<m_nDices;i++)
	{
		m_pMyTerrain->GetSceneManager()->destroyCamera(m_Dice[i].cam);
	}
}

int MyGameDiceState::GetDiceCount()
{
	return m_nDices;
}

int MyGameDiceState::GetDiceNumber( int id )
{
	return m_Dice[id].diceNum;
}

int MyGameDiceState::GetDiceSum()
{
	int sum=0;
	for (int i=0;i<m_nDices;i++)
	{
		sum+=m_Dice->diceNum;
	}
	return sum;
}

MyGameDiceState::~MyGameDiceState()
{

	
}

MyGameAttackState::MyGameAttackState( std::string name ):MyGameState(name)
{
	
}

void MyGameAttackState::on_State_Entry()
{
	MyGameState::on_State_Entry();
	m_pSoureceArea=MyGameStateManager::GetSingleton().GetChooseArea(1);
	m_pDestinationArea=MyGameStateManager::GetSingleton().GetChooseArea(2);

	m_fDrawSwordsTime=m_pSoureceArea->GetArmyEntity()->getAnimationState("DrawSwords")->getLength();
	m_fAttackTime=m_pSoureceArea->GetArmyEntity()->getAnimationState("SliceVertical")->getLength();
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
			m_Stage="Move";
			m_DiceSum1=1;
			m_DiceSum2=0;
		}
		else
		{
			m_pGameDiceState=new MyGameDiceState(m_Name+".OffensiveSideDice",m_pSoureceArea->GetArmyCount());
			SetGoInState(m_pGameDiceState,"OffensiveSideDiceFinish");
		}
	}
	else if(m_Stage=="OffensiveSideDiceFinish")
	{
		m_DiceSum1=m_pGameDiceState->GetDiceSum();
		m_pGameDiceState=new MyGameDiceState(m_Name+".DefensiveSideDice",m_pDestinationArea->GetArmyCount());
		SetGoInState(m_pGameDiceState,"DefensiveSideDiceFinish");
	}
	else if(m_Stage=="DefensiveSideDiceFinish")
	{
		m_DiceSum2=m_pGameDiceState->GetDiceSum();
		m_Stage="Move";
	}
	else if(m_Stage=="Move")
	{
		m_pSceneNodeTransformer=MySceneNodeTransformer::Create(m_pSoureceArea->GetArmySceneNode());
		m_pGameWaitingState=new MyGameWaitingState(m_Name+".Move",m_pSceneNodeTransformer);
		m_SourcePos=m_pSoureceArea->GetArmySceneNode()->getPosition();
		Ogre::Vector3 dir=m_pDestinationArea->GetArmySceneNode()->getPosition()-m_pSoureceArea->GetArmySceneNode()->getPosition();
		dir.normalise();
		Ogre::Vector3 pos=m_pDestinationArea->GetArmySceneNode()->getPosition()-dir*Global::MoveDistance;
		m_pSceneNodeTransformer->SetAutoTurn(true);
		m_pSceneNodeTransformer->SetForwardVector(Ogre::Vector3(0,0,1));
		m_pSceneNodeTransformer->SetOnTerrain(true,Global::HeightOffsetForSinbadOnTerrain);
		m_pSceneNodeTransformer->MoveToAtSpeed(pos,Global::MoveSpeed);

		m_pSourceEntityTransformer=MyEntityTransformer::Create(m_pSoureceArea->GetArmyEntity());
		m_pDestinationEntityTransformer=MyEntityTransformer::Create(m_pDestinationArea->GetArmyEntity());
		m_pSourceEntityTransformer->AddAnimation("RunTop");
		m_pSourceEntityTransformer->AddAnimation("RunBase");
		m_pSourceEntityTransformer->PlayInTime(9999);
		SetGoInState(m_pGameWaitingState,"DrawSwords");
	}
	else if(m_Stage=="DrawSwords")
	{
		m_pSourceEntityTransformer->Stop();
		m_pSourceEntityTransformer->RemoveAllAnimations();
		m_pSourceEntityTransformer->AddAnimation("DrawSwords",false);
		m_pSourceEntityTransformer->PlayInTime(m_fDrawSwordsTime/2);
		m_pGameWaitingState=new MyGameWaitingState(m_Name+".DrawSwords",m_pSourceEntityTransformer);
		SetGoInState(m_pGameWaitingState,"DrawSwordsFinish");
	}
	else if(m_Stage=="DrawSwordsFinish")
	{
		m_pSoureceArea->GetArmyEntity()->detachAllObjectsFromBone();
		m_pSoureceArea->GetArmyEntity()->attachObjectToBone("Handle.L", m_pSoureceArea->GetSword(1));
		m_pSoureceArea->GetArmyEntity()->attachObjectToBone("Handle.R", m_pSoureceArea->GetSword(2));

		m_pDestinationArea->GetArmyEntity()->detachAllObjectsFromBone();
		m_pDestinationArea->GetArmyEntity()->attachObjectToBone("Handle.L", m_pDestinationArea->GetSword(1));
		m_pDestinationArea->GetArmyEntity()->attachObjectToBone("Handle.R", m_pDestinationArea->GetSword(2));

		Ogre::Vector3 dir=-m_pSceneNodeTransformer->GetDirection();
		dir.y=0;
		m_pDestinationArea->GetArmySceneNode()->setOrientation(Ogre::Vector3(0,0,1).getRotationTo(dir));
		m_pSourceEntityTransformer->PlayInTime(m_fDrawSwordsTime/2);
		m_pGameWaitingState=new MyGameWaitingState(m_Name+".DrawSwordsFinish",m_pSourceEntityTransformer);
		SetGoInState(m_pGameWaitingState,"Attack");
	}
	else  if(m_Stage=="Attack")
	{
		m_pSourceEntityTransformer->RemoveAllAnimations();
		m_pSourceEntityTransformer->AddAnimation("SliceVertical");
		m_pSourceEntityTransformer->PlayInTime(m_fAttackTime*4);
		m_pGameWaitingState=new MyGameWaitingState(m_Name+".Attack",m_pSourceEntityTransformer);

		m_pDestinationEntityTransformer->AddAnimation("SliceVertical");
		m_pDestinationEntityTransformer->PlayInTime(m_fAttackTime*4);
		SetGoInState(m_pGameWaitingState,"MoveBack");
	}
	else if(m_Stage=="MoveBack")
	{
		m_pSourceEntityTransformer->RemoveAllAnimations();
		m_pSourceEntityTransformer->AddAnimation("RunTop");
		m_pSourceEntityTransformer->AddAnimation("RunBase");
		m_pSourceEntityTransformer->PlayInTime(9999);
		m_pSoureceArea->GetArmyEntity()->detachAllObjectsFromBone();
		m_pSoureceArea->GetArmyEntity()->attachObjectToBone("Sheath.L", m_pSoureceArea->GetSword(1));
		m_pSoureceArea->GetArmyEntity()->attachObjectToBone("Sheath.R", m_pSoureceArea->GetSword(2));
		m_pDestinationArea->GetArmyEntity()->detachAllObjectsFromBone();
		m_pDestinationArea->GetArmyEntity()->attachObjectToBone("Sheath.L", m_pDestinationArea->GetSword(1));
		m_pDestinationArea->GetArmyEntity()->attachObjectToBone("Sheath.R", m_pDestinationArea->GetSword(2));
		m_pSceneNodeTransformer->MoveToAtSpeed(m_SourcePos,Global::MoveSpeed);
		m_pGameWaitingState=new MyGameWaitingState(m_Name+".MoveBack",m_pSceneNodeTransformer);


		MyEventInfo &info=MyEventInfo::GetSingleton();

		info.AttackerArea=m_pSoureceArea;
		info.DefenderArea=m_pDestinationArea;
		
		if(m_DiceSum1>m_DiceSum2)
		{
			info.WinnerArea=m_pSoureceArea;
			info.Winner=info.WinnerArea->GetAreaBelong();
			info.LoserArea=m_pDestinationArea;
			info.Loser=info.LoserArea->GetAreaBelong();
		}
		else
		{
			info.WinnerArea=m_pDestinationArea;
			info.Winner=info.WinnerArea->GetAreaBelong();
			info.LoserArea=m_pSoureceArea;
			info.Loser=info.LoserArea->GetAreaBelong();
		}

		MyBuffManager::GetSingleton().StateTrigger("BeforeFightWin");
		
		if(info.AttackerArea==info.WinnerArea)
		{
			info.LoserArea->SetAreaBelongID(info.WinnerArea->GetAreaBelongID());
			info.LoserArea->SetArmyCount(info.WinnerArea->GetArmyCount()-1);
			info.WinnerArea->SetArmyCount(1);
		}
		else
		{
			info.LoserArea->SetArmyCount(1);
		}
		MyBuffManager::GetSingleton().StateTrigger("AfterFightWin");
		SetStage("Animation");
		
	}
	else if(m_Stage=="Animation")
	{
		SetGoInState(m_pGameWaitingState,"Finish");
	}
	else if(m_Stage=="Finish")
	{
		m_pSceneNodeTransformer->Release();
		m_pSourceEntityTransformer->Release();
		m_pDestinationEntityTransformer->Release();
		GoNextState();
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

MyEventInfo::MyEventInfo()
{
	if(m_pSingleton)
		throw "Error";
	else
		m_pSingleton=this;
	memset(this,0,sizeof(MyEventInfo));
}

MyEventInfo & MyEventInfo::GetSingleton()
{
	return *m_pSingleton;
}

void MyEventInfo::Clean()
{
	memset(this,0,sizeof(MyEventInfo));
}

void MyEventInfo::DefineInLua( lua_State *L )
{
	lua_tinker::class_add<MyEventInfo>(L,"MyEventInfo");

// 	lua_tinker::class_mem<MyEventInfo>(L,"TriggerPlayer",&MyEventInfo::TriggerPlayer);
// 	lua_tinker::class_mem<MyEventInfo>(L,"Winner",&MyEventInfo::Winner);
// 	lua_tinker::class_mem<MyEventInfo>(L,"Loser",&MyEventInfo::Loser);
// 	lua_tinker::class_mem<MyEventInfo>(L,"WinnerArea",&MyEventInfo::WinnerArea);
// 	lua_tinker::class_mem<MyEventInfo>(L,"LoserArea",&MyEventInfo::LoserArea);
// 	lua_tinker::class_mem<MyEventInfo>(L,"AttackerArea",&MyEventInfo::AttackerArea);
// 	lua_tinker::class_mem<MyEventInfo>(L,"DefenderArea",&MyEventInfo::DefenderArea);
// 	lua_tinker::class_mem<MyEventInfo>(L,"AttackerDiceSum",&MyEventInfo::AttackerDiceSum);
// 	lua_tinker::class_mem<MyEventInfo>(L,"DefenderDiceSum",&MyEventInfo::DefenderDiceSum);

	lua_tinker::class_def<MyEventInfo>(L,"GetTriggerPlayer",&MyEventInfo::GetTriggerPlayer);
	lua_tinker::class_def<MyEventInfo>(L,"GetWinner",&MyEventInfo::GetWinner);
	lua_tinker::class_def<MyEventInfo>(L,"GetLoser",&MyEventInfo::GetLoser);
	lua_tinker::class_def<MyEventInfo>(L,"GetWinnerArea",&MyEventInfo::GetWinnerArea);
	lua_tinker::class_def<MyEventInfo>(L,"GetLoserArea",&MyEventInfo::GetLoserArea);
	lua_tinker::class_def<MyEventInfo>(L,"GetAttackerArea",&MyEventInfo::GetAttackerArea);
	lua_tinker::class_def<MyEventInfo>(L,"GetDefenderArea",&MyEventInfo::GetDefenderArea);
	lua_tinker::class_def<MyEventInfo>(L,"GetAttackerDiceSum",&MyEventInfo::GetAttackerDiceSum);
	lua_tinker::class_def<MyEventInfo>(L,"GetDefenderDiceSum",&MyEventInfo::GetDefenderDiceSum);

	lua_tinker::class_def<MyEventInfo>(L,"SetTriggerPlayer",&MyEventInfo::SetTriggerPlayer);
	lua_tinker::class_def<MyEventInfo>(L,"SetWinner",&MyEventInfo::SetWinner);
	lua_tinker::class_def<MyEventInfo>(L,"SetLoser",&MyEventInfo::SetLoser);
	lua_tinker::class_def<MyEventInfo>(L,"SetWinnerArea",&MyEventInfo::SetWinnerArea);
	lua_tinker::class_def<MyEventInfo>(L,"SetLoserArea",&MyEventInfo::SetLoserArea);
	lua_tinker::class_def<MyEventInfo>(L,"SetAttackerArea",&MyEventInfo::SetAttackerArea);
	lua_tinker::class_def<MyEventInfo>(L,"SetDefenderArea",&MyEventInfo::SetDefenderArea);
	lua_tinker::class_def<MyEventInfo>(L,"SetAttackerDiceSum",&MyEventInfo::SetAttackerDiceSum);
	lua_tinker::class_def<MyEventInfo>(L,"SetDefenderDiceSum",&MyEventInfo::SetDefenderDiceSum);

}

MyMineCardState::MyMineCardState( std::string name,MyArea *area ):MyGameState(name)
{
	m_pArea=area;
	m_pParticleSystem=MyTerrain::GetSingleton().GetSceneManager()->createParticleSystem("MineParticle","Explosion");
	m_pNode=m_pArea->GetArmySceneNode()->createChildSceneNode();
	m_pNode->setScale(Ogre::Vector3(0.1));
	m_pNode->attachObject(m_pParticleSystem);
	m_fRestTime=m_pParticleSystem->getEmitter(0)->getMaxDuration();
	
}

MyMineCardState::~MyMineCardState()
{
	m_pArea->GetArmySceneNode()->removeChild(m_pNode);
	MyTerrain::GetSingleton().GetSceneManager()->destroyParticleSystem(m_pParticleSystem);
}

bool MyMineCardState::frameStarted( const Ogre::FrameEvent& evt )
{
	m_fRestTime-=evt.timeSinceLastFrame;
	if(m_fRestTime<=0)
		ReturnLastState();
	return true;
}
