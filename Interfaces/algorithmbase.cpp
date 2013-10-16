#include "stdafx.h"
#include "algorithmbase.h"

#include <qtranslator.h>

AlgorithmBase::AlgorithmBase( QObject* parent ) : QObject( parent ), IAlgorithm()
{
	m_translator = new QTranslator( this );
}

AlgorithmBase::~AlgorithmBase()
{

}

void AlgorithmBase::setPropertyValueByTagName( const QString& tagName, const QVariant& value )
{
	m_propertyMap.insert( tagName, value );
}

void AlgorithmBase::getPropertyValueByTagName( const QString& tagName, QVariant& value, bool& ok ) const
{
	if( m_propertyMap.contains( tagName ) )
	{
		value = m_propertyMap[ tagName ];
		ok = true;
		return;
	}

	ok = false;
}

void AlgorithmBase::initWithMarkers( const QVector<QPointF>& points )
{
	m_markers = points;
}

void AlgorithmBase::output( QVector<QPointF>& points ) const
{
	points = m_samples;
}