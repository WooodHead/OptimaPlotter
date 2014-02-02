#include "stdafx.h"
#include "doublepropertydelegate.h"
#include "knotitem.h"
#include "markeritem.h"
#include "basemodel.h"
#include "globals.h"

#include "qwt_plot.h"

#include <float.h>

#include <qspinbox.h>

DoublePropertyDelegate::DoublePropertyDelegate( QObject* parent ) : QStyledItemDelegate( parent )
{

}

DoublePropertyDelegate::~DoublePropertyDelegate()
{

}

QWidget* DoublePropertyDelegate::createEditor( QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index ) const
{
	QDoubleSpinBox* spinBox = new QDoubleSpinBox( parent );
	spinBox->setDecimals( 5 );
	spinBox->setMinimum( -DBL_MAX );
	spinBox->setMaximum( DBL_MAX );
	return spinBox;
}
	
void DoublePropertyDelegate::setEditorData( QWidget* editor, const QModelIndex& index ) const
{
	QDoubleSpinBox* spinBox = dynamic_cast<QDoubleSpinBox*>( editor );
	const BaseModel* model = dynamic_cast<const BaseModel*>( index.model() );
	const QwtPlotItem* plotItem = model->itemFromIndexAs<QwtPlotItem>( index );

	switch( plotItem->rtti() )
	{
	case Globals::Rtti_PlotMarker:
		{
			const MarkerItem* markerItem = dynamic_cast<const MarkerItem*>( plotItem );
			if( index.column() == 0 )
				spinBox->setValue( markerItem->xValue() );
			else if( index.column() == 1 )
				spinBox->setValue( markerItem->yValue() );
			return;
		}
	case Globals::Rtti_PlotKnot:
		{
			const KnotItem* knotItem = dynamic_cast<const KnotItem*>( plotItem );
			if( index.column() == 0 )
				spinBox->setValue( knotItem->coordinate() );
			return;
		}
	default:
		return;
	}
}

void DoublePropertyDelegate::setModelData( QWidget* editor, QAbstractItemModel* model, const QModelIndex& index ) const
{
	QDoubleSpinBox* spinBox = dynamic_cast<QDoubleSpinBox*>( editor );
	BaseModel* baseModel = dynamic_cast<BaseModel*>( model );
	QwtPlotItem* plotItem = baseModel->itemFromIndexAs<QwtPlotItem>( index );

	switch( plotItem->rtti() )
	{
	case Globals::Rtti_PlotMarker:
		{
			MarkerItem* markerItem = dynamic_cast<MarkerItem*>( plotItem );
			if( index.column() == 0 )
				markerItem->setXValue( spinBox->value() );
			else if( index.column() == 1 )
				markerItem->setYValue( spinBox->value() );

			markerItem->plot()->replot();
			return;
		}
	case Globals::Rtti_PlotKnot:
		{
			KnotItem* knotItem = dynamic_cast<KnotItem*>( plotItem );
			if( index.column() == 0 )
				knotItem->setCoordinate( spinBox->value() );

			knotItem->plot()->replot();
			return;
		}
	default:
		return;
	}
}