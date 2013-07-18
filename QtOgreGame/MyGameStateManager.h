#pragma once
#include "stdafx.h"
class MyCard;
class MyArea;
class MyTerrain;
class MyPlayer
{
	friend class MyArea;
	int m_ID;
	CEGUI::String m_Name;
	int m_nArmyCount;
	int m_nGold;
	int m_MaxMoveTimes;

	int m_MoveTimes;
	int m_nAreaCount;
	std::set<MyArea*>	m_AreaSet;
	std::vector<MyCard*>	m_CardVector;
public:
	MyPlayer(int id);
	
	void AddCard(MyCard *card,bool update=true);
	void RemoveCard(MyCard *card,bool update=true);
	
	int GetAreaCount();
	int GetMoveTimes();
	int GetMaxMoveTimes();
	int GetID();
	CEGUI::String GetName();
	std::set<MyArea*> GetAreaSet();
	MyCard* GetCardByID(int id);
	int	GetCardCount();

	void SetAreaCount(int n);
	void SetMoveTimes(int n);
	void SetID(int id);
	void SetName(char *name);
	void SetMaxMoveTimes(int n);

	static void DefineInLua(lua_State *L);
};
class MyGameState
{
public:
	MyGameState(QString name);
	QString GetName();

	virtual void on_State_Entry();
	virtual void on_State_Exit();
	virtual	void on_State_Return();
	virtual bool frameStarted(const Ogre::FrameEvent& evt);
	virtual bool keyPressed(const OIS::KeyEvent &arg);
	virtual bool keyReleased(const OIS::KeyEvent &arg);
	virtual bool mouseMoved(const OIS::MouseEvent &arg);
	virtual bool mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
	virtual bool mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
protected:
	QString m_Name;
};



class MyGameStartState:public MyGameState
{
public:
	MyGameStartState(QString name);
	virtual bool frameStarted(const Ogre::FrameEvent& evt);
	virtual void on_State_Entry();
protected:
	float time;
};


class MyGamePlayerChangeState:public MyGameState
{
public:
	MyGamePlayerChangeState(QString name);
	virtual bool frameStarted(const Ogre::FrameEvent& evt);
	virtual void on_State_Entry();
	virtual void on_State_Exit();
protected:
	float time;
};

class MyGamePlayState:public MyGameState
{
public:
	MyGamePlayState(QString name);
	virtual bool mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
		
};

class MyDice
{
public:
	Critter::BodyDescription	bodyDescription;
	Critter::Body				*body;
	Ogre::Quaternion			bodyOrientation;
	Ogre::Vector3				bodyPosition;
	int							diceNum;
	Ogre::Camera				*cam;
};
class MyGameDiceState:public MyGameState
{
public:
	MyGameDiceState(QString name);
	virtual bool frameStarted(const Ogre::FrameEvent& evt);
	virtual void on_State_Entry();
	int	nDices;
	MyDice						dice[8];
protected:
	float time;
	MyArea *m_Area1,*m_Area2;
	int	m_iChoose1,m_iChoose2;

	Critter::RenderSystem		*m_pRenderSystem;
	MyTerrain					*m_pMyTerrain;
	

	
};

class MyGameAttackState:public MyGameState
{
public:
	MyGameAttackState(QString name);
	virtual bool frameStarted(const Ogre::FrameEvent& evt);
	virtual void on_State_Entry();
	virtual	void on_State_Return();
	int	m_iChoose1,m_iChoose2;
protected:
	float time;
	int returntimes;
	MyArea *m_Area1,*m_Area2;
	Ogre::Vector3 source,dest,dir; 
	int state;
	int len;
	int diceNum1,diceNum2;
	

};

class MyGameStateManager
{
	static MyGameStateManager*		m_pSingleton; 


	int m_iChoose1,m_iChoose2;
	MyPlayer *m_CurrentPlayer;
	int	m_nPlayerCount;
	MyGameState *m_CurrentState;
	std::list<MyGameState*> m_LastStateList;
	std::map<QString,MyGameState*>	m_StateMap;
	std::vector<MyPlayer*> m_PlayerVector;
public:
	MyGameStateManager(void);
	~MyGameStateManager(void);

	static MyGameStateManager& GetSingleton();
	
	void InitMap();
	MyPlayer *GetPlayer(int id);
	MyGameState *GetCurrentState();
	MyPlayer *GetCurrentPlayer();

	MyGameState *GetState(QString name);

	void SetCurrentState(QString name);
	void GoInState(QString name);
	void ReturnLastState();

	void AddState(MyGameState *state);
	void SetCurrentPlayerID(int id);

	int GetChoose(int id);
	void ChooseArea(int id);
	void ClearChoose();

	void TurnNextPlayer();

	static void DefineInLua(lua_State *L);
	

};

