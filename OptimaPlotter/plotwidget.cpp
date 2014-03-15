#include "stdafx.h"
#include "plotwidget.h"
#include "plotpanner.h"
#include "plotmagnifier.h"
#include "knotpicker.h"
#include "canvaspicker.h"
#include "knotitem.h"
#include "markeritem.h"
#include "markerpickerclickpointmachine.h"
#include "globals.h"

#include "qwt_plot_curve.h"
#include "qwt_plot_panner.h"
#include "qwt_plot_magnifier.h"
#include "qwt_plot_picker.h"
#include "qwt_picker_machine.h"
#include "qwt_plot_curve.h"
#include "qwt_symbol.h"
#include "qwt_plot_grid.h"
#include "qwt_scale_div.h"

#include <limits>
#include <qpen.h>
#include <qdebug.h>

PlotWidget::PlotWidget( QWidget* parent ): QwtPlot( parent )
{
	//setTitle( tr( "Function plot" ) );

	m_markerPickerClickPointMachine = new MarkerPickerClickPointMachine();
	m_plotPicker = new QwtPlotPicker( QwtPlot::xBottom, QwtPlot::yLeft,
		QwtPicker::CrossRubberBand, QwtPicker::AlwaysOn, canvas() );
	m_plotPicker->setStateMachine( m_markerPickerClickPointMachine );
	m_plotPicker->setEnabled( false );

	m_plotPanner = new PlotPanner( canvas() );
	m_plotPanner->setEnabled( false );

	m_plotMagnifier = new PlotMagnifier( canvas() );
	m_plotMagnifier->setEnabled( false );

	m_canvasPicker = 0;
	m_knotPicker = 0;

	m_plotCurve = new QwtPlotCurve();
	m_plotCurve->setPen( Qt::blue, 2.0, Qt::SolidLine );
	m_plotCurve->attach( this );

	m_plotGrid = new QwtPlotGrid;
    m_plotGrid->setMajorPen( Qt::black, 1, Qt::DotLine );
    m_plotGrid->attach( this );

	setAxisScale( yLeft, -50, 50 );
	setAxisScale( xBottom, -50, 50 );

	connect( m_plotPicker, SIGNAL( selected( const QPointF& ) ),
		this, SIGNAL( pointPicked( const QPointF& ) ) );
}

PlotWidget::~PlotWidget()
{

}

void PlotWidget::setPannerEnabled( bool enabled )
{
	m_plotPanner->setEnabled( enabled );
}

bool PlotWidget::isPannerEnabled() const
{
	return m_plotPanner->isEnabled();
}

void PlotWidget::setPickerEnabled( bool enabled )
{
	m_plotPicker->setEnabled( enabled );
}

bool PlotWidget::isPickerEnabled() const
{
	return m_plotPicker->isEnabled();
}

void PlotWidget::setMagnifierEnabled( bool enabled )
{
	m_plotMagnifier->setEnabled( enabled );
}

bool PlotWidget::isMagnifierEnabled() const
{
	return m_plotMagnifier->isEnabled();
}

void PlotWidget::setKnotPicker( KnotPicker* knotPicker )
{
	if( m_knotPicker == 0 && knotPicker != 0 )
	{
		m_knotPicker = knotPicker;
		m_knotPicker->setEnabled( false );
		
		connect( m_knotPicker, SIGNAL( clicked( double ) ),
			this, SIGNAL( knotPicked( double ) ) );
	}
}

void PlotWidget::setCanvasPicker( CanvasPicker* canvasPicker )
{
	if( m_canvasPicker == 0 && canvasPicker != 0 )
	{
		m_canvasPicker = canvasPicker;
		m_canvasPicker->setEnabled( false );

		connect( m_canvasPicker, SIGNAL( picked( Qt::KeyboardModifiers, QwtPlotItem* ) ),
			this, SLOT( onPicked( Qt::KeyboardModifiers, QwtPlotItem* ) ) );

		connect( m_canvasPicker, SIGNAL( dataChanged( QwtPlotItem* ) ), this,
			SIGNAL( dataChanged( QwtPlotItem* ) ) );
	}
}

void PlotWidget::setKnotPickerEnabled( bool enabled )
{
	m_knotPicker->setEnabled( enabled );
}

bool PlotWidget::isKnotPickerEnabled() const
{
	return m_knotPicker->isEnabled();
}

void PlotWidget::setCanvasPickerEnabled( bool enabled )
{
	m_canvasPicker->setEnabled( enabled );
}

bool PlotWidget::isCanvasPickerEnabled() const
{
	return m_canvasPicker->isEnabled();
}

void PlotWidget::setRangeEnabled( bool enabled )
{
	const QwtPlotItemList& knotList = itemList( Globals::Rtti_PlotKnot );
	for ( QwtPlotItemIterator it = knotList.begin(); it != knotList.end(); ++it )
	{
		KnotItem* knot = dynamic_cast<KnotItem*>( *it );
		if( knot->isRangeBorder() )
			knot->setEnabled( enabled );
	}
}

void PlotWidget::setKnotsEnabled( bool enabled )
{
	const QwtPlotItemList& knotList = itemList( Globals::Rtti_PlotKnot );
	for ( QwtPlotItemIterator it = knotList.begin(); it != knotList.end(); ++it )
	{
		KnotItem* knot = dynamic_cast<KnotItem*>( *it );
		if( !knot->isRangeBorder() )
			knot->setEnabled( enabled );
	}
}

void PlotWidget::setKnotsEditable( bool enabled )
{
	const QwtPlotItemList& knotList = itemList( Globals::Rtti_PlotKnot );
	for ( QwtPlotItemIterator it = knotList.begin(); it != knotList.end(); ++it )
	{
		KnotItem* knot = dynamic_cast<KnotItem*>( *it );
		if( !knot->isRangeBorder() )
			knot->setEditAllowed( enabled );
	}
}

void PlotWidget::setCurveSamples( const QVector<QPointF>& samples )
{
	m_plotCurve->setSamples( samples );
}

void PlotWidget::insertMarker( const QPointF& point )
{
	const QwtPlotItemList& plotItemList = this->itemList();
	for ( QwtPlotItemIterator it = plotItemList.begin(); it != plotItemList.end(); ++it )
	{
		if( (*it)->rtti() == Globals::Rtti_PlotMarker )
		{
			const MarkerItem* plotMarker = dynamic_cast<const MarkerItem*>( *it );
			if( plotMarker->value() == point )
			{
				return;
			}
		}
	}

	MarkerItem* plotMarker = new MarkerItem();
	plotMarker->setValue( point );
	plotMarker->attach( this );

	emit itemAdded( plotMarker );
}

void PlotWidget::insertKnot( double coordinate )
{
	/*QwtPlotCurve* knotCurve = new QwtPlotCurve();
	knotCurve->setPen( Qt::cyan, 3.0, Qt::SolidLine );
	knotCurve->attach( this );

	QVector<QPointF> knotSamples;
	double val = 1.0e+30;
	knotSamples << QPointF( coordinate, -val ) <<  QPointF( coordinate, val );
	knotCurve->setSamples( knotSamples );*/
	KnotItem* knot = new KnotItem();
	knot->setCoordinate( coordinate );
	knot->attach( this );

	emit itemAdded( knot );
}

void PlotWidget::markerPointsVector( QVector<QPointF>& points ) const
{
	const QwtPlotItemList& plotItemList = this->itemList( Globals::Rtti_PlotMarker );
	for ( QwtPlotItemIterator it = plotItemList.begin(); it != plotItemList.end(); ++it )
	{
		const MarkerItem* plotMarker = dynamic_cast<const MarkerItem*>( *it );
		points.append( plotMarker->value() );
	}
}

void PlotWidget::knotsVector( QVector<double>& knots ) const
{
	const QwtPlotItemList& knotList = this->itemList( Globals::Rtti_PlotKnot );
	for ( QwtPlotItemIterator it = knotList.begin(); it != knotList.end(); ++it )
	{
		const KnotItem* knot = dynamic_cast<const KnotItem*>( *it );
		if( !knot->isRangeBorder() )
			knots.append( knot->coordinate() );
	}
}

void PlotWidget::rangeBorders( double& left, double& right ) const
{
	QList<double> borders;

	const QwtPlotItemList& knotList = this->itemList( Globals::Rtti_PlotKnot );
	for ( QwtPlotItemIterator it = knotList.begin(); it != knotList.end(); ++it )
	{
		const KnotItem* knot = dynamic_cast<const KnotItem*>( *it );
		if( knot->isRangeBorder() )
			borders.append( knot->coordinate() );
	}

	qSort( borders.begin(), borders.end() );
	left = borders.first();
	right = borders.last();
}

void PlotWidget::panToCenter()
{
	m_plotPanner->panToCenter();
	m_plotMagnifier->rescale( 0.2 );
}

void PlotWidget::onPicked( Qt::KeyboardModifiers modifiers, QwtPlotItem* plotItem )
{
	QList<QwtPlotItem*> selectedItems;
	QList<QwtPlotItem*> deselectedItems;

	if( plotItem == 0 )
	{
		foreach( QwtPlotItem* item, m_listOfSelectedKnots )
		{
			KnotItem* knotItem = dynamic_cast<KnotItem*>( item );
			knotItem->deselect();
		}

		deselectedItems.append( m_listOfSelectedKnots );
		m_listOfSelectedKnots.clear();

		emit selectionChanged( selectedItems, deselectedItems );

		foreach( QwtPlotItem* item, m_listOfSelectedMarkers )
		{
			MarkerItem* markerItem = dynamic_cast<MarkerItem*>( item );
			markerItem->deselect();
		}

		deselectedItems.clear();
		deselectedItems.append( m_listOfSelectedMarkers );
		m_listOfSelectedMarkers.clear();

		emit selectionChanged( selectedItems, deselectedItems );

		return;
	}

	if( modifiers & Qt::ControlModifier )
	{
		switch( plotItem->rtti() )
		{
		case Globals::Rtti_PlotKnot:
			{
				KnotItem* knotItem = dynamic_cast<KnotItem*>( plotItem );
				knotItem->setSelectionState( !knotItem->isSelected() );

				knotItem->isSelected() ? m_listOfSelectedKnots.append( knotItem ) 
					: m_listOfSelectedKnots.removeAll( knotItem );

				knotItem->isSelected() ? selectedItems.append( knotItem ) : deselectedItems.append( knotItem );
				emit selectionChanged( selectedItems, deselectedItems );
				break;
			}
		case Globals::Rtti_PlotMarker:
			{
				MarkerItem* markerItem = dynamic_cast<MarkerItem*>( plotItem );
				markerItem->setSelectionState( !markerItem->isSelected() );

				markerItem->isSelected() ? m_listOfSelectedMarkers.append( markerItem ) 
					: m_listOfSelectedMarkers.removeAll( markerItem );

				markerItem->isSelected() ? selectedItems.append( markerItem ) : deselectedItems.append( markerItem );
				emit selectionChanged( selectedItems, deselectedItems );
				break;
			}
		default:
			break;
		}
	}
	else
	{
		switch( plotItem->rtti() )
		{
		case Globals::Rtti_PlotKnot:
			{
				foreach( QwtPlotItem* item, m_listOfSelectedKnots )
				{
					KnotItem* knotItem = dynamic_cast<KnotItem*>( item );
					knotItem->deselect();
				}

				deselectedItems.append( m_listOfSelectedKnots );
				m_listOfSelectedKnots.clear();

				KnotItem* knotItem = dynamic_cast<KnotItem*>( plotItem );
				knotItem->select();

				selectedItems.append( knotItem );
				m_listOfSelectedKnots.append( knotItem );

				emit selectionChanged( selectedItems, deselectedItems );
				break;
			}
		case Globals::Rtti_PlotMarker:
			{
				foreach( QwtPlotItem* item, m_listOfSelectedMarkers )
				{
					MarkerItem* markerItem = dynamic_cast<MarkerItem*>( item );
					markerItem->deselect();
				}

				deselectedItems.append( m_listOfSelectedMarkers );
				m_listOfSelectedMarkers.clear();

				MarkerItem* markerItem = dynamic_cast<MarkerItem*>( plotItem );
				markerItem->select();

				selectedItems.append( markerItem );
				m_listOfSelectedMarkers.append( markerItem );

				emit selectionChanged( selectedItems, deselectedItems );
				break;
			}
		default:
			break;
		}
	}
}

QList<QwtPlotItem*>& PlotWidget::listOfSelectedItems( int itemType )
{
	static QList<QwtPlotItem*> dummyList;

	switch( itemType )
	{
	case::Globals::Rtti_PlotKnot:
		return m_listOfSelectedKnots;
	case::Globals::Rtti_PlotMarker:
		return m_listOfSelectedMarkers;
	default:
		return dummyList;
	}
}

void PlotWidget::onItemAdded( QwtPlotItem* plotItem )
{
	plotItem->attach( this );
	replot();
}

void PlotWidget::onSelectionChanged( QList<QwtPlotItem*>& selectedItems, QList<QwtPlotItem*>& deselectedItems )
{
	int itemType;
	if( selectedItems.count() != 0 )
		itemType = selectedItems.first()->rtti();
	else if( deselectedItems.count() != 0 )
		itemType = deselectedItems.first()->rtti();
	else
		return;

	switch( itemType )
	{
	case::Globals::Rtti_PlotKnot:
		{
			foreach( QwtPlotItem* deselectedItem, deselectedItems )
			{
				KnotItem* knotItem = dynamic_cast<KnotItem*>( deselectedItem );
				knotItem->setSelectionState( false );
				m_listOfSelectedKnots.removeAll( knotItem );
			}

			foreach( QwtPlotItem* selectedItem, selectedItems )
			{
				KnotItem* knotItem = dynamic_cast<KnotItem*>( selectedItem );
				knotItem->setSelectionState( true );
				m_listOfSelectedKnots.append( knotItem );
			}
			return;
		}
	case::Globals::Rtti_PlotMarker:
		{
			foreach( QwtPlotItem* deselectedItem, deselectedItems )
			{
				MarkerItem* markerItem = dynamic_cast<MarkerItem*>( deselectedItem );
				markerItem->setSelectionState( false );
				m_listOfSelectedMarkers.removeAll( markerItem );
			}

			foreach( QwtPlotItem* selectedItem, selectedItems )
			{
				MarkerItem* markerItem = dynamic_cast<MarkerItem*>( selectedItem );
				markerItem->setSelectionState( true );
				m_listOfSelectedMarkers.append( markerItem );
			}
			return;
		}
	}
}

void PlotWidget::onDeleteSelectedMarkers()
{
	m_plotCurve->setSamples( QVector<QPointF>() );
	foreach( QwtPlotItem* item, m_listOfSelectedMarkers )
	{
		item->detach();
		delete item;
	}

	m_listOfSelectedMarkers.clear();

	replot();
}

void PlotWidget::onDeleteSelectedKnots()
{
	//Do not delete the knots of type "range border", thus keep a separate list ot them
	QList<QwtPlotItem*> borders;

	m_plotCurve->setSamples( QVector<QPointF>() );
	foreach( QwtPlotItem* item, m_listOfSelectedKnots )
	{
		KnotItem* knotItem = dynamic_cast<KnotItem*>( item );

		if( knotItem->isRangeBorder() )
			borders.append( knotItem );
		else
		{
			knotItem->detach();
			delete knotItem;
		}
	}

	m_listOfSelectedKnots.clear();
	m_listOfSelectedKnots.append( borders );

	replot();
}

void PlotWidget::boundingXCoordinates( double& x1, double& x2 ) const
{
	QwtScaleDiv scaleDiv = axisScaleDiv( xBottom );
	x1 = scaleDiv.lowerBound();
	x2 = scaleDiv.upperBound();
}