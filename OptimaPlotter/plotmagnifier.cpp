#include "stdafx.h"
#include "plotmagnifier.h"

PlotMagnifier::PlotMagnifier( QWidget* parent ): QwtPlotMagnifier( parent )
{

}

PlotMagnifier::~PlotMagnifier()
{

}

void PlotMagnifier::rescale( double factor )
{
	QwtPlotMagnifier::rescale( factor );
}