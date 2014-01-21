#ifndef CANVASPICKER_H
#define CANVASPICKER_H

#include <qobject.h>

class QPoint;
class QCustomEvent;
class QwtPlot;
class QwtPlotCurve;
class MarkerItem;
class KnotItem;
class QwtPlotItem;

class CanvasPicker: public QObject
{
    Q_OBJECT

public:
    CanvasPicker( QwtPlot* plot );
    virtual bool eventFilter( QObject*, QEvent* );
	virtual bool event( QEvent* );

	void setEnabled( bool enabled = true );
	bool isEnabled() const;

signals:
	void picked( Qt::KeyboardModifiers modifiers, QwtPlotItem* item );

private:
	bool m_isEnabled;

    void select( const QPoint&, Qt::KeyboardModifiers modifiers );
    void move( const QPoint& );
	void release();

	QwtPlot* plot();
    const QwtPlot* plot() const;

    MarkerItem* m_selectedMarker;
	KnotItem* m_selectedKnot;
};

#endif