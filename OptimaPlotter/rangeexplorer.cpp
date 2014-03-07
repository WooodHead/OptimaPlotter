#include "stdafx.h"
#include "rangeexplorer.h"
#include "rangemodel.h"
#include "globals.h"
#include "optimaplotter.h"
#include "plotwidget.h"

#include <qtreeview.h>
#include <qaction.h>
#include <qapplication.h>

RangeExplorer::RangeExplorer( const PlotWidget* plotWidget, QWidget* parent ) : ExplorerBase( parent ),
	m_plotWidget( plotWidget ), m_isDefaultRangeInited( false )
{
	m_model = new RangeModel( this );
	m_treeView->setModel( m_model );
	
	//connect( m_actionAdd, SIGNAL( triggered() ), this, SLOT( onAddNewKnot() ) );
	connect( m_model, SIGNAL( itemAdded( QwtPlotItem* ) ), this, SIGNAL( itemAdded( QwtPlotItem* ) ) );
	connect( m_treeView->selectionModel(), SIGNAL( selectionChanged( const QItemSelection& , const QItemSelection& ) ),
		this, SLOT( onSelectionChanged( const QItemSelection& , const QItemSelection& ) ) );

	m_toolBar->hide();
}

RangeExplorer::~RangeExplorer()
{

}

QString RangeExplorer::name() const
{
	return tr( "Range Explorer" );
}

RangeModel* RangeExplorer::model() const
{
	return dynamic_cast<RangeModel*>( m_model );
}

void RangeExplorer::initRange()
{
	if( !m_isDefaultRangeInited )
	{
		if( m_plotWidget )
		{
			double x1 = 0.0;
			double x2 = 0.0;

			double leftBound, rightBound;

			m_plotWidget->boundingXCoordinates( x1, x2 );
			
			leftBound = x1 + ( x2 - x1 ) / 10.0;
			rightBound = x2 - ( x2 - x1 ) / 10.0;
			
			KnotItem* knotItem1 = new KnotItem( true );
			knotItem1->setCoordinate( leftBound );
			model()->onAddNewKnot( knotItem1 );

			KnotItem* knotItem2 = new KnotItem( true );
			knotItem2->setCoordinate( rightBound );
			model()->onAddNewKnot( knotItem2 );

			m_isDefaultRangeInited = true;
		}
	}
}

void RangeExplorer::onSelectionChangedFromPlotWidget( QList<QwtPlotItem*>& selectedItems, QList<QwtPlotItem*>& deselectedItems )
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

void RangeExplorer::showEvent( QShowEvent * event ) 
{
	if( !m_isDefaultRangeInited )
	{
		initRange();
	}
}