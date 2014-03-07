#include "stdafx.h"
#include "algorithmbase.h"
#include "globals.h"

#include <qtranslator.h>
#include <qapplication.h>

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

void AlgorithmBase::initWithKnots( const QVector<double>& knots )
{
	m_knots = knots;
}

void AlgorithmBase::initWithRange( double left, double right )
{
	m_rangeLeftBorder = left;
	m_rangeRightBorder = right;
}

void AlgorithmBase::outputSamples( QVector<QPointF>& points ) const
{
	points = m_samples;
}

void AlgorithmBase::outputKnots( QVector<double>& knots ) const
{
	knots = m_knots;
}

void AlgorithmBase::applyLanguage( int language )
{
	switch( language )
	{
	case ( int )Globals::LANG_EN:
		{
			QApplication::removeTranslator( m_translator );
			break;
		}
	default:
		{
			m_translator->load( translatorPath( language ) );
			QApplication::installTranslator( m_translator );
			break;
		}
	}
}

Globals::AlgorithmFlags AlgorithmBase::flags() const
{
	return Globals::ALGO_FLAG_POINT_PICKER;
}

