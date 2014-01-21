#ifndef MARKERITEM_H
#define MARKERITEM_H

#include "selectable.h"

#include "qwt_plot_marker.h"

class MarkerItem: public QwtPlotMarker, public Selectable
{
public:
	MarkerItem();
	virtual ~MarkerItem();

	virtual int rtti() const;

	virtual void select();
	virtual void deselect();
};

#endif