#pragma once
#include "stdafx.h"
class MyUIUpdater
{
	static MyUIUpdater *m_pSingleton;
public:
	static MyUIUpdater& GetSingleton();


	void on_AreaChoose();
	void on_CardChange();
	void on_MoveTimesChange();
};