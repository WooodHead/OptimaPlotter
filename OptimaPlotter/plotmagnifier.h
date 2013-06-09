#ifndef PLOTMAGNIFIER_H
#define PLOTMAGNIFIER_H

#include "qwt_plot_magnifier.h"

class PlotMagnifier : public QwtPlotMagnifier
{
	Q_OBJECT

public:
	PlotMagnifier( QWidget* parent );
	~PlotMagnifier();

	void rescale( double factor );
};

#endif //PLOTMAGNIFIER_H