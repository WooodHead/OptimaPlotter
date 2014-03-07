#ifndef EXPLORERBASE_H
#define EXPLORERBASE_H

#include <qwidget.h>
#include <qstring.h>
#include <qlist.h>

class QToolBar;
class QAction;
class QTreeView;
class BaseModel;
class QwtPlotItem;
class QItemSelection;
class QEvent;

class ExplorerBase : public QWidget
{
	Q_OBJECT
public:
	ExplorerBase( QWidget* parent = 0 );
	virtual ~ExplorerBase();

	virtual QString name() const = 0;
	virtual void setupUi();
	QTreeView* treeView() const;

	void deleteAllItems();
	void allowActions( bool allow = true );

	virtual BaseModel* model() const;

public slots:
	void onDataChanged( QwtPlotItem* plotItem );
	void onSelectionChangedFromPlotWidget( QList<QwtPlotItem*>& selectedItems, QList<QwtPlotItem*>& deselectedItems );

signals:
	void itemAdded( QwtPlotItem* plotItem );
	void selectionChanged( QList<QwtPlotItem*>& selectedItems, QList<QwtPlotItem*>& deselectedItems );
	void deleteSelectedItems();

private slots:
	void onSelectionChanged( const QItemSelection& selected, const QItemSelection& deselected );
	void onDelete();

protected:
	virtual void changeEvent( QEvent* event );
	virtual void retranslateUi();

protected:
	QToolBar* m_toolBar;

	QAction* m_actionAdd;
	QAction* m_actionDelete;

	QTreeView* m_treeView;
	BaseModel* m_model;
};

#endif