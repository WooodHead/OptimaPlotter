#include "stdafx.h"
#include "plotwidget.h"
#include "plotpanner.h"
#include "plotmagnifier.h"
#include "knotpicker.h"
#include "knotitem.h"
#include "globals.h"

#include "qwt_plot_curve.h"
#include "qwt_plot_panner.h"
#include "qwt_plot_magnifier.h"
#include "qwt_plot_picker.h"
#include "qwt_picker_machine.h"
#include "qwt_plot_curve.h"
#include "qwt_plot_marker.h"
#include "qwt_symbol.h"
#include "qwt_plot_grid.h"

#include <limits>
#include <qpen.h>
#include <qdebug.h>

PlotWidget::PlotWidget( QWidget* parent ): QwtPlot( parent )
{
	//setTitle( tr( "Function plot" ) );

	m_pickerClickPointMachine = new QwtPickerClickPointMachine();
	m_plotPicker = new QwtPlotPicker( QwtPlot::xBottom, QwtPlot::yLeft,
		QwtPicker::CrossRubberBand, QwtPicker::AlwaysOn, canvas() );
	m_plotPicker->setStateMachine( m_pickerClickPointMachine );

	m_plotPanner = new PlotPanner( canvas() );
	m_plotMagnifier = new PlotMagnifier( canvas() );
	m_knotPicker = 0;

	m_plotCurve = new QwtPlotCurve();
	m_plotCurve->setPen( Qt::blue, 2.0, Qt::SolidLine );
	m_plotCurve->attach( this );

	m_plotGrid = new QwtPlotGrid;
    m_plotGrid->setMajorPen( Qt::black, 1, Qt::DotLine );
    m_plotGrid->attach( this );

	setAxisScale( yLeft, -20, 20 );
	setAxisScale( xBottom, -20, 20 );

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
		
		connect( m_knotPicker, SIGNAL( clicked( double ) ),
			this, SIGNAL( knotPicked( double ) ) );
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

void PlotWidget::setKnotsEnabled( bool enabled )
{
	const QwtPlotItemList& knotList = itemList( Globals::Rtti_PlotKnot );
	for ( QwtPlotItemIterator it = knotList.begin(); it != knotList.end(); ++it )
	{
		KnotItem* knot = dynamic_cast<KnotItem*>( *it );
		knot->setEnabled( enabled );
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
		if( (*it)->rtti() == QwtPlotItem::Rtti_PlotMarker )
		{
			const QwtPlotMarker* plotMarker = dynamic_cast<const QwtPlotMarker*>( *it );
			if( plotMarker->value() == point )
			{
				return;
			}
		}
	}

	QwtPlotMarker* plotMarker = new QwtPlotMarker();
	plotMarker->setSymbol( new QwtSymbol( QwtSymbol::XCross, Qt::red, QPen( Qt::red, 2.0 ), QSize( 10, 10 ) ) );
	plotMarker->setValue( point );
	plotMarker->attach( this );
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
}

void PlotWidget::markerPointsVector( QVector<QPointF>& points ) const
{
	const QwtPlotItemList& plotItemList = this->itemList( QwtPlotItem::Rtti_PlotMarker );
	for ( QwtPlotItemIterator it = plotItemList.begin(); it != plotItemList.end(); ++it )
	{
		const QwtPlotMarker* plotMarker = dynamic_cast<const QwtPlotMarker*>( *it );
		points.append( plotMarker->value() );
	}
}

void PlotWidget::knotsVector( QVector<double>& knots ) const
{
	const QwtPlotItemList& knotList = this->itemList( Globals::Rtti_PlotKnot );
	for ( QwtPlotItemIterator it = knotList.begin(); it != knotList.end(); ++it )
	{
		const KnotItem* knot = dynamic_cast<const KnotItem*>( *it );
		knots.append( knot->coordinate() );
	}
}

void PlotWidget::panToCenter()
{
	m_plotPanner->panToCenter();
	m_plotMagnifier->rescale( 0.2 );
}

void PlotWidget::reset()
{
	const QwtPlotItemList& plotItemList = itemList();
	QList<QwtPlotItem*> itemsToDetachAndDelete;
	for ( QwtPlotItemIterator it = plotItemList.begin(); it != plotItemList.end(); ++it )
	{
		int type = (*it)->rtti();
		switch( type )
		{
		case QwtPlotItem::Rtti_PlotMarker:
		case Globals::Rtti_PlotKnot:
			{
				itemsToDetachAndDelete.append( *it );
				break;
			}
		case QwtPlotItem::Rtti_PlotCurve:
			{
				QwtPlotCurve* plotCurve = dynamic_cast<QwtPlotCurve*>( *it );
				plotCurve->setSamples( QVector<QPointF>() );

				break;
			}
		default:
			{
				continue;
			}
		}
	}

	foreach( QwtPlotItem* item, itemsToDetachAndDelete )
	{
		item->detach();
		delete item;
	}

	replot();
}