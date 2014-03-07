#include "stdafx.h"
#include "rangemodel.h"

#include "qwt_plot.h"

#include <qpoint.h>

RangeModel::RangeModel( QObject* parent ) : BaseModel( parent )
{
}

RangeModel::~RangeModel()
{

}

int RangeModel::columnCount( const QModelIndex& parent ) const
{
	return 1;
}

QVariant RangeModel::data( const QModelIndex& index, int role ) const
{
	if( role == Qt::DisplayRole )
	{
		const int row = index.row();
		const int column = index.column();

		if( row >= 0 && row < m_plotItems.count() )
		{
			QwtPlotItem* plotItem = m_plotItems.at( row );
			KnotItem* knotItem = dynamic_cast<KnotItem*>( plotItem );

			if( knotItem )
			{
				switch( column )
				{
				case 0:
					return knotItem->coordinate();
				}
			}
		}
	}

	return QVariant();
}

QVariant RangeModel::headerData( int section, Qt::Orientation orientation, int role ) const
{
	if( orientation == Qt::Horizontal && role == Qt::DisplayRole )
	{
		switch( section )
		{
			case 0:
				return "X";
		}

	}

	return BaseModel::headerData( section, orientation, role );
}

void RangeModel::onAddNewKnot( KnotItem* knotItem, bool emitKnotAdded )
{
	beginInsertRows( QModelIndex(), m_plotItems.count(), m_plotItems.count() );
	m_plotItems.append( knotItem );
	endInsertRows();

	if( emitKnotAdded )
		emit itemAdded( knotItem );
}


bool RangeModel::setData( const QModelIndex& index, const QVariant& value, int role )
{
	/*if( role == Qt::EditRole )
	{
		KnotItem* knotItem = itemFromIndexAs<KnotItem>( index );
		const int column = index.column();

		bool isOk;
		const double valueInDouble = value.toDouble( &isOk );

		if( !isOk )
			return false;

		switch( column )
		{
		case 0:
			{
				knotItem->setCoordinate( valueInDouble );
				knotItem->plot()->replot();
				return true;
			}
		default:
			return false;
		}
	}*/
	
	return QAbstractItemModel::setData( index, value, role );
}