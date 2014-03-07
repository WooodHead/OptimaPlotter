#include "stdafx.h"
#include "knotexplorer.h"
#include "knotmodel.h"
#include "globals.h"

#include <qtreeview.h>
#include <qaction.h>

KnotExplorer::KnotExplorer( QWidget* parent ) : ExplorerBase( parent )
{
	m_model = new KnotModel( this );
	m_treeView->setModel( m_model );
	
	connect( m_actionAdd, SIGNAL( triggered() ), this, SLOT( onAddNewKnot() ) );
	connect( m_model, SIGNAL( itemAdded( QwtPlotItem* ) ), this, SIGNAL( itemAdded( QwtPlotItem* ) ) );
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
		model()->onAddNewKnot( dynamic_cast<KnotItem*>( plotItem ), false );
	}
}

void KnotExplorer::onAddNewKnot()
{
	model()->onAddNewKnot( new KnotItem(), true );
}

void KnotExplorer::addKnot( double coordinate, bool isKnotEditable )
{
	KnotItem* knotItem = new KnotItem( coordinate );
	knotItem->setEditAllowed( isKnotEditable );
	model()->onAddNewKnot( knotItem, true );
}

KnotModel* KnotExplorer::model() const
{
	return dynamic_cast<KnotModel*>( m_model );
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