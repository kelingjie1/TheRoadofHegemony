#pragma once
#include "stdafx.h"
class MyUIUpdater
{
	static MyUIUpdater *m_pSingleton;
public:
	MyUIUpdater();

	static MyUIUpdater& GetSingleton();

	void on_AreaChoose();
	void on_CardChange();
	void on_MoveTimesChange();
};