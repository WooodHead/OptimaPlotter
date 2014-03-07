#ifndef RANGEMODEL_H
#define RANGEMODEL_H

#include "knotitem.h"
#include "basemodel.h"

class RangeModel: public BaseModel
{
	Q_OBJECT
public:
	RangeModel( QObject* parent = 0 );
	virtual ~RangeModel();

	virtual int	columnCount( const QModelIndex& parent = QModelIndex() ) const;
	virtual QVariant data( const QModelIndex& index, int role = Qt::DisplayRole ) const;
	virtual QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
	virtual bool setData( const QModelIndex& index, const QVariant& value, int role = Qt::EditRole );

public slots:
	void onAddNewKnot( KnotItem* knotItem, bool emitKnotAdded = true );
};

#endif