#include "stdafx.h"
#include "MyGameApp.h"
#include "MyUIUpdater.h"
#include "MyTerrain.h"
#include "MyGameStateManager.h"
#include "MyPageManager.h"
#include "MyCardManager.h"
#include "MyBuffManager.h"
#include "Global.h"
MyUIUpdater *MyUIUpdater::m_pSingleton=0;

MyUIUpdater::MyUIUpdater()
{
	if(m_pSingleton)
		throw "Error";
	else
		m_pSingleton=this;
	InitEvent();
}

void MyUIUpdater::InitEvent()
{
	
}

MyUIUpdater& MyUIUpdater::GetSingleton()
{
	return *m_pSingleton;
}

void MyUIUpdater::UpdateAreaInfo()
{
	int choose1=MyGameStateManager::GetSingleton().GetChooseID(1);
	int choose2=MyGameStateManager::GetSingleton().GetChooseID(2);
	UpdateBuffList();

	if(choose1>=0)
	{
		if(choose1==choose2)
		{
			MyArea *area=MyTerrain::GetSingleton().GetArea(choose1);
			if (area->GetAreaBelongID()==MyGameStateManager::GetSingleton().GetCurrentPlayer()->GetID())
			{
				MyPageManager::GetSingleton().GetPage("GamePlayingPage")->GetWindow()->getChild("Build")->setVisible(true);

			}
			return;
		}
		
		
		MyArea *area=MyTerrain::GetSingleton().GetArea(choose1);
		CEGUI::WindowManager &winMgr=CEGUI::WindowManager::getSingleton();
		CEGUI::Window *win=MyPageManager::GetSingleton().GetPage("GamePlayingPage")->GetWindow();
		win->getChild("AreaInfo1")->setVisible(true);

		int playerid=area->GetAreaBelongID();
		win->getChild("AreaInfo1")->setText(CEGUIText((
			QStringLiteral("区域：")+QString::number(choose1)+"\n"+
			QStringLiteral("所属玩家：")+MyGameStateManager::GetSingleton().GetPlayer(playerid)->GetName().c_str()+"("+QString::number(playerid)+")"+"\n"+
			QStringLiteral("军队数量：")+QString::number(area->GetArmyCount())+"\n"+
			QStringLiteral("地形特性：无\n")+
			QStringLiteral("放置卡牌：无\n")
			).toLocal8Bit().data()));
		

		

		if(choose2>=0)
		{
			area=MyTerrain::GetSingleton().GetArea(choose2);
			win=MyPageManager::GetSingleton().GetPage("GamePlayingPage")->GetWindow();
			win->getChild("AreaInfo2")->setVisible(true);
			playerid=area->GetAreaBelongID();
			win->getChild("AreaInfo2")->setText(CEGUIText((
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
			if (area->GetAreaBelongID()==MyGameStateManager::GetSingleton().GetCurrentPlayer()->GetID())
			{

				win->getChild("Move")->setVisible(true);
				win->getChild("Action")->setText(CEGUIText("移动\n--->\n"));
				win->getChild("Move/OK")->setEnabled(MyGameStateManager::GetSingleton().GetCurrentPlayer()->GetMoveTimes()>0);
				dynamic_cast<CEGUI::Scrollbar*>(win->getChild("Move/ScrollBar"))->setScrollPosition(0.0);
				dynamic_cast<CEGUI::Scrollbar*>(win->getChild("Move/ScrollBar"))->setScrollPosition(1.0);
			}
			else
			{
				win->getChild("Attack")->setVisible(true);
				win->getChild("Action")->setText(CEGUIText("攻击\n--->\n"));
				win->getChild("Attack/OK")->setEnabled(MyGameStateManager::GetSingleton().GetCurrentPlayer()->GetMoveTimes()>0);
				dynamic_cast<CEGUI::Scrollbar*>(win->getChild("Attack/ScrollBar"))->setScrollPosition(0.0);
				dynamic_cast<CEGUI::Scrollbar*>(win->getChild("Attack/ScrollBar"))->setScrollPosition(1.0);
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

void MyUIUpdater::UpdateCardBox()
{
	CEGUI::ScrollablePane *CardBox=dynamic_cast<CEGUI::ScrollablePane*>(MyPageManager::GetSingleton().GetPage("GamePlayingPage")->GetWindow()->getChild("CardBox"));
	CEGUI::ScrolledContainer *CardBoxContainer=const_cast<CEGUI::ScrolledContainer*>(CardBox->getContentPane());

	MyPlayer *player=MyGameStateManager::GetSingleton().GetCurrentPlayer();
	int height=MyGameApp::GetSingleton().GetRenderWindow()->getHeight();
	for (int i=CardBoxContainer->getChildCount();i<player->GetCardCount();i++)
	{
		CEGUI::String name=CEGUIText((QStringLiteral("Card")+QString::number(i)).toLocal8Bit().data());
		CEGUI::Window *card=CardBoxContainer->createChild("OgreTray/StaticImage",name);
		card->setPosition(CEGUI::UVector2(CEGUI::UDim(0,i*height*Global::CardWidthScale),CEGUI::UDim(Global::CardChooseHeight,0)));
		card->setSize(CEGUI::USize(CEGUI::UDim(0,height*Global::CardWidthScale),CEGUI::UDim(1-Global::CardChooseHeight,0)));
		card->setProperty("BackgroundEnabled","false");
		card->setProperty("FrameEnabled","false");
		card->subscribeEvent(CEGUI::Window::EventMouseClick,
			CEGUI::Event::Subscriber(&MyGamePlayingPage::on_Card_clicked,(MyGamePlayingPage*)MyPageManager::GetSingleton().GetPage("GamePlayingPage")));
		card->subscribeEvent(CEGUI::Window::EventMouseDoubleClick,
			CEGUI::Event::Subscriber(&MyGamePlayingPage::on_Card_doubleClicked,(MyGamePlayingPage*)MyPageManager::GetSingleton().GetPage("GamePlayingPage")));
		card->subscribeEvent(CEGUI::Window::EventMouseEntersArea,
			CEGUI::Event::Subscriber(&MyUIUpdater::on_MouseEnterCardArea,this));
		card->subscribeEvent(CEGUI::Window::EventMouseLeavesArea,
			CEGUI::Event::Subscriber(&MyUIUpdater::on_MouseLeaveCardArea,this));
	}
	for (int i=CardBoxContainer->getChildCount()-1;i>=player->GetCardCount();i--)
	{
		CEGUI::String name=CEGUIText((QStringLiteral("Card")+QString::number(i)).toLocal8Bit().data());
		CardBoxContainer->removeChild(name);
	}
	

	
	for (int i=0;i<player->GetCardCount();i++)
	{
		MyCard *mycard=player->GetCardByID(i);
		CEGUI::String name=CEGUIText((QStringLiteral("Card")+QString::number(i)).toLocal8Bit().data());
		CEGUI::Window *card=CardBoxContainer->getChild(name);
		card->setProperty("Image",mycard->GetCardType()->GetImageName());
		if (mycard->IsChoosed())
		{
			card->setYPosition(CEGUI::UDim(0,0));
		}
		else
		{
			card->setYPosition(CEGUI::UDim(Global::CardChooseHeight,0));
		}
	}
}

void MyUIUpdater::UpdateMoveTimes()
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
		lingpai->setPosition(CEGUI::UVector2(CEGUI::UDim(0.95-i*0.05,0),CEGUI::UDim(0.06,0)));
		lingpai->setSize(CEGUI::USize(CEGUI::UDim(0.05,0),CEGUI::UDim(0.05,0)));
		lingpai->setProperty("BackgroundEnabled","false");
		lingpai->setProperty("FrameEnabled","false");
		lingpai->setProperty("Image","lingpai/img1");
	}
	n=newn;
}

bool MyUIUpdater::on_MouseEnterCardArea( const CEGUI::EventArgs& e )
{
	CEGUI::Window *win=MyPageManager::GetSingleton().GetPage("GamePlayingPage")->GetWindow();
	win->getChild("InfoFrame")->setVisible(true);


	CEGUI::WindowEventArgs *ev=(CEGUI::WindowEventArgs*)(&e);
	CEGUI::Window *cardwin=ev->window;
	QString s=cardwin->getName().c_str();
	int id=s.remove("Card").toInt();
	MyCard *card=MyGameStateManager::GetSingleton().GetCurrentPlayer()->GetCardByID(id);
	win->getChild("InfoFrame/InfoText")->setText((CEGUI::utf8*)ANSI_to_UTF8(card->GetCardType()->GetDescription()).c_str());
	return true;
}

bool MyUIUpdater::on_MouseLeaveCardArea( const CEGUI::EventArgs& e )
{
	CEGUI::Window *win=MyPageManager::GetSingleton().GetPage("GamePlayingPage")->GetWindow();
	win->getChild("InfoFrame")->setVisible(false);
	return true;
}

void MyUIUpdater::UpdateBuffList()
{
	int choose1=MyGameStateManager::GetSingleton().GetChooseID(1);
	int choose2=MyGameStateManager::GetSingleton().GetChooseID(2);
	CEGUI::Window *win=MyPageManager::GetSingleton().GetPage("GamePlayingPage")->GetWindow();
	if(choose1>=0)
	{
		MyArea *area=MyTerrain::GetSingleton().GetArea(choose1);
		MyPlayer *player=MyGameStateManager::GetSingleton().GetPlayer(area->GetAreaBelongID());
		CEGUI::Window *buffwin;
		MyBuff *buff;
		for (int i=m_SourecBuffVector.size();i<player->GetBuffCount()+area->GetBuffCount();i++)
		{
			buffwin=win->createChild("OgreTray/StaticImage","SourceBuffWindow"+QString::number(i).toStdString());
			buffwin->setPosition(CEGUI::UVector2(CEGUI::UDim(0.035*i,0),CEGUI::UDim(0.24,0)));
			buffwin->setSize(CEGUI::USize(CEGUI::UDim(0.035,0),CEGUI::UDim(0.06,0)));
			buffwin->setProperty("FrameEnabled","false");
			buffwin->subscribeEvent(CEGUI::Window::EventMouseEntersArea,
				CEGUI::Event::Subscriber(&MyUIUpdater::on_MouseEnterSourceBuffArea,this));
			buffwin->subscribeEvent(CEGUI::Window::EventMouseLeavesArea,
				CEGUI::Event::Subscriber(&MyUIUpdater::on_MouseLeaveSourceBuffArea,this));
		}
		for (int i=player->GetBuffCount()+area->GetBuffCount();i<m_SourecBuffVector.size();i++)
		{
			win->removeChild("SourceBuffWindow"+QString::number(i).toStdString());
		}
		m_SourecBuffVector.resize(player->GetBuffCount()+area->GetBuffCount());
		for (int i=0;i<player->GetBuffCount();i++)
		{
			buffwin=win->getChild("SourceBuffWindow"+QString::number(i).toStdString());
			buff=player->GetBuffByID(i);
			buffwin->setProperty("Image",buff->GetBuffType()->GetImageName());
			m_SourecBuffVector[i]=buff;
		}
		for (int i=player->GetBuffCount();i<area->GetBuffCount();i++)
		{
			buffwin=win->getChild("SourceBuffWindow"+QString::number(i).toStdString());
			buff=area->GetBuffByID(i-player->GetBuffCount());
			buffwin->setProperty("Image",buff->GetBuffType()->GetImageName());
			m_SourecBuffVector[i]=buff;
		}
	}
	else
	{
		for (int i=0;i<m_SourecBuffVector.size();i++)
		{
			win->removeChild("SourceBuffWindow"+QString::number(i).toStdString());
		}
		m_SourecBuffVector.resize(0);
	}

	if(choose2>=0&&choose1!=choose2)
	{
		MyArea *area=MyTerrain::GetSingleton().GetArea(choose2);
		MyPlayer *player=MyGameStateManager::GetSingleton().GetPlayer(area->GetAreaBelongID());
		CEGUI::Window *buffwin;
		MyBuff *buff;
		for (int i=m_DestinationBuffVector.size();i<player->GetBuffCount()+area->GetBuffCount();i++)
		{
			buffwin=win->createChild("OgreTray/StaticImage","DestinationBuffWindow"+QString::number(i).toStdString());
			buffwin->setPosition(CEGUI::UVector2(CEGUI::UDim(0.23+0.035*i,0),CEGUI::UDim(0.24,0)));
			buffwin->setSize(CEGUI::USize(CEGUI::UDim(0.035,0),CEGUI::UDim(0.06,0)));
			buffwin->setProperty("FrameEnabled","false");
			buffwin->subscribeEvent(CEGUI::Window::EventMouseEntersArea,
				CEGUI::Event::Subscriber(&MyUIUpdater::on_MouseEnterDestinationBuffArea,this));
			buffwin->subscribeEvent(CEGUI::Window::EventMouseLeavesArea,
				CEGUI::Event::Subscriber(&MyUIUpdater::on_MouseLeaveDestinationBuffArea,this));
		}
		for (int i=player->GetBuffCount()+area->GetBuffCount();i<m_DestinationBuffVector.size();i++)
		{
			win->removeChild("DestinationBuffWindow"+QString::number(i).toStdString());
		}
		m_DestinationBuffVector.resize(player->GetBuffCount()+area->GetBuffCount());
		for (int i=0;i<player->GetBuffCount();i++)
		{
			buffwin=win->getChild("DestinationBuffWindow"+QString::number(i).toStdString());
			buff=player->GetBuffByID(i);
			buffwin->setProperty("Image",buff->GetBuffType()->GetImageName());
			m_DestinationBuffVector[i]=buff;
		}
		for (int i=player->GetBuffCount();i<area->GetBuffCount();i++)
		{
			buffwin=win->getChild("DestinationBuffWindow"+QString::number(i).toStdString());
			buff=area->GetBuffByID(i-player->GetBuffCount());
			buffwin->setProperty("Image",buff->GetBuffType()->GetImageName());
			m_DestinationBuffVector[i]=buff;
		}
	}
	else
	{
		for (int i=0;i<m_DestinationBuffVector.size();i++)
		{
			win->removeChild("DestinationBuffWindow"+QString::number(i).toStdString());
		}
		m_DestinationBuffVector.resize(0);
	}
}

bool MyUIUpdater::on_MouseEnterSourceBuffArea( const CEGUI::EventArgs& e )
{
	CEGUI::Window *win=MyPageManager::GetSingleton().GetPage("GamePlayingPage")->GetWindow();
	win->getChild("InfoFrame")->setVisible(true);


	CEGUI::WindowEventArgs *ev=(CEGUI::WindowEventArgs*)(&e);
	CEGUI::Window *cardwin=ev->window;
	QString s=cardwin->getName().c_str();
	int id=s.remove("SourceBuffWindow").toInt();
	MyBuff *buff=m_SourecBuffVector[id];
	win->getChild("InfoFrame/InfoText")->setText((CEGUI::utf8*)ANSI_to_UTF8(buff->GetBuffType()->GetDescription()).c_str());
	return true;
}

bool MyUIUpdater::on_MouseLeaveSourceBuffArea( const CEGUI::EventArgs& e )
{
	CEGUI::Window *win=MyPageManager::GetSingleton().GetPage("GamePlayingPage")->GetWindow();
	win->getChild("InfoFrame")->setVisible(false);
	return true;
}

bool MyUIUpdater::on_MouseEnterDestinationBuffArea( const CEGUI::EventArgs& e )
{
	CEGUI::Window *win=MyPageManager::GetSingleton().GetPage("GamePlayingPage")->GetWindow();
	win->getChild("InfoFrame")->setVisible(true);



	CEGUI::WindowEventArgs *ev=(CEGUI::WindowEventArgs*)(&e);
	CEGUI::Window *cardwin=ev->window;
	QString s=cardwin->getName().c_str();
	int id=s.remove("DestinationBuffWindow").toInt();
	MyBuff *buff=m_DestinationBuffVector[id];
	win->getChild("InfoFrame/InfoText")->setText((CEGUI::utf8*)ANSI_to_UTF8(buff->GetBuffType()->GetDescription()).c_str());
	return true;
}

bool MyUIUpdater::on_MouseLeaveDestinationBuffArea( const CEGUI::EventArgs& e )
{
	CEGUI::Window *win=MyPageManager::GetSingleton().GetPage("GamePlayingPage")->GetWindow();
	win->getChild("InfoFrame")->setVisible(false);
	return true;
}

void MyUIUpdater::on_PlayerChanged()
{
	UpdateCardBox();
}
