#ifndef KNOTMODEL_H
#define KNOTMODEL_H

#include "knotitem.h"
#include "basemodel.h"

class KnotModel: public BaseModel
{
	Q_OBJECT
public:
	KnotModel( QObject* parent = 0 );
	virtual ~KnotModel();

	virtual int	columnCount( const QModelIndex& parent = QModelIndex() ) const;
	virtual QVariant data( const QModelIndex& index, int role = Qt::DisplayRole ) const;
	virtual QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
	virtual bool setData( const QModelIndex& index, const QVariant& value, int role = Qt::EditRole );

public slots:
	void onAddNewKnot( KnotItem* knotItem, bool emitKnotAdded = true );
};

#endif