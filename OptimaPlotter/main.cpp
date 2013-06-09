#include "stdafx.h"
#include "optimaplotter.h"

#include <QtGui/QApplication>
#include "qtimer.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	OptimaPlotter w;
	w.show();
	QTimer::singleShot(0, &w, SLOT( onEventLoopStarted() ) );
	return a.exec();
}
