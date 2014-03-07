#include "stdafx.h"
#include "canvaspicker.h"
#include "knotitem.h"
#include "markeritem.h"
#include "globals.h"
#include "plotwidget.h"

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


CanvasPicker::CanvasPicker( QwtPlot* plot ): QObject( plot ), m_isEnabled( false ), m_dragAndDropInProgress( false ),
	m_typeOfItemsToDrag( -1 ), m_itemToPick( 0 )
{
    QwtPlotCanvas* canvas = qobject_cast<QwtPlotCanvas*>( plot->canvas() );
    canvas->installEventFilter( this );
	m_isEnabled = true;

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
				const QMouseEvent* mouseEvent = static_cast<QMouseEvent*>( event );
				if( mouseEvent->button() == Qt::LeftButton )
					release( mouseEvent->pos(), mouseEvent->modifiers() );
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
	m_selectionPoint = pos;
	m_previousPoint = pos;

    double minDistanceMarkers = 10e10;
	double minDistanceKnots = 10e10;

	MarkerItem* markerWithMinDistance = 0;
	KnotItem* knotWithMinDistance = 0;

	int selectionType = -1;

	PlotWidget* plotWidget = dynamic_cast<PlotWidget*>( plot() );

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
		m_itemToPick = markerWithMinDistance;
		selectionType = Globals::Rtti_PlotMarker;
	}
	else if(  minDistanceKnots < Globals::SELECTION_PIXEL_TOLERANCE && knotWithMinDistance != 0 )
	{
		m_itemToPick = knotWithMinDistance;
		selectionType = Globals::Rtti_PlotKnot;
	}

	if( selectionType == -1 )
	{
		emit picked( modifiers, 0 );
		return;
	}

	QList<QwtPlotItem*>& listOfSelectedItems = plotWidget->listOfSelectedItems( selectionType );

	if( listOfSelectedItems.count() == 0 )
	{
		// Select and allow the user to drag and drop.
		emit picked( modifiers, m_itemToPick );

		m_typeOfItemsToDrag = selectionType;
		m_dragAndDropInProgress = true;
		m_itemToPick = 0;
	}
	else
	{
		if( listOfSelectedItems.contains( m_itemToPick ) )
		{
			// We don't know yet whether the user wants to do a selection or a drag and drop operation.
			// The picking operation will be detected in CanvasPicker::release().

			m_typeOfItemsToDrag = selectionType;
			m_dragAndDropInProgress = true;
		}
		else
		{
			emit picked( modifiers, m_itemToPick );
			m_itemToPick = 0;

			if( listOfSelectedItems.count() == 1 )
			{
				m_typeOfItemsToDrag = selectionType;
				m_dragAndDropInProgress = true;
			}
		}
	}
}

void CanvasPicker::move( const QPoint &pos )
{
	if( !m_dragAndDropInProgress )
		return;

	PlotWidget* plotWidget = dynamic_cast<PlotWidget*>( plot() );

	switch( m_typeOfItemsToDrag )
	{
	case Globals::Rtti_PlotMarker:
		{
			const double dX = plot()->invTransform( QwtPlot::xBottom, pos.x() ) - plot()->invTransform( QwtPlot::xBottom, m_previousPoint.x() );
			const double dY = plot()->invTransform( QwtPlot::yLeft, pos.y() ) - plot()->invTransform( QwtPlot::yLeft, m_previousPoint.y() );

			QList<QwtPlotItem*>& listOfSelectedMarkers = plotWidget->listOfSelectedItems( Globals::Rtti_PlotMarker );

			foreach( QwtPlotItem* item, listOfSelectedMarkers )
			{
				MarkerItem* markerItem = dynamic_cast<MarkerItem*>( item );
				markerItem->setValue(  markerItem->xValue() + dX, markerItem->yValue() + dY );
				emit dataChanged( markerItem );
			}

			break;
		}
	case Globals::Rtti_PlotKnot:
		{
			const double dX = plot()->invTransform( QwtPlot::xBottom, pos.x() ) - plot()->invTransform( QwtPlot::xBottom, m_previousPoint.x() );

			QList<QwtPlotItem*>& listOfSelectedKnots = plotWidget->listOfSelectedItems( Globals::Rtti_PlotKnot );

			foreach( QwtPlotItem* item, listOfSelectedKnots )
			{
				KnotItem* knotItem = dynamic_cast<KnotItem*>( item );
				if( knotItem->isEditAllowed() )
				{
					knotItem->setCoordinate( knotItem->coordinate() + dX );
					emit dataChanged( knotItem );
				}
			}

			break;
		}
	default:
		{
			m_dragAndDropInProgress = false;
			m_typeOfItemsToDrag = -1;
			return;
		}
	}

	m_previousPoint = pos;

	QwtPlotCanvas *plotCanvas = 
		qobject_cast<QwtPlotCanvas *>( plot()->canvas() );

	plotCanvas->setPaintAttribute( QwtPlotCanvas::ImmediatePaint, true );
	plot()->replot();
	plotCanvas->setPaintAttribute( QwtPlotCanvas::ImmediatePaint, false );
}

void CanvasPicker::release( const QPoint& pos, Qt::KeyboardModifiers modifiers )
{
	if( m_selectionPoint == pos && m_itemToPick != 0 )
	{
		emit picked( modifiers, m_itemToPick );
	}

	m_dragAndDropInProgress = false;
	m_typeOfItemsToDrag = -1;
	m_itemToPick = 0;
}

QwtPlot* CanvasPicker::plot()
{
    return qobject_cast<QwtPlot *>( parent() );
}

const QwtPlot* CanvasPicker::plot() const
{
    return qobject_cast<const QwtPlot *>( parent() );
}