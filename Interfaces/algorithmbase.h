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

	virtual void setPropertyValueByTagName( const QString& tagName, const QVariant& value );
	virtual void getPropertyValueByTagName( const QString& tagName, QVariant& value, bool& ok ) const;

	virtual void initWithMarkers( const QVector<QPointF>& points );
	virtual void output( QVector<QPointF>& points ) const;

protected:
	PROPERTYMAP m_propertyMap;
	QVector<QPointF> m_markers;
	QVector<QPointF> m_samples;
	QTranslator* m_translator;
};

#endif //ALGORITHMBASE_H