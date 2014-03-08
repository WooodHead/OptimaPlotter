#ifndef SORTINGMODEL_H
#define SORTINGMODEL_H

#include "basemodel.h"

#include <qsortfilterproxymodel.h>

class SortingModel: public QSortFilterProxyModel
{
public:
	SortingModel( QObject* parent = 0 );
	virtual ~SortingModel();

	virtual bool lessThan ( const QModelIndex& left, const QModelIndex& right ) const;

	BaseModel* sourceModel() const;
	QModelIndex indexOfItem( QwtPlotItem* plotItem ) const;

	template <typename T>
	const T* itemFromIndexAs( const QModelIndex& index ) const;

	template <typename T>
	T* itemFromIndexAs( const QModelIndex& index );
};

template <typename T>
const T* SortingModel::itemFromIndexAs( const QModelIndex& index ) const
{
	if( sourceModel() )
		return sourceModel()->itemFromIndexAs<T>( mapToSource( index ) );
	return 0;
}

template <typename T>
T* SortingModel::itemFromIndexAs( const QModelIndex& index )
{
	if( sourceModel() )
		return sourceModel()->itemFromIndexAs<T>( mapToSource( index ) );
	return 0;
}


#endif