#ifndef PLOTWIDGET_H
#define PLOTWIDGET_H

#include "qwt_plot.h"

#include <qpoint.h>
#include <qvector.h>

class QwtPlotPicker;
class QwtPickerClickPointMachine;
class PlotPanner;
class PlotMagnifier;
class QwtPlotCurve;

class PlotWidget : public QwtPlot
{
	Q_OBJECT

public:
	PlotWidget( QWidget* parent = 0 );
	~PlotWidget();

	void setPannerEnabled( bool enabled );
	bool isPannerEnabled() const; 
	void setPickerEnabled( bool enabled );
	bool isPickerEnabled() const;
	void setMagnifierEnabled( bool enabled );
	bool isMagnifierEnabled() const;

	void setCurveSamples( const QVector<QPointF>& samples );
	void insertMarker( const QPointF& point );

	void markerPointsVector( QVector<QPointF>& points ) const;

	void panToCenter();

	void reset();

signals:
	void pointPicked( const QPointF& point );

private:
	QwtPlotCurve* m_plotCurve;
	QwtPlotPicker* m_plotPicker;
	QwtPickerClickPointMachine* m_pickerClickPointMachine;
	PlotPanner* m_plotPanner;
	PlotMagnifier* m_plotMagnifier;
};

#endif //PLOTWIDGET_H