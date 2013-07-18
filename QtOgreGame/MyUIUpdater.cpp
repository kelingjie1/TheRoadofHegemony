#include "stdafx.h"
#include "MyGameApp.h"
#include "MyUIUpdater.h"
#include "MyTerrain.h"
#include "MyGameStateManager.h"
#include "MyPageManager.h"
#include "MyCardManager.h"
#include "Global.h"
MyUIUpdater *MyUIUpdater::m_pSingleton=0;

MyUIUpdater& MyUIUpdater::GetSingleton()
{
	return *m_pSingleton;
}

void MyUIUpdater::on_AreaChoose()
{
	int choose1=MyGameStateManager::GetSingleton().GetChoose(1);
	int choose2=MyGameStateManager::GetSingleton().GetChoose(2);
	

	if(choose1>=0)
	{
		if(choose1==choose2)
		{
			MyArea *area=MyTerrain::GetSingleton().GetArea(choose1);
			if (area->GetAreaBelong()==MyGameStateManager::GetSingleton().GetCurrentPlayer()->GetID())
			{
				MyPageManager::GetSingleton().GetPage("GamePlayingPage")->GetWindow()->getChild("Build")->setVisible(true);

			}
			return;
		}
		
		
		MyArea *area=MyTerrain::GetSingleton().GetArea(choose1);
		CEGUI::WindowManager &winMgr=CEGUI::WindowManager::getSingleton();
		CEGUI::Window *win=MyPageManager::GetSingleton().GetPage("GamePlayingPage")->GetWindow()->getChild("AreaInfo1");
		win->setVisible(true);
		int playerid=area->GetAreaBelong();
		win->setText(CEGUIText((
			QStringLiteral("区域：")+QString::number(choose1)+"\n"+
			QStringLiteral("所属玩家：")+MyGameStateManager::GetSingleton().GetPlayer(playerid)->GetName().c_str()+"("+QString::number(playerid)+")"+"\n"+
			QStringLiteral("军队数量：")+QString::number(area->GetArmyCount())+"\n"+
			QStringLiteral("地形特性：无\n")+
			QStringLiteral("放置卡牌：无\n")
			).toLocal8Bit().data()));
		if(choose2>=0)
		{
			area=MyTerrain::GetSingleton().GetArea(choose2);
			win=MyPageManager::GetSingleton().GetPage("GamePlayingPage")->GetWindow()->getChild("AreaInfo2");
			win->setVisible(true);
			playerid=area->GetAreaBelong();
			win->setText(CEGUIText((
				QStringLiteral("区域：")+QString::number(choose2)+"\n"+
				QStringLiteral("所属玩家：")+MyGameStateManager::GetSingleton().GetPlayer(playerid)->GetName().c_str()+"("+QString::number(playerid)+")"+"\n"+
				QStringLiteral("军队数量：")+QString::number(area->GetArmyCount())+"\n"+
				QStringLiteral("地形特性：无")+"\n"+
				QStringLiteral("放置卡牌：无")+"\n"
				).toLocal8Bit().data()));
			win=MyPageManager::GetSingleton().GetPage("GamePlayingPage")->GetWindow()->getChild("Action");
			win->setVisible(true);

			win=MyPageManager::GetSingleton().GetPage("GamePlayingPage")->GetWindow();
			MyGameApp::GetSingleton().GetMyTerrain()->SetAreaHighLight(choose2,true);
			MyArea *area=MyTerrain::GetSingleton().GetArea(choose2);
			if (area->GetAreaBelong()==MyGameStateManager::GetSingleton().GetCurrentPlayer()->GetID())
			{

				win->getChild("Move")->setVisible(true);
				win->getChild("Action")->setText(CEGUIText("移动\n--->\n"));
				win->getChild("Move/OK")->setEnabled(MyGameStateManager::GetSingleton().GetCurrentPlayer()->GetMoveTimes()>0);
			}
			else
			{
				win->getChild("Attack")->setVisible(true);
				win->getChild("Action")->setText(CEGUIText("攻击\n--->\n"));
				win->getChild("Attack/OK")->setEnabled(MyGameStateManager::GetSingleton().GetCurrentPlayer()->GetMoveTimes()>0);
			}
		}

	}
	else
	{
		CEGUI::WindowManager &winMgr=CEGUI::WindowManager::getSingleton();
		CEGUI::Window *win=MyPageManager::GetSingleton().GetPage("GamePlayingPage")->GetWindow()->getChild("AreaInfo1");
		win->setVisible(false);
		win=MyPageManager::GetSingleton().GetPage("GamePlayingPage")->GetWindow()->getChild("AreaInfo2");
		win->setVisible(false);
		win=MyPageManager::GetSingleton().GetPage("GamePlayingPage")->GetWindow()->getChild("Action");
		win->setVisible(false);
	}
}

void MyUIUpdater::on_CardChange()
{
	CEGUI::ScrollablePane *CardBox=dynamic_cast<CEGUI::ScrollablePane*>(MyPageManager::GetSingleton().GetPage("GamePlayingPage")->GetWindow()->getChild("CardBox"));
	CEGUI::ScrolledContainer *CardBoxContainer=const_cast<CEGUI::ScrolledContainer*>(CardBox->getContentPane());

	MyPlayer *player=MyGameStateManager::GetSingleton().GetCurrentPlayer();
	std::list<MyCard*> cardlist=player->GetCardList();
	int height=MyGameApp::GetSingleton().GetRenderWindow()->getHeight();
	for (int i=CardBoxContainer->getChildCount();i<cardlist.size();i++)
	{
		CEGUI::String name=CEGUIText((QStringLiteral("Card")+QString::number(i)).toLocal8Bit().data());
		CEGUI::Window *card=CardBoxContainer->createChild("OgreTray/StaticImage",name);
		card->setPosition(CEGUI::UVector2(CEGUI::UDim(0,i*height*Global::CardWidthScale),CEGUI::UDim(0,0)));
		card->setSize(CEGUI::USize(CEGUI::UDim(0,height*Global::CardWidthScale),CEGUI::UDim(1,0)));
		card->setProperty("BackgroundEnabled","false");
		card->setProperty("FrameEnabled","false");
	}
	for (int i=CardBoxContainer->getChildCount()-1;i>=(int)cardlist.size();i--)
	{
		CEGUI::String name=CEGUIText((QStringLiteral("Card")+QString::number(i)).toLocal8Bit().data());
		CardBoxContainer->removeChild(name);
	}
	

	
	int i=0;
	for (std::list<MyCard*>::iterator it=cardlist.begin();it!=cardlist.end();it++)
	{
		CEGUI::String name=CEGUIText((QStringLiteral("Card")+QString::number(i)).toLocal8Bit().data());
		CEGUI::Window *card=CardBoxContainer->getChild(name);
		card->setProperty("Image",(*it)->GetCardType()->GetImageName());
		i++;
	}
}

void MyUIUpdater::on_MoveTimesChange()
{
	static int n=0;
	int newn=MyGameStateManager::GetSingleton().GetCurrentPlayer()->GetMoveTimes();
	CEGUI::Window *win=MyPageManager::GetSingleton().GetPage("GamePlayingPage")->GetWindow();
	for (int i=newn;i<n;i++)
	{
		CEGUI::String name=CEGUIText((QStringLiteral("lingpai")+QString::number(i)).toLocal8Bit().data());
		win->removeChild(name);
	}
	for (int i=n;i<newn;i++)
	{
		CEGUI::String name=CEGUIText((QStringLiteral("lingpai")+QString::number(i)).toLocal8Bit().data());
		CEGUI::Window *lingpai=win->createChild("OgreTray/StaticImage",name);
		lingpai->setPosition(CEGUI::UVector2(CEGUI::UDim(0.95-i*0.05,0),CEGUI::UDim(0.01,0)));
		lingpai->setSize(CEGUI::USize(CEGUI::UDim(0.05,0),CEGUI::UDim(0.1,0)));
		lingpai->setProperty("BackgroundEnabled","false");
		lingpai->setProperty("FrameEnabled","false");
		lingpai->setProperty("Image","lingpai/img1");
	}
	n=newn;
}
