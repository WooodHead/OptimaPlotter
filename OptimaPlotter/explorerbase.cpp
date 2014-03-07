#include "stdafx.h"
#include "explorerbase.h"
#include "basemodel.h"
#include "globals.h"

#include "doublepropertydelegate.h"

#include <qtoolbar.h>
#include <qboxlayout.h>
#include <qaction.h>
#include <qtreeview.h>
#include <qevent.h>

ExplorerBase::ExplorerBase( QWidget* parent ) : QWidget( parent ), m_model( 0 )
{
	m_treeView = new QTreeView( this );
	m_treeView->setSelectionMode( QAbstractItemView::ExtendedSelection );

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

BaseModel* ExplorerBase::model() const
{
	return m_model;
}

void ExplorerBase::onDataChanged( QwtPlotItem* plotItem )
{
	if( plotItem->rtti() == Globals::Rtti_PlotMarker ||
		plotItem->rtti() == Globals::Rtti_PlotKnot )
	{
		m_model->emitDataChangedForItem( plotItem );
	}
}

void ExplorerBase::onSelectionChanged( const QItemSelection& selected, const QItemSelection& deselected )
{
	if( m_treeView->selectionModel()->selectedRows().count() == 0 )
		m_actionDelete->setEnabled( false );
	else
		m_actionDelete->setEnabled( true );

	QModelIndexList selectedIndexes = selected.indexes();
	QModelIndexList deselectedIndexes = deselected.indexes();

	QList<QwtPlotItem*> selectedItems;
	QList<QwtPlotItem*> deselectedItems;

	foreach( QModelIndex index, selectedIndexes )
	{
		if( index.column() != 0 )
			continue;
		selectedItems.append( model()->itemFromIndexAs<QwtPlotItem>( index ) );
	}

	foreach( QModelIndex index, deselectedIndexes )
	{
		if( index.column() != 0 )
			continue;
		deselectedItems.append( model()->itemFromIndexAs<QwtPlotItem>( index ) );
	}

	emit selectionChanged( selectedItems, deselectedItems );
}


void ExplorerBase::onSelectionChangedFromPlotWidget( QList<QwtPlotItem*>& selectedItems, QList<QwtPlotItem*>& deselectedItems )
{
	blockSignals( true );
	foreach( QwtPlotItem* plotItem, selectedItems )
		m_treeView->selectionModel()->select( m_model->indexOfItem( plotItem ), QItemSelectionModel::Select | QItemSelectionModel::Rows );

	foreach( QwtPlotItem* plotItem, deselectedItems )
		m_treeView->selectionModel()->select( m_model->indexOfItem( plotItem ), QItemSelectionModel::Deselect | QItemSelectionModel::Rows );
	blockSignals( false );
}

void ExplorerBase::deleteAllItems()
{
	QItemSelection selection;
	QModelIndex topLeftIndex;
	QModelIndex bottomRightIndex;
	topLeftIndex = model()->index( 0, 0 );
	bottomRightIndex = model()->index( model()->rowCount() - 1, model()->columnCount() - 1 );

	selection.select( topLeftIndex, bottomRightIndex );
	m_treeView->selectionModel()->select( selection, QItemSelectionModel::Select );
	onDelete();
}

void ExplorerBase::onDelete()
{
	QItemSelection itemSelection = m_treeView->selectionModel()->selection();
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
		m_model->removeItemByIndex( persistentIndex );
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