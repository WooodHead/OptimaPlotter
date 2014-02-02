#include "stdafx.h"
#include "basemodel.h"

BaseModel::BaseModel( QObject* parent ) : QAbstractItemModel( parent )
{

}

BaseModel::~BaseModel()
{

}

QModelIndex	BaseModel::index( int row, int column, const QModelIndex& parent ) const
{
	if( row >= 0 &&
		row < m_plotItems.count() )
		return createIndex( row, column, m_plotItems.at( row ) );

	return QModelIndex();
}

QModelIndex	BaseModel::parent( const QModelIndex & index ) const
{
	return QModelIndex();
}

int	BaseModel::rowCount( const QModelIndex& parent ) const
{
	return m_plotItems.count();
}

bool BaseModel::hasChildren( const QModelIndex& parent ) const
{
	if( !parent.isValid() )
		return true;

	return false;
}

void BaseModel::emitDataChangedForItem( QwtPlotItem* plotItem )
{
	const int row = m_plotItems.indexOf( plotItem );
	QModelIndex startIndex = createIndex( row, 0, plotItem );
	QModelIndex endIndex = createIndex( row, columnCount() - 1, plotItem );
	emit dataChanged( startIndex, endIndex );
}

QModelIndex BaseModel::indexOfItem( QwtPlotItem* plotItem ) const
{
	const int row = m_plotItems.indexOf( plotItem );
	return createIndex( row, 0, plotItem );
}

void BaseModel::removeItemByIndex( const QModelIndex& index )
{
	QwtPlotItem* plotItemToRemove = itemFromIndexAs<QwtPlotItem>( index );
	beginRemoveRows( QModelIndex(), index.row(), index.row() );
	m_plotItems.removeAll( plotItemToRemove );
	endRemoveRows();
}

Qt::ItemFlags BaseModel::flags ( const QModelIndex& index ) const
{
	if( index.isValid() )
		return QAbstractItemModel::flags( index ) | Qt::ItemIsEditable;
	return QAbstractItemModel::flags( index );
}