#ifndef RANGEEXPLORER_H
#define RANGEEXPLORER_H

#include "rangemodel.h"
#include "explorerbase.h"

class KnotItem;
class PlotWidget;

class RangeExplorer : public ExplorerBase
{
	Q_OBJECT
public:
	RangeExplorer( const PlotWidget* plotWidget, QWidget* parent = 0 );
	virtual ~RangeExplorer();

	void initRange();

	virtual QString name() const;
	virtual RangeModel* sourceModel() const;
	
protected:
	virtual void showEvent( QShowEvent* event );

public slots:
	virtual void onSelectionChangedFromPlotWidget( QList<QwtPlotItem*>& selectedItems, QList<QwtPlotItem*>& deselectedItems );

private:
	const PlotWidget* m_plotWidget;
	bool m_isDefaultRangeInited;
};

#endif