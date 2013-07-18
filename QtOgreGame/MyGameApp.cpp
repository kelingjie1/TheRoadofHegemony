#include "StdAfx.h"
#include "MyGameApp.h"
#include "MyCardManager.h"
#include "MyUIUpdater.h"
#include "InputHook.h"
#include "Global.h"
MyGameApp* MyGameApp::m_pSingleton=0;
MyGameApp& MyGameApp::GetSingleton()
{
	return *m_pSingleton;
}

MyGameApp::MyGameApp()
{

	if(m_pSingleton)
		throw "Error";
	else
		m_pSingleton=this;
}

void MyGameApp::Init()
{
	Global::LoadGlobleSetting();
	InitOgre();
	InitCEGUI();
	InitResource();
}

void MyGameApp::InitOgre()
{
#ifdef _DEBUG
	m_pRoot=new Ogre::Root("plugins_d.cfg");
#else
	m_pRoot=new Ogre::Root("plugins.cfg");
#endif
	m_pRoot->showConfigDialog();
	m_pRoot->initialise(true,"Ogre by KLJ");

 
//  	Ogre::NameValuePairList params;
//  	params["left"]                 = "0";
//  	params["top"]                  = "0";
//  	params["border"]               = "none";
//  	params["externalWindowHandle"] = Ogre::StringConverter::toString((size_t)(HWND)ui.OgreWidget->winId());
// 	m_pRenderWindow = m_pRoot->createRenderWindow("OgreWindow", 800, 600, false, &params);
	
	m_pRenderWindow=m_pRoot->getAutoCreatedWindow();

	Ogre::ConfigFile cf;
	cf.load("resources.cfg");
	// Go through all sections & settings in the file

	Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();
	Ogre::String secName, typeName, archName;
	while (seci.hasMoreElements())
	{
		secName = seci.peekNextKey();
		Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
		Ogre::ConfigFile::SettingsMultiMap::iterator i;
		for (i = settings->begin(); i != settings->end(); ++i)
		{
			typeName = i->first;
			archName = i->second;
			Ogre::ResourceGroupManager::getSingleton().addResourceLocation(

				archName, typeName, secName);

		}

	}
	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
	
	m_pSceneMgr=Ogre::Root::getSingleton().createSceneManager(Ogre::SceneType::ST_GENERIC);

	m_pRoot->addFrameListener(this);
	Ogre::WindowEventUtilities::addWindowEventListener(m_pRenderWindow,this);

	size_t windowHnd = 0;
	std::ostringstream windowHndStr;
	OIS::ParamList pl;
	m_pRenderWindow->getCustomAttribute("WINDOW", &windowHnd);
	m_hwnd=(HWND)windowHnd;
	windowHndStr << windowHnd;
	pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));
	//非独占

	pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_FOREGROUND" )));
	pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_NONEXCLUSIVE")));
	pl.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_FOREGROUND")));
	pl.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_NONEXCLUSIVE")));
	//ShowCursor(false);

	m_pInputManager = OIS::InputManager::createInputSystem(pl);
	m_pKeyboard = static_cast<OIS::Keyboard*>(m_pInputManager->createInputObject(OIS::OISKeyboard, true));
	m_pMouse = static_cast<OIS::Mouse*>(m_pInputManager->createInputObject(OIS::OISMouse, true));
	if (m_pKeyboard)
		m_pKeyboard->setEventCallback(this);
	if (m_pMouse)
		m_pMouse->setEventCallback(this);
	m_pMouse->getMouseState().width=m_pRenderWindow->getWidth();
	m_pMouse->getMouseState().height=m_pRenderWindow->getHeight();

	m_pMainCamera=m_pSceneMgr->createCamera("MainCamera");
	m_pMainCamera->setNearClipDistance(0.1);
	m_pMainCamera->setFarClipDistance(10000);
	m_pMainCamera->setPosition(Ogre::Vector3(50,1000,50));
	m_pMainCamera->lookAt(Ogre::Vector3(1000,0,1000));
	m_pRenderWindow->addViewport(m_pMainCamera);
}

void MyGameApp::InitScene()
{
	
	m_pMyTerrain=new MyTerrain(m_pSceneMgr);
	m_pGameStateManager=new MyGameStateManager;
	m_pMyTerrain->InitArmy();
	
}

void MyGameApp::DestroyScene()
{
	delete m_pMyTerrain;
	delete m_pGameStateManager;
	m_pSceneMgr->clearScene();
}

bool MyGameApp::frameStarted( const Ogre::FrameEvent& evt )
{
	if(evt.timeSinceLastFrame>0.1)
		*const_cast<Ogre::Real*>(&evt.timeSinceLastFrame)=0.1;


	if(GetAsyncKeyState('W')&0x8000)
	{
		m_pMainCamera->move(100*m_pMainCamera->getDirection()*evt.timeSinceLastFrame);
	}
	if(GetAsyncKeyState('S')&0x8000)
	{
		m_pMainCamera->move(-100*m_pMainCamera->getDirection()*evt.timeSinceLastFrame);
	}
	if(GetAsyncKeyState('A')&0x8000)
	{
		m_pMainCamera->move(-100*m_pMainCamera->getRight()*evt.timeSinceLastFrame);
	}
	if(GetAsyncKeyState('D')&0x8000)
	{
		m_pMainCamera->move(100*m_pMainCamera->getRight()*evt.timeSinceLastFrame);
	}
	if(GetAsyncKeyState('Q')&0x8000)
	{
		m_pMainCamera->move(-100*m_pMainCamera->getUp()*evt.timeSinceLastFrame);
	}
	if(GetAsyncKeyState('E')&0x8000)
	{
		m_pMainCamera->move(100*m_pMainCamera->getUp()*evt.timeSinceLastFrame);
	}
	if(GetAsyncKeyState(VK_UP)&0x8000)
	{
		m_pMainCamera->pitch(Ogre::Radian(evt.timeSinceLastFrame));
	}
	if(GetAsyncKeyState(VK_DOWN)&0x8000)
	{
		m_pMainCamera->pitch(-Ogre::Radian(evt.timeSinceLastFrame));
	}
	if(GetAsyncKeyState(VK_LEFT)&0x8000)
	{
		m_pMainCamera->yaw(Ogre::Radian(evt.timeSinceLastFrame));
	}
	if(GetAsyncKeyState(VK_RIGHT)&0x8000)
	{
		m_pMainCamera->yaw(-Ogre::Radian(evt.timeSinceLastFrame));
	}
	return true;
}

bool MyGameApp::frameRenderingQueued( const Ogre::FrameEvent& evt )
{
	CEGUI::System::getSingleton().renderAllGUIContexts();
	return true;
}

bool MyGameApp::frameEnded( const Ogre::FrameEvent& evt )
{
	if (m_pKeyboard)
		m_pKeyboard->capture();
	if (m_pMouse)
		m_pMouse->capture();
	return true;
}

void MyGameApp::Run()
{
	bool run=true;
	while(run)
	{
		if (m_pRenderWindow->isActive())
		{
			if (m_fTimeDelta<0)
			{
				run=m_pRoot->renderOneFrame();
			}
			else
			{
				run=m_pRoot->renderOneFrame(0);
				m_fTimeDelta=-1;
			}
		}
		else
		{
			m_fTimeDelta=-1;
		}
		
		Ogre::WindowEventUtilities::messagePump();
	}
	
}

void MyGameApp::windowFocusChange( Ogre::RenderWindow* rw )
{

}

void MyGameApp::windowClosed( Ogre::RenderWindow* rw )
{
	m_pRoot->shutdown();
	ExitProcess(0);
}

bool MyGameApp::keyPressed( const OIS::KeyEvent &arg )
{
	CEGUI::System::getSingleton().getDefaultGUIContext().injectKeyDown((CEGUI::Key::Scan)arg.key);
	MyPageManager::GetSingleton().GetCurrentPage()->keyPressed(arg);
	return true; 
}

bool MyGameApp::keyReleased( const OIS::KeyEvent &arg )
{
	CEGUI::System::getSingleton().getDefaultGUIContext().injectKeyUp((CEGUI::Key::Scan)arg.key);
	MyPageManager::GetSingleton().GetCurrentPage()->keyReleased(arg);
	return true; 
}

bool MyGameApp::mouseMoved( const OIS::MouseEvent &arg )
{
	CEGUI::System::getSingleton().getDefaultGUIContext().injectMousePosition(arg.state.X.abs,arg.state.Y.abs);
	MyPageManager::GetSingleton().GetCurrentPage()->mouseMoved(arg);
	return true; 
}

bool MyGameApp::mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
 	CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonDown((CEGUI::MouseButton)id);
	MyPageManager::GetSingleton().GetCurrentPage()->mousePressed(arg,id);
	return true; 
}

bool MyGameApp::mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonUp((CEGUI::MouseButton)id);
	MyPageManager::GetSingleton().GetCurrentPage()->mouseReleased(arg,id);
	return true; 
}

void MyGameApp::InitCEGUI()
{
	m_pCEGUIRenderer=&CEGUI::OgreRenderer::create(*m_pRenderWindow);
	CEGUI::System *sys=&CEGUI::System::create(*m_pCEGUIRenderer);

 	CEGUI::DefaultResourceProvider *rp = static_cast<CEGUI::DefaultResourceProvider*>(sys->getResourceProvider());
 
	rp->setResourceGroupDirectory("schemes", "./datafiles/schemes/");
	rp->setResourceGroupDirectory("imagesets","./datafiles/imagesets/");
	rp->setResourceGroupDirectory("fonts", "./datafiles/fonts/");
	rp->setResourceGroupDirectory("layouts", "./datafiles/layouts/");
	rp->setResourceGroupDirectory("looknfeels","./datafiles/looknfeel/");
	rp->setResourceGroupDirectory("lua_scripts","./datafiles/lua_scripts/");
	// 使用Xerces作为XML解析器
	//rp->setResourceGroupDirectory("schemas","datafiles/xml_schemas/");

	CEGUI::ImageManager::setImagesetDefaultResourceGroup("imagesets");
	CEGUI::Font::setDefaultResourceGroup("fonts");
	CEGUI::Scheme::setDefaultResourceGroup("schemes");
	CEGUI::WidgetLookManager::setDefaultResourceGroup("looknfeels");
	CEGUI::WindowManager::setDefaultResourceGroup("layouts");
	CEGUI::ScriptModule::setDefaultResourceGroup("lua_scripts");
	// 仅当你用xerces做XML解析器,并为它定义了一个资源组的时候使用
	//CEGUI::XercesParser::setSchemaDefaultResourceGroup("schemas");

	CEGUI::SchemeManager::getSingleton().createFromFile("OgreTray.scheme");
	CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().setDefaultImage("OgreTrayImages/MouseArrow");
	
	InputHook::StartHook();

	m_pPageManager=new MyPageManager;
}

MyTerrain* MyGameApp::GetMyTerrain()
{
	return m_pMyTerrain;
}

Ogre::Camera * MyGameApp::GetMainCamera()
{
	return m_pMainCamera;
}

Ogre::RenderWindow * MyGameApp::GetRenderWindow()
{
	return m_pRenderWindow;
}

void MyGameApp::InitResource()
{
	MyBuffManager *BuffManager=new MyBuffManager;
	MyCardManager *CardManager=new MyCardManager;
	MyUIUpdater	*UIUpdater=new MyUIUpdater;
}

void MyGameApp::SetNextTimeDelta( float timeDelta )
{
	m_fTimeDelta=timeDelta;
}

