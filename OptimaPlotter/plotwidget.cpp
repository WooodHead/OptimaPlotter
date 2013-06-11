#include "stdafx.h"
#include "plotwidget.h"
#include "plotpanner.h"
#include "plotmagnifier.h"

#include "qwt_plot_curve.h"
#include "qwt_plot_panner.h"
#include "qwt_plot_magnifier.h"
#include "qwt_plot_picker.h"
#include "qwt_picker_machine.h"
#include "qwt_plot_curve.h"
#include "qwt_plot_marker.h"
#include "qwt_symbol.h"
#include "qwt_plot_grid.h"

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

	m_plotCurve = new QwtPlotCurve();
	m_plotCurve->setPen( Qt::blue, 2.0, Qt::SolidLine );
	m_plotCurve->attach( this );

	m_plotGrid = new QwtPlotGrid;
    m_plotGrid->setMajorPen( Qt::black, 1, Qt::DotLine );
    m_plotGrid->attach( this );

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

void PlotWidget::markerPointsVector( QVector<QPointF>& points ) const
{
	const QwtPlotItemList& plotItemList = this->itemList();
	for ( QwtPlotItemIterator it = plotItemList.begin(); it != plotItemList.end(); ++it )
	{
		if( (*it)->rtti() == QwtPlotItem::Rtti_PlotMarker )
		{
			const QwtPlotMarker* plotMarker = dynamic_cast<const QwtPlotMarker*>( *it );
			points.append( plotMarker->value() );
		}
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