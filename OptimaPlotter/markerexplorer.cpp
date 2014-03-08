#include "stdafx.h"
#include "markerexplorer.h"
#include "markermodel.h"
#include "markeritem.h"
#include "sortingmodel.h"

#include "globals.h"

#include <qtreeview.h>
#include <qaction.h>

MarkerExplorer::MarkerExplorer( QWidget* parent ) : ExplorerBase( parent )
{
	m_sourceModel = new MarkerModel( this );
	m_sortingModel->setSourceModel( m_sourceModel );
	
	connect( m_actionAdd, SIGNAL( triggered() ), this, SLOT( onAddNewMarker() ) );
	connect( m_sourceModel, SIGNAL( itemAdded( QwtPlotItem* ) ), this, SIGNAL( itemAdded( QwtPlotItem* ) ) );
	connect( m_treeView->selectionModel(), SIGNAL( selectionChanged( const QItemSelection& , const QItemSelection& ) ),
		this, SLOT( onSelectionChanged( const QItemSelection& , const QItemSelection& ) ) );
}

MarkerExplorer::~MarkerExplorer()
{

}

QString MarkerExplorer::name() const
{
	return tr( "Marker Explorer" );
}

void MarkerExplorer::onItemAdded( QwtPlotItem* plotItem )
{
	if( plotItem->rtti() == Globals::Rtti_PlotMarker )
	{
		sourceModel()->onAddNewMarker( dynamic_cast<MarkerItem*>( plotItem ), false );
	}
}

void MarkerExplorer::onAddNewMarker()
{
	sourceModel()->onAddNewMarker( new MarkerItem(), true );
}

MarkerModel* MarkerExplorer::sourceModel() const
{
	return dynamic_cast<MarkerModel*>( m_sourceModel );
}

void MarkerExplorer::onSelectionChangedFromPlotWidget( QList<QwtPlotItem*>& selectedItems, QList<QwtPlotItem*>& deselectedItems )
{
	int itemType;
	if( selectedItems.count() != 0 )
		itemType = selectedItems.first()->rtti();
	else if( deselectedItems.count() != 0 )
		itemType = deselectedItems.first()->rtti();
	else
		return;

	if( itemType == Globals::Rtti_PlotMarker )
		ExplorerBase::onSelectionChangedFromPlotWidget( selectedItems, deselectedItems );
}