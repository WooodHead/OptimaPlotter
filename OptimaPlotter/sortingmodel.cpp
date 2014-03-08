#include "stdafx.h"
#include "sortingmodel.h"
#include "markeritem.h"
#include "knotitem.h"

#include "globals.h"

SortingModel::SortingModel( QObject* parent ) : QSortFilterProxyModel( parent )
{

}
	
SortingModel::~SortingModel()
{

}

bool SortingModel::lessThan ( const QModelIndex& left, const QModelIndex& right ) const
{
	const QwtPlotItem* leftItem = sourceModel()->itemFromIndexAs<QwtPlotItem>( left );
	const QwtPlotItem* rightItem = sourceModel()->itemFromIndexAs<QwtPlotItem>( right );

	if( leftItem->rtti() == rightItem->rtti() )
	{
		if( leftItem->rtti() == Globals::Rtti_PlotMarker )
		{
			const MarkerItem* leftMarker = dynamic_cast<const MarkerItem*>( leftItem );
			const MarkerItem* rightMarker = dynamic_cast<const MarkerItem*>( rightItem );

			return ( left.column() == 0 ? 
				leftMarker->xValue() < rightMarker->xValue() :
				leftMarker->yValue() < rightMarker->yValue() );
		}
		else if( leftItem->rtti() == Globals::Rtti_PlotKnot )
		{
			const KnotItem* leftKnot = dynamic_cast<const KnotItem*>( leftItem );
			const KnotItem* rightKnot = dynamic_cast<const KnotItem*>( rightItem );

			return ( leftKnot->coordinate() < rightKnot->coordinate() );
		}
	}

	return true;
}

BaseModel* SortingModel::sourceModel() const
{
	return dynamic_cast<BaseModel*>( QSortFilterProxyModel::sourceModel() );
}

QModelIndex SortingModel::indexOfItem( QwtPlotItem* plotItem ) const
{
	return mapFromSource( sourceModel()->indexOfItem( plotItem ) );
}