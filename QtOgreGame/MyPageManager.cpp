#include "stdafx.h"
#include "MyGameApp.h"
#include "MyPageManager.h"
#include "MyCardManager.h"
#include "Global.h"
MyPageManager *MyPageManager::m_pSingleton=0;

MyPageManager::MyPageManager():m_CurrentPage(0)
{
	if(m_pSingleton)
		throw "Error";
	else
		m_pSingleton=this;

	AddNewPage(new MyMenuPage("MenuPage"));
	AddNewPage(new MyGameSettingPage("GameSettingPage"));
	AddNewPage(new MyGameLoadingPage("GameLoadingPage"));
	AddNewPage(new MyGamePlayingPage("GamePlayingPage"));
	
// 	for (std::map<CEGUI::String,MyPage*>::iterator it=m_WindowMap.begin();it!=m_WindowMap.end();it++)
// 	{
// 		CEGUI::System::getSingleton().setGUISheet(it->second->GetWindow());
// 	}

	ChangePage("MenuPage");
}

void MyPageManager::ChangePage( char* name )
{
	if (m_CurrentPage!=0)
	{
		m_CurrentPage->GetWindow()->deactivate();
		m_CurrentPage->GetWindow()->hide();
		m_CurrentPage->OnPageUnload();
	}
	m_CurrentPage=m_WindowMap[name];
	CEGUI::System::getSingleton().getDefaultGUIContext().setRootWindow(m_CurrentPage->GetWindow());
	m_CurrentPage->GetWindow()->activate();
	m_CurrentPage->GetWindow()->show();
	m_CurrentPage->OnPageLoad();
	
}

MyPageManager & MyPageManager::GetSingleton()
{
	return *m_pSingleton;
}

MyPage *MyPageManager::GetCurrentPage()
{
	return m_CurrentPage;
}

void MyPageManager::AddNewPage( MyPage *page )
{
	m_WindowMap[page->GetName()]=page;
}

MyPage * MyPageManager::GetPage( char *name )
{
	return m_WindowMap[name];
}


CEGUI::Window	*MyPage::GetWindow()
{
	return m_pWindow;
}

void MyPage::OnPageLoad()
{

}

void MyPage::OnPageUnload()
{

}

MyPage::MyPage( const char *name ) :m_szName(name)
{

}

std::string MyPage::GetName()
{
	return m_szName;
}

bool MyPage::keyPressed( const OIS::KeyEvent &arg )
{
	return false;
}

bool MyPage::keyReleased( const OIS::KeyEvent &arg )
{
	return false;
}

bool MyPage::mouseMoved( const OIS::MouseEvent &arg )
{
	return false;
}

bool MyPage::mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	return false;
}

bool MyPage::mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	return false;
}

MyMenuPage::MyMenuPage( const char *name ) :MyPage(name)
{
	m_pWindow=CEGUI::WindowManager::getSingleton().loadLayoutFromFile("MenuPage.layout");
	CEGUI::WindowManager &winMgr=CEGUI::WindowManager::getSingleton();
	m_pWindow->getChild("Background/StartGame")->subscribeEvent(CEGUI ::PushButton::EventClicked,CEGUI::Event::Subscriber(&MyMenuPage::on_StartGame_clicked,this));
}
bool MyMenuPage::on_StartGame_clicked(const CEGUI::EventArgs& e)
{
	MyPageManager::GetSingleton().ChangePage("GameSettingPage");
	return true;
}




MyGameSettingPage::MyGameSettingPage( const char *name ) :MyPage(name)
{
	m_pWindow=CEGUI::WindowManager::getSingleton().loadLayoutFromFile("GameSettingPage.layout");
	CEGUI::WindowManager &winMgr=CEGUI::WindowManager::getSingleton();
	m_pWindow->getChild("Background/OK")->subscribeEvent(CEGUI ::PushButton::EventClicked,CEGUI::Event::Subscriber(&MyGameSettingPage::on_OK_clicked,this));

	CEGUI::Combobox *PlayerCount=dynamic_cast<CEGUI::Combobox*>(m_pWindow->getChild("Background/PlayerCount"));
	PlayerCount->addItem(new CEGUI::ListboxTextItem("1",1));
	PlayerCount->addItem(new CEGUI::ListboxTextItem("2",2));
	PlayerCount->addItem(new CEGUI::ListboxTextItem("3",3));
	PlayerCount->addItem(new CEGUI::ListboxTextItem("4",4));
	PlayerCount->addItem(new CEGUI::ListboxTextItem("5",5));
	PlayerCount->addItem(new CEGUI::ListboxTextItem("6",6));
	PlayerCount->addItem(new CEGUI::ListboxTextItem("7",7));
	PlayerCount->addItem(new CEGUI::ListboxTextItem("8",8));
	PlayerCount->setText(CEGUIText("请选择玩家数量"));
	PlayerCount->subscribeEvent(CEGUI::Combobox::EventListSelectionAccepted,CEGUI::Event::Subscriber(&MyGameSettingPage::on_PlayerCount_ListSelectionAccepted,this));
	for (int i=1;i<=8;i++)
	{
		CEGUI::Window *win;
		CEGUI::String id="0";
		id[0]='0'+i;
		win=m_pWindow->getChild("Background/Name"+id);
		win->setVisible(false);
		win=m_pWindow->getChild("Background/Hero"+id);
		win->setVisible(false);
		win=m_pWindow->getChild("Background/Race"+id);
		win->setVisible(false);
		win=m_pWindow->getChild("Background/Camp"+id);
		win->setVisible(false);
		win=m_pWindow->getChild("Background/Color"+id);
		win->setVisible(false);
	}
}

bool MyGameSettingPage::on_OK_clicked( const CEGUI::EventArgs& e )
{
	MyPageManager::GetSingleton().ChangePage("GameLoadingPage");
	return true;
}

bool MyGameSettingPage::on_PlayerCount_ListSelectionAccepted( const CEGUI::EventArgs& e )
{
	CEGUI::WindowManager &winMgr=CEGUI::WindowManager::getSingleton();
	CEGUI::Combobox *PlayerCount=dynamic_cast<CEGUI::Combobox*>(m_pWindow->getChild("Background/PlayerCount"));
	int n=QString(PlayerCount->getText().c_str()).toInt();
	for (int i=1;i<=8;i++)
	{
		CEGUI::Window *win;
		CEGUI::String id="0";
		id[0]='0'+i;
		win=m_pWindow->getChild("Background/Name"+id);
		win->setVisible(i<=n);
		win=m_pWindow->getChild("Background/Hero"+id);
		win->setVisible(i<=n);
		win=m_pWindow->getChild("Background/Race"+id);
		win->setVisible(i<=n);
		win=m_pWindow->getChild("Background/Camp"+id);
		win->setVisible(i<=n);
		win=m_pWindow->getChild("Background/Color"+id);
		win->setVisible(i<=n);
	}
	return true;
}

MyGamePlayingPage::MyGamePlayingPage( const char *name ) :MyPage(name)
{
	CEGUI::WindowManager &winMgr=CEGUI::WindowManager::getSingleton();
	m_pWindow=winMgr.loadLayoutFromFile("GamePlayingPage.layout");
	m_pWindow->getChild("Menu/Restart")->subscribeEvent(CEGUI ::PushButton::EventClicked,CEGUI::Event::Subscriber(&MyGamePlayingPage::on_Restart_clicked,this));
	m_pWindow->getChild("Menu/Return")->subscribeEvent(CEGUI ::PushButton::EventClicked,CEGUI::Event::Subscriber(&MyGamePlayingPage::on_Return_clicked,this));
	m_pWindow->getChild("Attack/OK")->subscribeEvent(CEGUI ::PushButton::EventClicked,CEGUI::Event::Subscriber(&MyGamePlayingPage::on_Attack_clicked,this));
	m_pWindow->getChild("Attack/Cancel")->subscribeEvent(CEGUI ::PushButton::EventClicked,CEGUI::Event::Subscriber(&MyGamePlayingPage::on_AttackCancel_clicked,this));
	m_pWindow->getChild("Move/OK")->subscribeEvent(CEGUI ::PushButton::EventClicked,CEGUI::Event::Subscriber(&MyGamePlayingPage::on_Move_clicked,this));
	m_pWindow->getChild("Move/Cancel")->subscribeEvent(CEGUI ::PushButton::EventClicked,CEGUI::Event::Subscriber(&MyGamePlayingPage::on_MoveCancel_clicked,this));
	m_pWindow->getChild("Build/Building1")->subscribeEvent(CEGUI ::PushButton::EventClicked,CEGUI::Event::Subscriber(&MyGamePlayingPage::on_Building1_clicked,this));
	m_pWindow->getChild("Build/Building2")->subscribeEvent(CEGUI ::PushButton::EventClicked,CEGUI::Event::Subscriber(&MyGamePlayingPage::on_Building2_clicked,this));
	m_pWindow->getChild("Build/Building3")->subscribeEvent(CEGUI ::PushButton::EventClicked,CEGUI::Event::Subscriber(&MyGamePlayingPage::on_Building3_clicked,this));
	m_pWindow->getChild("Build/Cancel")->subscribeEvent(CEGUI ::PushButton::EventClicked,CEGUI::Event::Subscriber(&MyGamePlayingPage::on_BuildCancel_clicked,this));
	m_pWindow->getChild("EndTurn")->subscribeEvent(CEGUI ::PushButton::EventClicked,CEGUI::Event::Subscriber(&MyGamePlayingPage::on_EndTurn_clicked,this));
	m_pWindow->getChild("Menu")->hide();

	CEGUI::Window *win=m_pWindow->createChild("OgreTray/StaticImage","GameStart");
	win->setProperty("Image","GameStart/s0");
	win->setArea(CEGUI::UVector2(CEGUI::UDim(0.4,0),CEGUI::UDim(0.3,0)),CEGUI::USize(CEGUI::UDim(0.3,0),CEGUI::UDim(0.3,0)));
	win->setProperty("FrameEnabled","false");
	win->setProperty("BackgroundEnabled","false");
	

	win=m_pWindow->createChild("OgreTray/StaticText","PlayerChange");
	win->setArea(CEGUI::UVector2(CEGUI::UDim(0.4,0),CEGUI::UDim(0.3,0)),CEGUI::USize(CEGUI::UDim(0.3,0),CEGUI::UDim(0.3,0)));
	win->setProperty("FrameEnabled","false");
	win->setProperty("BackgroundEnabled","false");
	win->setFont("hwxk40");
	win->setProperty("TextColours","tl:FFFFF263 tr:FFFFF263 bl:FFFFF263 br:FFFFF263");
	win->setVisible(false);

	win=m_pWindow->getChild("Attack");
	win->setVisible(false);
	win=m_pWindow->getChild("Build");
	win->setVisible(false);
	win=m_pWindow->getChild("Move");
	win->setVisible(false);
	win=m_pWindow->getChild("AreaInfo1");
	win->setVisible(false);
	win=m_pWindow->getChild("AreaInfo2");
	win->setVisible(false);
	win=m_pWindow->getChild("Action");
	win->setVisible(false);

	CEGUI::ScrollablePane *cardbox=dynamic_cast<CEGUI::ScrollablePane*>(m_pWindow->getChild("CardBox"));
	cardbox->setContentPaneAutoSized(true);

}

bool MyGamePlayingPage::on_Restart_clicked( const CEGUI::EventArgs& e )
{
	

	return true;
}

bool MyGamePlayingPage::on_Return_clicked( const CEGUI::EventArgs& e )
{
	MyGameApp::GetSingleton().DestroyScene();
	MyPageManager::GetSingleton().ChangePage("MenuPage");
	return true;
}

bool MyGamePlayingPage::mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	if (StateManager)
		StateManager->GetCurrentState()->mousePressed(arg,id);
	return true;
}

bool MyGamePlayingPage::keyPressed( const OIS::KeyEvent &arg )
{
	if (StateManager)
		StateManager->GetCurrentState()->keyPressed(arg);
	if (arg.key==OIS::KC_ESCAPE)
	{
		CEGUI::Window *win=m_pWindow->getChild("Menu");
		win->setVisible(!win->isVisible());
	}
	return true;
}

bool MyGamePlayingPage::frameStarted( const Ogre::FrameEvent& evt )
{
	if (StateManager)
		StateManager->GetCurrentState()->frameStarted(evt);
	CEGUI::Vector2f p=CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().getPosition();
	Ogre::Camera *cam=MyGameApp::GetSingleton().GetMainCamera();
	if (p.d_y<10)
	{
		Ogre::Vector3 d=cam->getDirection();
		d.y=0;
		d.normalise();
		MyGameApp::GetSingleton().GetMainCamera()->move(d*evt.timeSinceLastFrame*200);
	}
	if (p.d_y>MyGameApp::GetSingleton().GetRenderWindow()->getHeight()-10)
	{
		Ogre::Vector3 d=cam->getDirection();
		d.y=0;
		d.normalise();
		MyGameApp::GetSingleton().GetMainCamera()->move(-d*evt.timeSinceLastFrame*200);
	}
	if (p.d_x<10)
	{
		cam->move(-cam->getRight()*evt.timeSinceLastFrame*200);
	}
	if (p.d_x>MyGameApp::GetSingleton().GetRenderWindow()->getWidth()-10)
	{
		cam->move(cam->getRight()*evt.timeSinceLastFrame*200);
	}
	return true;
}

void MyGamePlayingPage::OnPageLoad()
{
	Ogre::Root::getSingleton().addFrameListener(this);
	StateManager=&MyGameStateManager::GetSingleton();

	StateManager->SetCurrentPlayerID(1);
	StateManager->SetCurrentState("GameStartState");
	

	lua_State *L=lua_open();
	luaopen_base(L);
	MyTerrain::DefineInLua(L);
	MyCard::DefineInLua(L);
	MyCardManager::DefineInLua(L);
	MyPlayer::DefineInLua(L);
	MyGameStateManager::DefineInLua(L);
	lua_tinker::set(L,"Terrain",&MyTerrain::GetSingleton());
	lua_tinker::set(L,"GameStateManager",&MyGameStateManager::GetSingleton());
	lua_tinker::set(L,"CardManager",&MyCardManager::GetSingleton());
	lua_tinker::dofile(L,"./media/lua/PlayingPageInit.lua");
	lua_close(L);
}

void MyGamePlayingPage::OnPageUnload()
{
	Ogre::Root::getSingleton().removeFrameListener(this);
}

bool MyGamePlayingPage::mouseMoved( const OIS::MouseEvent &arg )
{
	if (StateManager)
		StateManager->GetCurrentState()->mouseMoved(arg);
	MyGameApp::GetSingleton().GetMainCamera()->move(MyGameApp::GetSingleton().GetMainCamera()->getDirection()*arg.state.Z.rel);
	return true;
}

bool MyGamePlayingPage::keyReleased( const OIS::KeyEvent &arg )
{
	if (StateManager)
		StateManager->GetCurrentState()->keyReleased(arg);
	return true;
}

bool MyGamePlayingPage::mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	if (StateManager)
		StateManager->GetCurrentState()->mouseReleased(arg,id);
	return true;
}

bool MyGamePlayingPage::on_Attack_clicked( const CEGUI::EventArgs& e )
{
	MyPlayer *player=MyGameStateManager::GetSingleton().GetCurrentPlayer();
	player->SetMoveTimes(player->GetMoveTimes()-1);
	MyGameStateManager &stateMgr=MyGameStateManager::GetSingleton();
	MyGameAttackState *attackstate=dynamic_cast<MyGameAttackState*>(stateMgr.GetState("GameAttackState"));
	attackstate->m_iChoose1=stateMgr.GetChoose(1);
	attackstate->m_iChoose2=stateMgr.GetChoose(2);
	stateMgr.SetCurrentState("GameAttackState");
	stateMgr.ClearChoose();
	m_pWindow->getChild("Attack")->setVisible(false);
	return true;
}

bool MyGamePlayingPage::on_AttackCancel_clicked( const CEGUI::EventArgs& e )
{
	m_pWindow->getChild("Attack")->setVisible(false);
	MyGameStateManager::GetSingleton().ClearChoose();
	return true;
}

bool MyGamePlayingPage::on_Move_clicked( const CEGUI::EventArgs& e )
{
	MyPlayer *player=MyGameStateManager::GetSingleton().GetCurrentPlayer();
	player->SetMoveTimes(player->GetMoveTimes()-1);
	MyGameStateManager::GetSingleton().ClearChoose();
	return true;
}

bool MyGamePlayingPage::on_MoveCancel_clicked( const CEGUI::EventArgs& e )
{
	m_pWindow->getChild("Move")->setVisible(false);
	MyGameStateManager::GetSingleton().ClearChoose();
	return true;
}

bool MyGamePlayingPage::on_Building1_clicked( const CEGUI::EventArgs& e )
{
	MyGameStateManager::GetSingleton().ClearChoose();
	return true;
}

bool MyGamePlayingPage::on_Building2_clicked( const CEGUI::EventArgs& e )
{
	MyGameStateManager::GetSingleton().ClearChoose();
	return true;
}

bool MyGamePlayingPage::on_Building3_clicked( const CEGUI::EventArgs& e )
{
	MyGameStateManager::GetSingleton().ClearChoose();
	return true;
}

bool MyGamePlayingPage::on_BuildCancel_clicked( const CEGUI::EventArgs& e )
{
	m_pWindow->getChild("Build")->setVisible(false);
	MyGameStateManager::GetSingleton().ClearChoose();
	return true;
}

bool MyGamePlayingPage::on_EndTurn_clicked( const CEGUI::EventArgs& e )
{
	MyGameStateManager::GetSingleton().TurnNextPlayer();
	return true;
}



MyGameLoadingPage::MyGameLoadingPage( const char *name ) :MyPage(name)
{
	m_pWindow=CEGUI::WindowManager::getSingleton().loadLayoutFromFile("GameLoadingPage.layout");
}

void MyGameLoadingPage::OnPageLoad()
{
	Ogre::Root::getSingleton().addFrameListener(this);
	MyGameApp::GetSingleton().InitScene();
}

void MyGameLoadingPage::OnPageUnload()
{
	Ogre::Root::getSingleton().removeFrameListener(this);
}

bool MyGameLoadingPage::frameStarted(const Ogre::FrameEvent& evt)
{
	MyPageManager::GetSingleton().ChangePage("GamePlayingPage");
	return true;
}
