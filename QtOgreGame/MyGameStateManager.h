#pragma once
#include "stdafx.h"
class MyCard;
class MyArea;
class MyTerrain;
class MyTransformer;
class MyWindowTransformer;
class MySceneNodeTransformer;
class MyEntityTransformer;
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
protected:
	std::string m_Name;
	std::string m_Stage;
	MyGameState *m_pNextState;
	MyGameState *m_pLastState;
	MyGameState *m_pGoInState;
	bool		m_bGoNext;
	bool		m_bReturn;
public:
	MyGameState(std::string name);
	std::string GetName();
	std::string GetStage();
	void SetStage(std::string state);
	MyGameState *GetNextState();
	MyGameState *GetLastState();
	MyGameState *GetGoInState();
	void SetNextState(MyGameState *state);
	void SetLastState(MyGameState *state);
	void SetGoInState(MyGameState *state,std::string nextStage);

	void ReturnLastState();
	bool IsReturn();
	void GoNextState();
	bool IsGoNext();
	virtual void on_State_Entry();
	virtual void on_State_Exit();
	virtual bool frameStarted(const Ogre::FrameEvent& evt);
	virtual bool keyPressed(const OIS::KeyEvent &arg);
	virtual bool keyReleased(const OIS::KeyEvent &arg);
	virtual bool mouseMoved(const OIS::MouseEvent &arg);
	virtual bool mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
	virtual bool mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id);

};


class MyGameStartState:public MyGameState
{
	CEGUI::AnimationInstance	*m_pGameStartAnim;
	CEGUI::AnimationInstance	*m_pGameStartMoveAnim;
	CEGUI::Window				*m_pWindow;
public:
	MyGameStartState(std::string name);
	virtual bool frameStarted(const Ogre::FrameEvent& evt);
	virtual void on_State_Entry();
	virtual void on_State_Exit();
};


class MyGamePlayerChangeState:public MyGameState
{
protected:
	CEGUI::AnimationInstance	*m_pMoveAnim;
	CEGUI::Window				*m_pWindow;
public:
	MyGamePlayerChangeState(std::string name);
	virtual bool frameStarted(const Ogre::FrameEvent& evt);
	virtual void on_State_Entry();
	virtual void on_State_Exit();

};

class MyGamePlayState:public MyGameState
{
public:
	MyGamePlayState(std::string name);
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
	MyGameDiceState(std::string name,int diceCount);
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

class MyGameWaitingState:public MyGameState
{
protected:
	MyTransformer			*m_pTransformer;
public:
	MyGameWaitingState(std::string name,MyTransformer *transformer);
	virtual bool frameStarted(const Ogre::FrameEvent& evt);
};

class MyGameMoveState:public MyGameState
{
protected:
	MyArea					*m_pSourceArea;
	MyArea					*m_pDestinationArea;
	MySceneNodeTransformer	*m_pSceneNodeTransformer;
public:
	MyGameMoveState(std::string name,MyArea *src,MyArea *dest);
	virtual bool frameStarted(const Ogre::FrameEvent& evt);
	virtual void on_State_Entry();
	virtual	void on_State_Exit();
};

class MyGameAttackAnimationState:public MyGameState
{
protected:
	MyArea					*m_pSourceArea;
	MyArea					*m_pDestinationArea;
	MyEntityTransformer		*m_pEntityTransformer;
public:
	MyGameAttackAnimationState(std::string name,MyArea *src,MyArea *dest);
	virtual bool frameStarted(const Ogre::FrameEvent& evt);
	virtual void on_State_Entry();
	virtual	void on_State_Exit();
};

class MyGameAttackState:public MyGameState
{
protected:
	MyArea					*m_pSoureceArea;
	MyArea					*m_pDestinationArea;
	MyGameWaitingState		*m_pGameWaitingState;
	MySceneNodeTransformer	*m_pSceneNodeTransformer;
	MyEntityTransformer		*m_pEntityTransformer;
public:
	MyGameAttackState(std::string name);
	virtual bool frameStarted(const Ogre::FrameEvent& evt);
	virtual void on_State_Entry();
	virtual	void on_State_Exit();
	

	

};

class MyGameStateManager
{
	static MyGameStateManager*			m_pSingleton; 
	QDataStream							m_DataStream;
	int									m_iChoose1,m_iChoose2;
	MyPlayer							*m_pCurrentPlayer;
	MyGameState							*m_pCurrentState;
	MyGameState							*m_pNextState;
	int									m_nPlayerCount;
	std::list<MyGameState*>				m_LastStateList;
	std::map<std::string,MyGameState*>	m_RootStateMap;
	std::vector<MyPlayer*>				m_PlayerVector;
public:
	MyGameStateManager(void);
	~MyGameStateManager(void);

	static MyGameStateManager& GetSingleton();
	
	void InitMap();
	MyPlayer *GetPlayer(int id);
	MyGameState *GetCurrentState();
	MyPlayer *GetCurrentPlayer();

	MyGameState *GetRootState(std::string name);
	QDataStream	&GetDataStream();

	void AddRootState(MyGameState *state);
	void SetCurrentPlayerID(int id);

	void SetNextState(std::string name);
	int GetChooseID(int id);
	MyArea *GetChooseArea(int id);
	void ChooseArea(int id);
	void ClearChoose();

	void TurnNextPlayer();
	bool frameStarted(const Ogre::FrameEvent& evt);
	bool keyPressed(const OIS::KeyEvent &arg);
	bool keyReleased(const OIS::KeyEvent &arg);
	bool mouseMoved(const OIS::MouseEvent &arg);
	bool mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
	bool mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id);

	static void DefineInLua(lua_State *L);
	

};

