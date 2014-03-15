#ifndef SPLINE_INTERVAL
#define SPLINE_INTERVAL

#include "splineknot.h"

#include <qpoint.h>
#include <qvector.h>

class SplineDNA;

class SplineInterval
{
public:
	SplineInterval( SplineDNA* splineDNA, const QVector<QPointF>* markers );
	~SplineInterval();

	double computeFitness();

	const SplineKnot& leftBorder() const;
	const SplineKnot& rightBorder() const;
	
	void setLeftBorder( const SplineKnot& knot );
	void setRightBorder( const SplineKnot& knot );

	bool operator <( const SplineInterval& other ) const;

	friend bool doIntervalsOverlap( const SplineInterval& interval1, const SplineInterval& interval2 );
	friend class SplineDNA;

private:
	const QVector<QPointF>* m_markers;
	QList<QPointF> m_markersInInterval;
	SplineDNA* m_splineDNA;
	SplineKnot m_leftBorder;
	SplineKnot m_rightBorder;

	double m_fitness;
};

#endif