#pragma once
#include "stdafx.h"
class MyPage
{
public:
	MyPage(const char *name);
	CEGUI::Window	*GetWindow();
	std::string	GetName();

	virtual void	OnPageLoad();
	virtual void	OnPageUnload();

	virtual bool keyPressed(const OIS::KeyEvent &arg);
	virtual bool keyReleased(const OIS::KeyEvent &arg);
	virtual bool mouseMoved(const OIS::MouseEvent &arg);
	virtual bool mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
	virtual bool mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
protected:
	CEGUI::Window	*m_pWindow;
	std::string		m_szName;
};
class MyPageManager
{
public:
	MyPageManager();

	static MyPageManager &GetSingleton();

	void ChangePage(char* name);
	MyPage *GetPage(char *name);
	MyPage	*GetCurrentPage();
	void AddNewPage(MyPage *page);
private:
	static MyPageManager			*m_pSingleton;

	MyPage							*m_CurrentPage;
	std::map<std::string,MyPage*>	m_WindowMap;
};

class MyMenuPage : public MyPage
{
public:
	MyMenuPage(const char *name);


	bool on_StartGame_clicked(const CEGUI::EventArgs& e);

};
class MyGamePlayingPage : public MyPage,Ogre::FrameListener
{
public:
	MyGamePlayingPage(const char *name);
	MyGameStateManager *StateManager;

	virtual void OnPageLoad();
	virtual void OnPageUnload();


	bool on_Restart_clicked(const CEGUI::EventArgs& e);
	bool on_Return_clicked(const CEGUI::EventArgs& e);
	bool on_Attack_clicked(const CEGUI::EventArgs& e);
	bool on_AttackCancel_clicked(const CEGUI::EventArgs& e);
	bool on_Move_clicked(const CEGUI::EventArgs& e);
	bool on_MoveCancel_clicked(const CEGUI::EventArgs& e);
	bool on_Building1_clicked(const CEGUI::EventArgs& e);
	bool on_Building2_clicked(const CEGUI::EventArgs& e);
	bool on_Building3_clicked(const CEGUI::EventArgs& e);
	bool on_BuildCancel_clicked(const CEGUI::EventArgs& e);
	bool on_EndTurn_clicked(const CEGUI::EventArgs& e);

	bool on_Card_clicked(const CEGUI::EventArgs& e);
	bool on_Card_doubleClicked(const CEGUI::EventArgs& e);

	virtual bool frameStarted(const Ogre::FrameEvent& evt);
	virtual bool keyPressed(const OIS::KeyEvent &arg);
	virtual bool keyReleased(const OIS::KeyEvent &arg);
	virtual bool mouseMoved(const OIS::MouseEvent &arg);
	virtual bool mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
	virtual bool mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id);



};
class MyGameSettingPage : public MyPage
{
public:
	MyGameSettingPage(const char *name);

	bool on_OK_clicked(const CEGUI::EventArgs& e);
	bool on_PlayerCount_ListSelectionAccepted(const CEGUI::EventArgs& e);
};
class MyGameLoadingPage : public MyPage,public Ogre::FrameListener
{
public:
	MyGameLoadingPage(const char *name);
	virtual void OnPageLoad();
	virtual void OnPageUnload();
	virtual bool frameStarted(const Ogre::FrameEvent& evt);
};