#ifndef ALGORITHMBASE_H
#define ALGORITHMBASE_H

#include "ialgorithm.h"

#include <qobject.h>
#include <qmap.h>
#include <qstring.h>
#include <qvariant.h>

class QTranslator;

typedef QMap<QString, QVariant> PROPERTYMAP;

class AlgorithmBase : public QObject, public IAlgorithm
{
	Q_OBJECT

public:
	AlgorithmBase( QObject* parent = 0 );
	virtual ~AlgorithmBase();

	virtual Globals::AlgorithmFlags flags() const;

	virtual void setPropertyValueByTagName( const QString& tagName, const QVariant& value );
	virtual void getPropertyValueByTagName( const QString& tagName, QVariant& value, bool& ok ) const;

	virtual void initWithMarkers( const QVector<QPointF>& points );
	virtual void initWithKnots( const QVector<double>& knots );
	virtual void initWithRange( double left, double right );

	virtual void outputSamples( QVector<QPointF>& points ) const;
	virtual void outputKnots( QVector<double>& knots ) const;

	virtual QString translatorPath( int language ) const = 0;
	virtual void applyLanguage( int language );

protected:
	PROPERTYMAP m_propertyMap;
	QVector<QPointF> m_markers;
	QVector<double> m_knots;
	QVector<QPointF> m_samples;
	QTranslator* m_translator;
	double m_rangeLeftBorder;
	double m_rangeRightBorder;
};

#endif //ALGORITHMBASE_H