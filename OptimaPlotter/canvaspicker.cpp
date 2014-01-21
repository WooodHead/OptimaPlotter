#include "stdafx.h"
#include "canvaspicker.h"
#include "knotitem.h"
#include "markeritem.h"
#include "globals.h"

#include "qapplication.h"
#include "qevent.h"
#include "qwhatsthis.h"
#include "qpainter.h"

#include "qwt_plot.h"
#include "qwt_symbol.h"
#include "qwt_scale_map.h"
#include "qwt_plot_canvas.h"
#include "qwt_plot_marker.h"
#include "qwt_plot_directpainter.h"
#include "qwt_math.h"


CanvasPicker::CanvasPicker( QwtPlot* plot ): QObject( plot ), m_isEnabled( false )
{
    QwtPlotCanvas* canvas = qobject_cast<QwtPlotCanvas*>( plot->canvas() );
    canvas->installEventFilter( this );
	m_isEnabled = true;

	m_selectedMarker = 0;
	m_selectedKnot = 0;

    canvas->setFocusPolicy( Qt::StrongFocus );
    canvas->setFocusIndicator( QwtPlotCanvas::ItemFocusIndicator );
    canvas->setFocus();
}

void CanvasPicker::setEnabled( bool enabled )
{
	if( m_isEnabled != enabled )
	{
		QwtPlotCanvas* canvas = qobject_cast<QwtPlotCanvas*>( plot()->canvas() );
		m_isEnabled = enabled;
		if( enabled )
			canvas->installEventFilter( this );
		else
			canvas->removeEventFilter( this );
	}
}

bool CanvasPicker::isEnabled() const
{
	return m_isEnabled;
}

bool CanvasPicker::event( QEvent *ev )
{
    if ( ev->type() == QEvent::User )
    {
        //showCursor( true );
        return true;
    }
    return QObject::event( ev );
}


bool CanvasPicker::eventFilter( QObject *object, QEvent *event )
{
    //if ( plot() == NULL || object != plot()->canvas() )
    //    return false;

    switch( event->type() )
    {
    //    case QEvent::FocusIn:
    //    {
    //        showCursor( true );
    //        break;
    //    }
    //    case QEvent::FocusOut:
    //    {
    //        showCursor( false );
    //        break;
    //    }
    //    case QEvent::Paint:
    //    {
    //        QApplication::postEvent( this, new QEvent( QEvent::User ) );
    //        break;
    //    }
        case QEvent::MouseButtonPress:
			{
				const QMouseEvent* mouseEvent = static_cast<QMouseEvent*>( event );
				if( mouseEvent->button() == Qt::LeftButton )
					select( mouseEvent->pos(), mouseEvent->modifiers() );
				return true;
			}
		case QEvent::MouseMove:
			{
				const QMouseEvent* mouseEvent = static_cast<QMouseEvent*>( event );
				move( mouseEvent->pos() );
				return true;
			}
		case QEvent::MouseButtonRelease:
			{
				release();
				return true;
			}
		case QEvent::Paint:
			{
				QApplication::postEvent( this, new QEvent( QEvent::User ) );
				break;
			}
		default:
			break;
    }

    return QObject::eventFilter( object, event );
}

void CanvasPicker::select( const QPoint &pos, Qt::KeyboardModifiers modifiers )
{
	m_selectedMarker = 0;
	m_selectedKnot = 0;

    double minDistanceMarkers = 10e10;
	double minDistanceKnots = 10e10;
	MarkerItem* markerWithMinDistance = 0;
	KnotItem* knotWithMinDistance = 0;
    //int index = -1;

	const QwtScaleMap xMap = plot()->canvasMap( QwtPlot::xBottom );
    const QwtScaleMap yMap = plot()->canvasMap( QwtPlot::yLeft );

    const QwtPlotItemList& itemList = plot()->itemList();
    for ( QwtPlotItemIterator it = itemList.begin(); it != itemList.end(); ++it )
    {
        if ( ( *it )->rtti() == Globals::Rtti_PlotMarker )
        {
            MarkerItem* marker = static_cast<MarkerItem*>( *it );

			const double deltaX = xMap.transform( marker->xValue() ) - pos.x();
			const double deltaY = yMap.transform( marker->yValue() ) - pos.y();
			const double distance = qSqrt( qwtSqr( deltaX ) + qwtSqr( deltaY ) );
            if ( distance < minDistanceMarkers )
			{
				minDistanceMarkers = distance;
				markerWithMinDistance = marker;
			}
		}
		else if ( ( *it )->rtti() == Globals::Rtti_PlotKnot )
		{
			KnotItem* knot = static_cast<KnotItem*>( *it );

			const double distance = qAbs( xMap.transform( knot->coordinate() ) - pos.x() );

			if( distance < minDistanceKnots )
			{
				minDistanceKnots = distance;
				knotWithMinDistance = knot;
			}
		}
    }

	// Give a priority to the markers
	if( minDistanceMarkers < Globals::SELECTION_PIXEL_TOLERANCE && markerWithMinDistance != 0 )
	{
		m_selectedMarker = markerWithMinDistance;
		emit picked( modifiers, m_selectedMarker );
		return;
	}

	if( minDistanceKnots < Globals::SELECTION_PIXEL_TOLERANCE && knotWithMinDistance != 0 )
	{
		m_selectedKnot = knotWithMinDistance;
		emit picked( modifiers, m_selectedKnot );
		return;
	}

	emit picked( modifiers, 0 );
}

void CanvasPicker::move( const QPoint &pos )
{
	if( !m_selectedMarker && !m_selectedKnot )
		return;

	if ( m_selectedMarker )
	{
		m_selectedMarker->setValue( plot()->invTransform( QwtPlot::xBottom, pos.x() ), 
			plot()->invTransform( QwtPlot::yLeft, pos.y() ) );
	}
	else if( m_selectedKnot )
	{
		m_selectedKnot->setCoordinate( plot()->invTransform( QwtPlot::xBottom, pos.x() ) );
	}

	QwtPlotCanvas *plotCanvas = 
		qobject_cast<QwtPlotCanvas *>( plot()->canvas() );

	plotCanvas->setPaintAttribute( QwtPlotCanvas::ImmediatePaint, true );
	plot()->replot();
	plotCanvas->setPaintAttribute( QwtPlotCanvas::ImmediatePaint, false );
}

void CanvasPicker::release()
{
	m_selectedMarker = 0;
	m_selectedKnot = 0;
}

QwtPlot* CanvasPicker::plot()
{
    return qobject_cast<QwtPlot *>( parent() );
}

const QwtPlot* CanvasPicker::plot() const
{
    return qobject_cast<const QwtPlot *>( parent() );
}