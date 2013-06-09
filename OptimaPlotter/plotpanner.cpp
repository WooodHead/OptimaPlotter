#include "stdafx.h"
#include "plotpanner.h"

#include "qevent.h"
#include "qapplication.h"

PlotPanner::PlotPanner( QWidget* parent ): QwtPlotPanner( parent )
{

}

PlotPanner::~PlotPanner()
{

}

void PlotPanner::panToCenter()
{
	if( isEnabled() )
	{
		QWidget* parentWidget = this->parentWidget();
		if( parentWidget )
		{
			QPoint pos( parentWidget->width() / 2, parentWidget->height() / 2 );
			QMouseEvent mousePressEvent( QEvent::MouseButtonPress, pos, Qt::LeftButton, 0, 0 );
			QApplication::sendEvent( parentWidget, &mousePressEvent );
		
			pos.setX( parentWidget->width() );
			pos.setY( 0 );

			QMouseEvent mouseMoveEvent( QEvent::MouseMove, pos, Qt::NoButton, Qt::NoButton, 0 );
			QApplication::sendEvent( parentWidget, &mouseMoveEvent );

			QMouseEvent mouseReleaseEvent( QEvent::MouseButtonRelease, pos, Qt::LeftButton, Qt::LeftButton, 0 );
			QApplication::sendEvent( parentWidget, &mouseReleaseEvent );
		}
	}
}