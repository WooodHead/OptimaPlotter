#ifndef BASEMODEL_H
#define BASEMODEL_H

#include "qwt_plot_item.h"

#include <qabstractitemmodel.h>

class BaseModel: public QAbstractItemModel
{
	Q_OBJECT
public:
	BaseModel( QObject* parent = 0 );
	virtual ~BaseModel();

	virtual QModelIndex	index( int row, int column, const QModelIndex& parent = QModelIndex() ) const;
	QModelIndex indexOfItem( QwtPlotItem* plotItem ) const;
	virtual QModelIndex	parent( const QModelIndex & index ) const;
	virtual int	rowCount( const QModelIndex& parent = QModelIndex() ) const;
	virtual bool hasChildren( const QModelIndex& parent = QModelIndex() ) const;
	void removeItemByIndex( const QModelIndex& index );

	void emitDataChangedForItem( QwtPlotItem* plotItem );

	template <typename T>
	const T* itemFromIndexAs( const QModelIndex& index ) const;

	template <typename T>
	T* itemFromIndexAs( const QModelIndex& index );
	
signals:
	void itemAdded( QwtPlotItem* plotItem );

protected:
	QList<QwtPlotItem*> m_plotItems;
};

template <typename T>
const T* BaseModel::itemFromIndexAs( const QModelIndex& index ) const
{
	return reinterpret_cast<T*>( index.internalPointer() );
}

template <typename T>
T* BaseModel::itemFromIndexAs( const QModelIndex& index )
{
	return reinterpret_cast<T*>( index.internalPointer() );
}

#endif