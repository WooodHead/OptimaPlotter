#ifndef KNOTPICKER_H
#define KNOTPICKER_H

#include <qobject.h>
#include <qrect.h>

class QwtPlot;
class QwtScaleWidget;

class KnotPicker: public QObject
{
    Q_OBJECT
public:
    KnotPicker( QwtPlot *plot );
    virtual bool eventFilter( QObject *, QEvent * );

	void setEnabled( bool enabled = true );
	bool isEnabled() const;

signals:
    void clicked( double value );

private:
    void mouseClicked( const QwtScaleWidget *, const QPoint & );
    QRect scaleRect( const QwtScaleWidget * ) const;

	bool m_isEnabled;
	QwtScaleWidget* m_bottomXAxis;
};

#endif