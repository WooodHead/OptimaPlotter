#ifndef IALGORITHM_H
#define IALGORITHM_H

#include "globals.h"

#include <qpoint.h>
#include <qvector.h>
#include <qstring.h>

#include <QtPlugin>

class IAlgorithm
{
public:
	virtual ~IAlgorithm() {}
	virtual void initWithMarkers( const QVector<QPointF>& points ) = 0;
	virtual void initWithKnots( const QVector<double>& knots ) = 0;
	virtual void initWithRange( double left, double right ) = 0;

	virtual void outputSamples( QVector<QPointF>& points ) const = 0;
	virtual void outputKnots( QVector<double>& knots ) const = 0;

	virtual void evaluate() = 0;

	virtual QString name() const = 0;
	virtual QString tagName() const = 0;
	virtual Globals::AlgorithmFlags flags() const = 0;

	virtual void applyLanguage( int language ) = 0;

	virtual void setPropertyValueByTagName( const QString& tagName, const QVariant& value ) = 0;
	virtual void getPropertyValueByTagName( const QString& tagName, QVariant& value, bool& ok ) const = 0;
};

Q_DECLARE_INTERFACE( IAlgorithm, "com.OptimaPlotter.AlgorithmInterface/1.0" );

#endif //IALGORITHM_H