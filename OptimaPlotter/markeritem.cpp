#include "stdafx.h"
#include "markeritem.h"
#include "globals.h"

#include "qwt_symbol.h"
#include "qwt_plot.h"

MarkerItem::MarkerItem() : QwtPlotMarker(), Selectable()
{
	setSymbol( new QwtSymbol( QwtSymbol::XCross, Qt::red, QPen( Qt::red, 2.0 ), QSize( 10, 10 ) ) );
}

MarkerItem::~MarkerItem()
{

}

int MarkerItem::rtti() const
{
	return Globals::Rtti_PlotMarker;
}

void MarkerItem::select()
{
	if( !isSelected() )
	{
		Selectable::select();
		setSymbol( new QwtSymbol( QwtSymbol::XCross, Qt::black, QPen( Qt::black, 3.0 ), QSize( 10, 10 ) ) );
		plot()->replot();
	}
}

void MarkerItem::deselect()
{
	if( isSelected() )
	{
		Selectable::deselect();
		setSymbol( new QwtSymbol( QwtSymbol::XCross, Qt::red, QPen( Qt::red, 2.0 ), QSize( 10, 10 ) ) );
		plot()->replot();
	}
}