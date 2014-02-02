#ifndef DOUBLEPROPERTYDELEGATE_H
#define DOUBLEPROPERTYDELEGATE_H

#include "qitemdelegate.h"
#include "qstyleditemdelegate.h"

class DoublePropertyDelegate: public QStyledItemDelegate
{
	Q_OBJECT
public:
	DoublePropertyDelegate( QObject* parent = 0 );
	virtual ~DoublePropertyDelegate();

	virtual QWidget* createEditor( QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index ) const;
	virtual void setEditorData( QWidget* editor, const QModelIndex& index ) const;
	virtual void setModelData( QWidget* editor, QAbstractItemModel* model, const QModelIndex& index ) const;
};

#endif