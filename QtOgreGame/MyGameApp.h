#pragma once
#include <Ogre.h>
#include <OIS.h>
#include "MyTerrain.h"
#include "MyGameStateManager.h"
#include "MyPageManager.h"
class MyGameApp : public Ogre::FrameListener,public OIS::KeyListener, OIS::MouseListener,public Ogre::WindowEventListener
{
public:
	MyGameApp();
	static MyGameApp& GetSingleton();
	void Init();
	void Run();
	Ogre::Camera *GetMainCamera();
	MyTerrain*	GetMyTerrain();
	Ogre::RenderWindow *GetRenderWindow();
	void InitScene();
	void DestroyScene();

	void SetNextTimeDelta(float timeDelta=0);

public slots:
		void RenderLoop();
private:

	static MyGameApp				*m_pSingleton;

	Ogre::Root						*m_pRoot;
	Ogre::RenderWindow				*m_pRenderWindow;
	Ogre::SceneManager				*m_pSceneMgr;
	CEGUI::OgreRenderer				*m_pCEGUIRenderer;

	HWND							m_hwnd;
	OIS::InputManager				*m_pInputManager;
	OIS::Keyboard					*m_pKeyboard;
	OIS::Mouse						*m_pMouse;

	Ogre::Camera					*m_pMainCamera;
	MyTerrain						*m_pMyTerrain;
	MyGameStateManager				*m_pGameStateManager;
	MyPageManager					*m_pPageManager;

	float							m_fTimeDelta;
	void InitOgre();
	void InitCEGUI();
	void InitResource();

	virtual bool frameStarted(const Ogre::FrameEvent& evt);
	virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);
	virtual bool frameEnded(const Ogre::FrameEvent& evt);
	virtual void windowFocusChange(Ogre::RenderWindow* rw);
	virtual void windowClosed(Ogre::RenderWindow* rw);
	virtual bool keyPressed(const OIS::KeyEvent &arg);
	virtual bool keyReleased(const OIS::KeyEvent &arg);
	virtual bool mouseMoved(const OIS::MouseEvent &arg);
	virtual bool mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
	virtual bool mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
};

