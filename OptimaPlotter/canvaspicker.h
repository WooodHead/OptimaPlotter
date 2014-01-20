#include <qobject.h>

class QPoint;
class QCustomEvent;
class QwtPlot;
class QwtPlotCurve;
class QwtPlotMarker;
class KnotItem;

class CanvasPicker: public QObject
{
    Q_OBJECT

public:
    CanvasPicker( QwtPlot* plot );
    virtual bool eventFilter( QObject*, QEvent* );
	virtual bool event( QEvent* );

private:
    void select( const QPoint& );
    void move( const QPoint& );
	void release();

	QwtPlot* plot();
    const QwtPlot* plot() const;

    QwtPlotMarker* m_selectedMarker;
	KnotItem* m_selectedKnot;
};
