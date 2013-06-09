#ifndef PLOTPANNER_H
#define PLOTPANNER_H

#include "qwt_plot_panner.h"

class PlotPanner : public QwtPlotPanner
{
	Q_OBJECT

public:
	PlotPanner( QWidget* parent );
	~PlotPanner();

	void panToCenter();
};

#endif //PLOTPANNER_H