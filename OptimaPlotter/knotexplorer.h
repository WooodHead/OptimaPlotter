#ifndef KNOTEXPLORER_H
#define KNOTEXPLORER_H

#include "explorerbase.h"
#include "knotmodel.h"

class KnotItem;

class KnotExplorer : public ExplorerBase
{
	Q_OBJECT
public:
	KnotExplorer( QWidget* parent = 0 );
	virtual ~KnotExplorer();

	virtual QString name() const;
	virtual KnotModel* model() const;

public slots:
	void onItemAdded( QwtPlotItem* plotItem );
	virtual void onSelectionChangedFromPlotWidget( QList<QwtPlotItem*>& selectedItems, QList<QwtPlotItem*>& deselectedItems );

private slots:
	void onAddNewKnot();

signals:
	void knotAdded( KnotItem* knotItem );
};

#endif