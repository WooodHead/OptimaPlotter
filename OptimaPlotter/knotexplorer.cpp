#include "stdafx.h"
#include "knotexplorer.h"
#include "knotmodel.h"
#include "globals.h"
#include "sortingmodel.h"

#include <qtreeview.h>
#include <qaction.h>

KnotExplorer::KnotExplorer( QWidget* parent ) : ExplorerBase( parent )
{
	m_sourceModel = new KnotModel( this );
	m_sortingModel->setSourceModel( m_sourceModel );
	
	connect( m_actionAdd, SIGNAL( triggered() ), this, SLOT( onAddNewKnot() ) );
	connect( m_sourceModel, SIGNAL( itemAdded( QwtPlotItem* ) ), this, SIGNAL( itemAdded( QwtPlotItem* ) ) );
	connect( m_treeView->selectionModel(), SIGNAL( selectionChanged( const QItemSelection& , const QItemSelection& ) ),
		this, SLOT( onSelectionChanged( const QItemSelection& , const QItemSelection& ) ) );
}

KnotExplorer::~KnotExplorer()
{

}

QString KnotExplorer::name() const
{
	return tr( "Knot Explorer" );
}

void KnotExplorer::onItemAdded( QwtPlotItem* plotItem )
{
	if( plotItem->rtti() == Globals::Rtti_PlotKnot )
	{
		sourceModel()->onAddNewKnot( dynamic_cast<KnotItem*>( plotItem ), false );
	}
}

void KnotExplorer::onAddNewKnot()
{
	sourceModel()->onAddNewKnot( new KnotItem(), true );
}

void KnotExplorer::addKnot( double coordinate, bool isKnotEditable )
{
	KnotItem* knotItem = new KnotItem( coordinate );
	knotItem->setEditAllowed( isKnotEditable );
	sourceModel()->onAddNewKnot( knotItem, true );
}

KnotModel* KnotExplorer::sourceModel() const
{
	return dynamic_cast<KnotModel*>( m_sourceModel );
}

void KnotExplorer::onSelectionChangedFromPlotWidget( QList<QwtPlotItem*>& selectedItems, QList<QwtPlotItem*>& deselectedItems )
{
	int itemType;
	if( selectedItems.count() != 0 )
		itemType = selectedItems.first()->rtti();
	else if( deselectedItems.count() != 0 )
		itemType = deselectedItems.first()->rtti();
	else
		return;

	if( itemType == Globals::Rtti_PlotKnot )
		ExplorerBase::onSelectionChangedFromPlotWidget( selectedItems, deselectedItems );
}