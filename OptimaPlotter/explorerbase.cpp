#include "stdafx.h"
#include "explorerbase.h"
#include "basemodel.h"
#include "globals.h"
#include "doublepropertydelegate.h"
#include "sortingmodel.h"

#include <qtoolbar.h>
#include <qboxlayout.h>
#include <qaction.h>
#include <qtreeview.h>
#include <qevent.h>

ExplorerBase::ExplorerBase( QWidget* parent ) : QWidget( parent ), m_sourceModel( 0 )
{
	m_treeView = new QTreeView( this );
	m_treeView->setSelectionMode( QAbstractItemView::ExtendedSelection );
	
	m_sortingModel = new SortingModel( this );
	m_treeView->setModel( m_sortingModel );
	m_treeView->setSortingEnabled( true );
	m_treeView->sortByColumn( 0, Qt::AscendingOrder );
	m_sortingModel->setDynamicSortFilter( true );

	DoublePropertyDelegate* doublePropertyDelegate = new DoublePropertyDelegate( m_treeView );
	m_treeView->setItemDelegate( doublePropertyDelegate );

	setupUi();
}

ExplorerBase::~ExplorerBase()
{

}

void ExplorerBase::setupUi()
{
	QVBoxLayout* verticalLayout = new QVBoxLayout;

	m_toolBar = new QToolBar( this );
	m_toolBar->setFloatable( false );
	m_toolBar->setMovable( false );

	m_actionAdd = new QAction( tr( "Add" ), this );
	m_toolBar->addAction( m_actionAdd );

	m_actionDelete = new QAction( tr( "Delete" ), this );
	m_actionDelete->setEnabled( false );
	connect( m_actionDelete, SIGNAL( triggered() ), this, SLOT( onDelete() ) );
	m_toolBar->addAction( m_actionDelete );

	verticalLayout->addWidget( m_toolBar );
	verticalLayout->addWidget( m_treeView );
	verticalLayout->setMargin( 1 );
	setLayout( verticalLayout );
}

void ExplorerBase::allowActions( bool allow )
{
	if( m_toolBar )
		m_toolBar->setVisible( allow );
}

QTreeView* ExplorerBase::treeView() const
{
	return m_treeView;
}

BaseModel* ExplorerBase::sourceModel() const
{
	return m_sourceModel;
}

void ExplorerBase::onDataChanged( QwtPlotItem* plotItem )
{
	if( plotItem->rtti() == Globals::Rtti_PlotMarker ||
		plotItem->rtti() == Globals::Rtti_PlotKnot )
	{
		m_sourceModel->emitDataChangedForItem( plotItem );
	}
}

void ExplorerBase::onSelectionChanged( const QItemSelection& selected, const QItemSelection& deselected )
{
	if( m_treeView->selectionModel()->selectedRows().count() == 0 )
		m_actionDelete->setEnabled( false );
	else
		m_actionDelete->setEnabled( true );

	QItemSelection sourceSelected;
	QItemSelection sourceDeselected;

	sourceSelected =  m_sortingModel->mapSelectionToSource( selected );
	sourceDeselected = m_sortingModel->mapSelectionToSource( deselected );

	QModelIndexList selectedIndexes = sourceSelected.indexes();
	QModelIndexList deselectedIndexes = sourceDeselected.indexes();

	QList<QwtPlotItem*> selectedItems;
	QList<QwtPlotItem*> deselectedItems;

	foreach( QModelIndex index, selectedIndexes )
	{
		if( index.column() != 0 )
			continue;
		selectedItems.append( sourceModel()->itemFromIndexAs<QwtPlotItem>( index ) );
	}

	foreach( QModelIndex index, deselectedIndexes )
	{
		if( index.column() != 0 )
			continue;
		deselectedItems.append( sourceModel()->itemFromIndexAs<QwtPlotItem>( index ) );
	}

	emit selectionChanged( selectedItems, deselectedItems );
}


void ExplorerBase::onSelectionChangedFromPlotWidget( QList<QwtPlotItem*>& selectedItems, QList<QwtPlotItem*>& deselectedItems )
{
	blockSignals( true );
	foreach( QwtPlotItem* plotItem, selectedItems )
		m_treeView->selectionModel()->select( m_sortingModel->indexOfItem( plotItem ), QItemSelectionModel::Select | QItemSelectionModel::Rows );

	foreach( QwtPlotItem* plotItem, deselectedItems )
		m_treeView->selectionModel()->select( m_sortingModel->indexOfItem( plotItem ), QItemSelectionModel::Deselect | QItemSelectionModel::Rows );
	blockSignals( false );
}

void ExplorerBase::deleteAllItems()
{
	QItemSelection selection;
	QModelIndex topLeftIndex;
	QModelIndex bottomRightIndex;
	topLeftIndex = m_sourceModel->index( 0, 0 );
	bottomRightIndex = m_sourceModel->index( m_sourceModel->rowCount() - 1, m_sourceModel->columnCount() - 1 );

	selection.select( topLeftIndex, bottomRightIndex );
	selection = m_sortingModel->mapSelectionFromSource( selection );
	m_treeView->selectionModel()->select( selection, QItemSelectionModel::Select );
	onDelete();
}

void ExplorerBase::onDelete()
{
	QItemSelection itemSelection = m_treeView->selectionModel()->selection();
	itemSelection = m_sortingModel->mapSelectionToSource( itemSelection );
	QModelIndexList selectedIndexes = itemSelection.indexes();
	QList<QPersistentModelIndex> selectedPersistentIndexes;

	blockSignals( true );
	foreach( QModelIndex index, selectedIndexes )
	{
		if( index.column() != 0 )
			continue;
		selectedPersistentIndexes.append( index );
	}

	foreach( QPersistentModelIndex persistentIndex, selectedPersistentIndexes )
	{
		m_sourceModel->removeItemByIndex( persistentIndex );
	}
	blockSignals( false );

	emit deleteSelectedItems();
}

void ExplorerBase::changeEvent( QEvent* event )
{
	if( event->type() == QEvent::LanguageChange )
		retranslateUi();
	else
		QWidget::changeEvent( event );
}

void ExplorerBase::retranslateUi()
{
	m_actionAdd->setText( tr( "Add" ) );
	m_actionDelete->setText( tr( "Delete" ) );
}