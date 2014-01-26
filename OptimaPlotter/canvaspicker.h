#ifndef CANVASPICKER_H
#define CANVASPICKER_H

#include "qobject.h"
#include "qpoint.h"

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

private:
    void select( const QPoint&, Qt::KeyboardModifiers modifiers );
    void move( const QPoint& );
	void release( const QPoint&, Qt::KeyboardModifiers modifiers );

	QwtPlot* plot();
    const QwtPlot* plot() const;

signals:
	void picked( Qt::KeyboardModifiers modifiers, QwtPlotItem* item );

private:
	bool m_isEnabled;

	QwtPlotItem* m_itemToPick;
	QPoint m_selectionPoint;
	QPoint m_previousPoint;
	bool m_dragAndDropInProgress;
	int m_typeOfItemsToDrag;
};

#endif