#ifndef MARKERMODEL_H
#define MARKERMODEL_H

#include "markeritem.h"
#include "basemodel.h"

class MarkerModel: public BaseModel
{
	Q_OBJECT
public:
	MarkerModel( QObject* parent = 0 );
	virtual ~MarkerModel();

	virtual int	columnCount( const QModelIndex& parent = QModelIndex() ) const;
	virtual QVariant data( const QModelIndex& index, int role = Qt::DisplayRole ) const;
	virtual QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
	virtual bool setData( const QModelIndex& index, const QVariant& value, int role = Qt::EditRole );

public slots:
	void onAddNewMarker( MarkerItem* markerItem, bool emitMarkerAdded = true );
};

#endif