#ifndef PLOTWIDGET_H
#define PLOTWIDGET_H

#include "qwt_plot.h"

#include <qpoint.h>
#include <qvector.h>

class QwtPlotPicker;
class KnotPicker;
class CanvasPicker;
class MarkerPickerClickPointMachine;
class PlotPanner;
class PlotMagnifier;
class QwtPlotCurve;
class QwtPlotGrid;

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
	void setKnotPickerEnabled( bool enabled );
	bool isKnotPickerEnabled() const;

	void setKnotPicker( KnotPicker* knotPicker );
	void setCanvasPicker( CanvasPicker* canvasPicker );
	void setCurveSamples( const QVector<QPointF>& samples );
	void setKnotsEnabled( bool enabled = true );

	void insertKnot( double coordinate );
	void insertMarker( const QPointF& point );

	void markerPointsVector( QVector<QPointF>& points ) const;
	void knotsVector( QVector<double>& knots ) const;

	void panToCenter();

	void reset();

signals:
	void pointPicked( const QPointF& point );
	void knotPicked( double coordinate );

private:
	QwtPlotCurve* m_plotCurve;
	QwtPlotPicker* m_plotPicker;
	MarkerPickerClickPointMachine* m_markerPickerClickPointMachine;
	PlotPanner* m_plotPanner;
	PlotMagnifier* m_plotMagnifier;
	QwtPlotGrid* m_plotGrid;
	KnotPicker* m_knotPicker;
	CanvasPicker* m_canvasPicker;
};

#endif //PLOTWIDGET_H