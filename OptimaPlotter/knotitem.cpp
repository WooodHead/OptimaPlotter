#include "stdafx.h"
#include "knotitem.h"
#include "globals.h"

#include "qwt_painter.h"
#include "qwt_plot.h"

#include <qpainter.h>
#include <qpen.h>

KnotItem::KnotItem( bool isRangeBorder ) : QwtPlotItem( QwtText( "Knot" ) ), Selectable(), m_isEnabled( true ), m_isEditAllowed( true ), m_isRangeBorder( isRangeBorder )
{
    setItemInterest( QwtPlotItem::ScaleInterest, true );
    setZ( 10.0 ); //TODO: do we really need this?

	m_coordinate = 0.0;
}

KnotItem::KnotItem( double coordinate, bool isRangeBorder ) : QwtPlotItem( QwtText( "Knot" ) ), Selectable(), m_isEnabled( true ), m_isEditAllowed( true ), m_isRangeBorder( isRangeBorder ), m_coordinate( coordinate )
{
    setItemInterest( QwtPlotItem::ScaleInterest, true );
    setZ( 10.0 ); //TODO: do we really need this?
}

KnotItem::~KnotItem() 
{

}

int KnotItem::rtti() const
{
	return Globals::Rtti_PlotKnot;
}

void KnotItem::draw( QPainter* p, const QwtScaleMap& xMap, const QwtScaleMap& yMap, const QRectF& rect ) const
{
	if( m_isEnabled && m_coordinate < m_xScaleDiv.upperBound() &&
		m_coordinate > m_xScaleDiv.lowerBound() )
	{
		double value = xMap.transform( m_coordinate );

		double y1 = rect.top();
		double y2 = rect.bottom() - 1.0;

		QPen pen;
		if( isRangeBorder() )
		{
			pen.setColor( isSelected() ? Qt::black : Qt::darkMagenta );
			pen.setWidth( isSelected() ? 4.0 : 2.5 );
		}
		else
		{
			pen.setColor( isSelected() ? Qt::black : Qt::darkCyan );
			pen.setWidth( isSelected() ? 3.0 : 2.0 );

			if( !isEditAllowed() )
			{
				pen.setStyle( Qt::DashDotLine );
			}
		}
		p->setPen( pen );

		QwtPainter::drawLine( p, value, y1, value, y2 );
	}
}

void KnotItem::updateScaleDiv( const QwtScaleDiv& xMap, const QwtScaleDiv& yMap )
{
	m_xScaleDiv = xMap;
	m_yScaleDiv = yMap;
}

void KnotItem::setCoordinate( double coordinate )
{
	m_coordinate = coordinate;
}

double KnotItem::coordinate() const
{
	return m_coordinate;
}

void KnotItem::setEnabled( bool enabled )
{
	m_isEnabled = enabled;
}

bool KnotItem::isEnabled() const
{
	return m_isEnabled;
}

void KnotItem::setEditAllowed( bool enabled )
{
	if( enabled != m_isEditAllowed )
	{
		m_isEditAllowed = enabled;
		if( plot() )
			plot()->replot();
	}
}

bool KnotItem::isEditAllowed() const
{
	return m_isEditAllowed;
}

bool KnotItem::isRangeBorder() const
{
	return m_isRangeBorder;
}

void KnotItem::select()
{
	if( !isSelected() )
	{
		Selectable::select();
		plot()->replot();
	}
}

void KnotItem::deselect()
{
	if( isSelected() )
	{
		Selectable::deselect();
		plot()->replot();
	}
}