#include "stdafx.h"
#include "MyGameApp.h"
int main(int argc,char** argv)
{
// 	QCoreApplication::addLibraryPath("./");
// 	QApplication a(argc,argv);

	MyGameApp app;
	app.Init();
	app.Run();
}