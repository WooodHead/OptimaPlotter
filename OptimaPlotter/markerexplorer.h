#ifndef MARKEREXPLORER_H
#define MARKEREXPLORER_h

#include "explorerbase.h"
#include "markermodel.h"

class MarkerItem;
class PlotItem;
class QwtPlotItem;

class MarkerExplorer : public ExplorerBase
{
	Q_OBJECT
public:
	MarkerExplorer( QWidget* parent = 0 );
	virtual ~MarkerExplorer();

	virtual QString name() const;

	virtual MarkerModel* sourceModel() const;

public slots:
	void onItemAdded( QwtPlotItem* plotItem );
	void onSelectionChangedFromPlotWidget( QList<QwtPlotItem*>& selectedItems, QList<QwtPlotItem*>& deselectedItems );

private slots:
	void onAddNewMarker();

signals:
	void markerAdded( MarkerItem* markerItem );
};

#endif