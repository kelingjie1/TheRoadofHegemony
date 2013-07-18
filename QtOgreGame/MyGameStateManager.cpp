#include "StdAfx.h"
#include "MyGameStateManager.h"
#include "MyGameApp.h"
#include "MyTerrain.h"
#include "CollisionTools.h"
#include "Global.h"
#include "MyUIUpdater.h"
MyGameStateManager* MyGameStateManager::m_pSingleton=0;
MyGameStateManager::MyGameStateManager(void):m_CurrentState(0),m_iChoose1(-1),m_iChoose2(-1)
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



	AddState(new MyGameStartState("GameStartState"));
	AddState(new MyGamePlayerChangeState("GamePlayerChangeState"));
	AddState(new MyGamePlayState("GamePlayState"));
	AddState(new MyGameDiceState("GameDiceState"));
	AddState(new MyGameAttackState("GameAttackState"));
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
	return m_CurrentState;
}

void MyGameStateManager::AddState( MyGameState *state )
{
	m_StateMap[state->GetName()]=state;
}

void MyGameStateManager::SetCurrentState( QString name )
{
	if (m_CurrentState)
	{
		m_CurrentState->on_State_Exit();
	}
	m_LastStateList.push_back(m_CurrentState);
	m_CurrentState=m_StateMap[name];
	m_CurrentState->on_State_Entry();
}

MyPlayer *MyGameStateManager::GetCurrentPlayer()
{
	return m_CurrentPlayer;
}

void MyGameStateManager::SetCurrentPlayerID( int id )
{
	m_CurrentPlayer=GetPlayer(id);
	SetCurrentState("GamePlayerChangeState");
	MyUIUpdater::GetSingleton().on_CardChange();
	m_CurrentPlayer->SetMoveTimes(m_CurrentPlayer->GetMaxMoveTimes());
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
			std::set<MyArea*> &AdjacencySet=MyTerrain::GetSingleton().GetArea(m_iChoose1)->m_AdjacencySet;
			if (AdjacencySet.find(MyTerrain::GetSingleton().GetArea(id))==AdjacencySet.end())
			{
				MyGameApp::GetSingleton().GetMyTerrain()->SetAreaHighLight(m_iChoose1,false);
				m_iChoose1=id;
				MyGameApp::GetSingleton().GetMyTerrain()->SetAreaHighLight(m_iChoose1,true);
			}
			else
			{
				m_iChoose2=id;
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

int MyGameStateManager::GetChoose( int id )
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

MyGameState * MyGameStateManager::GetState( QString name )
{
	if (m_StateMap[name]==0)
	{
		m_StateMap.erase(name);
		return 0;
	}
	else
	{
		return m_StateMap[name];
	}
}

void MyGameStateManager::ReturnLastState()
{
	if (m_LastStateList.empty())
	{
		return;
	}
	if (m_CurrentState)
	{
		m_CurrentState->on_State_Exit();
	}
	m_CurrentState=*m_LastStateList.rbegin();
	m_LastStateList.pop_back();
	m_CurrentState->on_State_Return();
}

void MyGameStateManager::GoInState( QString name )
{
	m_LastStateList.push_back(m_CurrentState);
	m_CurrentState=m_StateMap[name];
	m_CurrentState->on_State_Entry();
}

void MyGameStateManager::TurnNextPlayer()
{
	int id=m_CurrentPlayer->GetID()+1;
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
	lua_tinker::class_def<MyGameStateManager>(L,"GetChoose",&MyGameStateManager::GetChoose);
	lua_tinker::class_def<MyGameStateManager>(L,"GetCurrentPlayer",&MyGameStateManager::GetCurrentPlayer);
	lua_tinker::class_def<MyGameStateManager>(L,"GetPlayer",&MyGameStateManager::GetPlayer);
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

std::list<MyCard*> MyPlayer::GetCardList()
{
	return m_CardList;
}

void MyPlayer::AddCard( MyCard *card )
{
	m_CardList.push_back(card);
	if(this==MyGameStateManager::GetSingleton().GetCurrentPlayer())
	{
		MyUIUpdater::GetSingleton().on_CardChange();
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

MyGameState::MyGameState( QString name )
{
	m_Name=name;
}

QString MyGameState::GetName()
{
	return m_Name;
}

void MyGameState::on_State_Entry()
{

}

void MyGameState::on_State_Exit()
{

}

void MyGameState::on_State_Return()
{

}
bool MyGameStartState::frameStarted(const Ogre::FrameEvent& evt)
{
	if (time<6)
	{
		time+=evt.timeSinceLastFrame;
		CEGUI::WindowManager &winMgr=CEGUI::WindowManager::getSingleton();
		CEGUI::Window *win=MyPageManager::GetSingleton().GetPage("GamePlayingPage")->GetWindow()->getChild("GameStart");
		CEGUI::String num=QString::number(((int)(time*10))%12).toStdString();
		win->setProperty("Image","GameStart/s"+num);
		win->setPosition(CEGUI::UVector2(CEGUI::UDim(time/3-1,0),CEGUI::UDim(0.3,0)));
	}
	else
	{
		MyGameStateManager::GetSingleton().SetCurrentState("GamePlayerChangeState");
	}
	return true;
}

MyGameStartState::MyGameStartState( QString name ):MyGameState(name)
{

}

void MyGameStartState::on_State_Entry()
{
	time=0;
}

MyGamePlayerChangeState::MyGamePlayerChangeState( QString name ):MyGameState(name)
{

}

bool MyGamePlayerChangeState::frameStarted( const Ogre::FrameEvent& evt )
{
	if (time<3)
	{
		time+=evt.timeSinceLastFrame;
		CEGUI::WindowManager &winMgr=CEGUI::WindowManager::getSingleton();
		CEGUI::Window *win=MyPageManager::GetSingleton().GetPage("GamePlayingPage")->GetWindow()->getChild("PlayerChange");
		win->setPosition(CEGUI::UVector2(CEGUI::UDim(time/2-0.5,0),CEGUI::UDim(0.3,0)));
		
	}
	else
	{
		MyGameStateManager::GetSingleton().SetCurrentState("GamePlayState");
	}
	return true;
}

void MyGamePlayerChangeState::on_State_Entry()
{
	time=0;
	MyPlayer *player=MyGameStateManager::GetSingleton().GetCurrentPlayer();
	CEGUI::WindowManager &winMgr=CEGUI::WindowManager::getSingleton();
	CEGUI::Window *win=MyPageManager::GetSingleton().GetPage("GamePlayingPage")->GetWindow()->getChild("PlayerChange");
	win->setText((CEGUI::utf8*)(QStringLiteral("玩家")+QString::number(player->GetID())+QStringLiteral("行动")).toUtf8().data());
	win->setVisible(true);
}

void MyGamePlayerChangeState::on_State_Exit()
{
	CEGUI::Window *win=MyPageManager::GetSingleton().GetPage("GamePlayingPage")->GetWindow()->getChild("PlayerChange");
	win->setVisible(false);
}

MyGamePlayState::MyGamePlayState( QString name ):MyGameState(name)
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


MyGameDiceState::MyGameDiceState( QString name ):MyGameState(name)
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
		MyGameStateManager::GetSingleton().ReturnLastState();
	}
	
	
	return true;
}

void MyGameDiceState::on_State_Entry()
{
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


MyGameAttackState::MyGameAttackState( QString name ):MyGameState(name)
{

}

bool MyGameAttackState::frameStarted( const Ogre::FrameEvent& evt )
{
	time+=evt.timeSinceLastFrame;
	if (returntimes==2)
	{
		if (state==0)
		{
			if (time<len/Global::MoveSpeed)
			{
				Ogre::Vector3 pos=source+dir*time*Global::MoveSpeed;
				pos.y=MyTerrain::GetSingleton().GetTerrainGroup()->getHeightAtWorldPosition(pos.x,0,pos.z)+25;
				m_Area1->m_ArmyNode->setPosition(pos);
				m_Area1->m_Army->getAnimationState("RunBase")->addTime(evt.timeSinceLastFrame);
				m_Area1->m_Army->getAnimationState("RunTop")->addTime(evt.timeSinceLastFrame);
			}
			else
			{
				state=1;
				time=0;
				
				m_Area1->m_Army->getAnimationState("RunBase")->setEnabled(false);
				m_Area1->m_Army->getAnimationState("RunTop")->setEnabled(false);

				m_Area1->m_Army->getAnimationState("DrawSwords")->setEnabled(true);
				m_Area2->m_Army->getAnimationState("DrawSwords")->setEnabled(true);

				

			}
		}
		else if (state==1)
		{
			

			if(time>m_Area2->m_Army->getAnimationState("DrawSwords")->getLength()/2&&time-evt.timeSinceLastFrame<m_Area2->m_Army->getAnimationState("DrawSwords")->getLength()/2)
			{


				m_Area1->m_Army->detachAllObjectsFromBone();
				m_Area1->m_Army->attachObjectToBone("Handle.L", m_Area1->m_Sword1);
				m_Area1->m_Army->attachObjectToBone("Handle.R", m_Area1->m_Sword2);

				m_Area2->m_Army->detachAllObjectsFromBone();
				m_Area2->m_Army->attachObjectToBone("Handle.L", m_Area2->m_Sword1);
				m_Area2->m_Army->attachObjectToBone("Handle.R", m_Area2->m_Sword2);
			}
			if (time<m_Area1->m_Army->getAnimationState("DrawSwords")->getLength())
			{
				m_Area1->m_Army->getAnimationState("DrawSwords")->addTime(evt.timeSinceLastFrame);
				m_Area2->m_Army->getAnimationState("DrawSwords")->addTime(evt.timeSinceLastFrame);
			}
			else
			{
				state=2;
				time=0;
				m_Area1->m_Army->getAnimationState("DrawSwords")->setEnabled(false);
				m_Area2->m_Army->getAnimationState("DrawSwords")->setEnabled(false);

				m_Area1->m_Army->getAnimationState("SliceVertical")->setEnabled(true);
				m_Area2->m_Army->getAnimationState("SliceVertical")->setEnabled(true);
			}
		}
		else if (state==2)
		{
			if (time<m_Area2->m_Army->getAnimationState("SliceVertical")->getLength()*4)
			{
				m_Area1->m_Army->getAnimationState("SliceVertical")->addTime(evt.timeSinceLastFrame);
				m_Area2->m_Army->getAnimationState("SliceVertical")->addTime(evt.timeSinceLastFrame);
			}
			else
			{
				state=3;
				time=0;
				m_Area1->m_Army->getAnimationState("DrawSwords")->setEnabled(true);
				m_Area2->m_Army->getAnimationState("DrawSwords")->setEnabled(true);
			}
		}
		else if (state==3)
		{
			if(time>m_Area2->m_Army->getAnimationState("DrawSwords")->getLength()/2&&time-evt.timeSinceLastFrame<m_Area2->m_Army->getAnimationState("DrawSwords")->getLength()/2)
			{


				m_Area1->m_Army->detachAllObjectsFromBone();
				m_Area1->m_Army->attachObjectToBone("Sheath.L", m_Area1->m_Sword1);
				m_Area1->m_Army->attachObjectToBone("Sheath.R", m_Area1->m_Sword2);

				m_Area2->m_Army->detachAllObjectsFromBone();
				m_Area2->m_Army->attachObjectToBone("Sheath.L", m_Area2->m_Sword1);
				m_Area2->m_Army->attachObjectToBone("Sheath.R", m_Area2->m_Sword2);
			}
			if (time<m_Area2->m_Army->getAnimationState("DrawSwords")->getLength())
			{
				m_Area1->m_Army->getAnimationState("DrawSwords")->addTime(-evt.timeSinceLastFrame);
				m_Area2->m_Army->getAnimationState("DrawSwords")->addTime(-evt.timeSinceLastFrame);
			}
			else
			{
				state=4;
				time=0;
				if(diceNum1>diceNum2)
				{
					int count=m_Area1->GetArmyCount();
					m_Area2->SetArmyCount(0);
					m_Area2->SetAreaBelong(m_Area1->GetAreaBelong());
					m_Area2->SetArmyCount(count-1);
					m_Area1->SetArmyCount(1);
				}
				else
				{
					m_Area1->SetArmyCount(1);
				}
				Ogre::Vector3 walkdir=-dir;
				walkdir.y=0;
				Ogre::Quaternion q1=Ogre::Vector3(0,0,1).getRotationTo(walkdir);
				Ogre::Quaternion q2(Ogre::Radian(),Ogre::Vector3(0,1,0));
				m_Area1->m_ArmyNode->setOrientation(q1);
				m_Area2->m_ArmyNode->setOrientation(q2);
				m_Area1->m_Army->getAnimationState("RunBase")->setEnabled(true);
				m_Area1->m_Army->getAnimationState("RunTop")->setEnabled(true);
			}
		}
		else if (state==4)
		{
			if(time<len/Global::MoveSpeed)
			{
				Ogre::Vector3 pos=source+dir*(len-time*Global::MoveSpeed);
				pos.y=MyTerrain::GetSingleton().GetTerrainGroup()->getHeightAtWorldPosition(pos.x,0,pos.z)+25;
				m_Area1->m_ArmyNode->setPosition(pos);
				m_Area1->m_Army->getAnimationState("RunBase")->addTime(evt.timeSinceLastFrame);
				m_Area1->m_Army->getAnimationState("RunTop")->addTime(evt.timeSinceLastFrame);
			}
			else
			{
				Ogre::Quaternion q1(Ogre::Radian(),Ogre::Vector3(0,1,0));
				m_Area1->m_ArmyNode->setOrientation(q1);
				Ogre::Vector3 pos=source;
				pos.y=MyTerrain::GetSingleton().GetTerrainGroup()->getHeightAtWorldPosition(pos.x,0,pos.z)+25;
				m_Area1->m_ArmyNode->setPosition(pos);
				MyGameStateManager::GetSingleton().SetCurrentState("GamePlayState");
			}
		}
	}
	return true;
}

void MyGameAttackState::on_State_Entry()
{
	returntimes=0;
	time=0;
	m_iChoose1=MyGameStateManager::GetSingleton().GetChoose(1);
	m_iChoose2=MyGameStateManager::GetSingleton().GetChoose(2);

	m_Area1=MyTerrain::GetSingleton().GetArea(m_iChoose1);
	m_Area2=MyTerrain::GetSingleton().GetArea(m_iChoose2);

	MyGameStateManager &stateMgr=MyGameStateManager::GetSingleton();

	if (m_Area2->GetArmyCount()==0)
	{
		returntimes=2;
		state=0;
		diceNum1=1;diceNum2=0;
		source=m_Area1->m_ArmyNode->getPosition();
		dest=m_Area2->m_ArmyNode->getPosition();
		dir=dest-source;
		len=dir.length()-20;
		dir.normalise();
		dest-=dir*20;
		Ogre::Vector3 walkdir=dir;
		walkdir.y=0;
		Ogre::Quaternion q1=Ogre::Vector3(0,0,1).getRotationTo(walkdir);
		Ogre::Quaternion q2=Ogre::Vector3(0,0,1).getRotationTo(-walkdir);
		m_Area1->m_ArmyNode->setOrientation(q1);
		m_Area2->m_ArmyNode->setOrientation(q2);

		m_Area1->m_Army->getAnimationState("RunBase")->setEnabled(true);
		m_Area1->m_Army->getAnimationState("RunTop")->setEnabled(true);
	}
	else
	{
		dynamic_cast<MyGameDiceState*>(stateMgr.GetState("GameDiceState"))->nDices=m_Area1->GetArmyCount();
		stateMgr.GoInState("GameDiceState");
		diceNum1=0;diceNum2=0;
		state=0;
	}
}

void MyGameAttackState::on_State_Return()
{
	returntimes++;
	MyGameStateManager &stateMgr=MyGameStateManager::GetSingleton();
	MyGameDiceState *diceState=dynamic_cast<MyGameDiceState*>(stateMgr.GetState("GameDiceState"));
	if (returntimes==1)
	{
		for (int i=0;i<diceState->nDices;i++)
		{
			diceNum1+=diceState->dice[i].diceNum;
		}

		diceState->nDices=m_Area2->GetArmyCount();
		stateMgr.GoInState("GameDiceState");
	}
	else
	{
		for (int i=0;i<diceState->nDices;i++)
		{
			diceNum2+=diceState->dice[i].diceNum;
		}
		source=m_Area1->m_ArmyNode->getPosition();
		dest=m_Area2->m_ArmyNode->getPosition();
		dir=dest-source;
		len=dir.length()-20;
		dir.normalise();
		dest-=dir*20;
		
		Ogre::Vector3 walkdir=dir;
		walkdir.y=0;
		Ogre::Quaternion q1=Ogre::Vector3(0,0,1).getRotationTo(walkdir);
		Ogre::Quaternion q2=Ogre::Vector3(0,0,1).getRotationTo(-walkdir);
		m_Area1->m_ArmyNode->setOrientation(q1);
		m_Area2->m_ArmyNode->setOrientation(q2);

		m_Area1->m_Army->getAnimationState("RunBase")->setEnabled(true);
		m_Area1->m_Army->getAnimationState("RunTop")->setEnabled(true);
	}
	
}
