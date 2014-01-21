#ifndef KNOTITEM_H
#define KNOTITEM_H

#include "selectable.h"

#include "qwt_global.h"
#include "qwt_plot_item.h"
#include "qwt_scale_map.h"
#include "qwt_scale_div.h"

class QPainter;

class KnotItem: public QwtPlotItem, public Selectable
{
public:
	KnotItem();
	virtual ~KnotItem();

	virtual int rtti() const;
	virtual void draw( QPainter* p, const QwtScaleMap& xMap, const QwtScaleMap& yMap, const QRectF& rect ) const;
    virtual void updateScaleDiv( const QwtScaleDiv &xMap, const QwtScaleDiv &yMap );

	void setCoordinate( double coordinate );
	double coordinate() const;

	void setEnabled( bool enabled = true );
	bool isEnabled() const;

	virtual void select();
	virtual void deselect();

private:
	QwtScaleDiv m_xScaleDiv;
    QwtScaleDiv m_yScaleDiv;
	double m_coordinate;
	bool m_isEnabled;
};

#endif