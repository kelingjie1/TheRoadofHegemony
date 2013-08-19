#pragma once
#include "stdafx.h"
class MyBuff;
class MyUIUpdater
{
protected:
	static MyUIUpdater *m_pSingleton;

	std::vector<MyBuff*> m_SourecBuffVector;
	std::vector<MyBuff*> m_DestinationBuffVector;
public:
	MyUIUpdater();

	static MyUIUpdater& GetSingleton();

	void UpdateBuffList();
	void UpdateAreaInfo();
	void UpdateCardBox();
	void UpdateMoveTimes();
	bool on_MouseEnterCardArea(const CEGUI::EventArgs& e);
	bool on_MouseLeaveCardArea(const CEGUI::EventArgs& e);
	bool on_MouseEnterSourceBuffArea(const CEGUI::EventArgs& e);
	bool on_MouseLeaveSourceBuffArea(const CEGUI::EventArgs& e);
	bool on_MouseEnterDestinationBuffArea(const CEGUI::EventArgs& e);
	bool on_MouseLeaveDestinationBuffArea(const CEGUI::EventArgs& e);
};