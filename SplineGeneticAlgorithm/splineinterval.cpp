#include "stdafx.h"
#include "splineinterval.h"
#include "splinedna.h"
#include "splinegeneticalgorithm.h"
#include "gasettings.h"

#include <qmath.h>

SplineInterval::SplineInterval( SplineDNA* splineDNA, const QVector<QPointF>* markers ) : 
m_markers( markers ), m_splineDNA( splineDNA ), m_fitness( 0 )
{

}

SplineInterval::~SplineInterval()
{

}

double SplineInterval::computeFitness()
{
	if( m_splineDNA->isFitnessRecalculationNeeded() )
		m_splineDNA->computeFitness();

	m_fitness = 0.0;
	const int splineDegree = 3;

	const QList<SplineKnot>& listOfKnots = m_splineDNA->listOfKnots();
	const QList<double>& listOfSplineCoefs = m_splineDNA->listOfSplineCoefs();
	const QList<QList<double> >& listOfLambdas = m_splineDNA->listOfLambdas();
	m_markersInInterval.clear();

	foreach( QPointF point, *m_markers )
	{
		if( point.x() >= m_leftBorder.coordinate() &&
			point.x() <= m_rightBorder.coordinate() )
			m_markersInInterval.append( point );
	}

	for( int i = 0; i < m_markersInInterval.count(); ++i )
	{
		double x = m_markersInInterval.at( i ).x();
		double y = 0;

		for( int j = 0; j < listOfSplineCoefs.count(); ++j )
		{
			double sum = 0;
			for( int k = 0; k <= splineDegree + 1; ++k )
			{
				sum += listOfLambdas.at( j ).at( k ) * 
					SplineGA::splineHelperFunction( listOfKnots.at( k + j ).coordinate(), x, splineDegree );
			}

			double coef = listOfSplineCoefs.at( j );
			y += sum * coef;
		}

		m_fitness += qPow( m_markersInInterval.at( i ).y() - y, 2 );
	}

	
	return m_fitness;
}

const SplineKnot& SplineInterval::leftBorder() const
{
	return m_leftBorder;
}
	
const SplineKnot& SplineInterval::rightBorder() const
{
	return m_rightBorder;
}
	
void SplineInterval::setLeftBorder( const SplineKnot& knot )
{
	m_leftBorder = knot;
}
	
void SplineInterval::setRightBorder( const SplineKnot& knot )
{
	m_rightBorder = knot;
}

bool SplineInterval::operator <( const SplineInterval& other ) const
{
	const int countOfMarkersInInterval = m_markersInInterval.count();
	const int countOfMarkersInOtherInterval = other.m_markersInInterval.count();

	if( countOfMarkersInInterval == 0 )
		return false;

	if( countOfMarkersInOtherInterval == 0 )
		return true;

	const double realFitnessOfInterval = m_fitness/ countOfMarkersInInterval;
	const double realFitnessOfOtherInterval = m_fitness/ countOfMarkersInOtherInterval;

	if( qAbs( realFitnessOfInterval - realFitnessOfOtherInterval ) < GASettings::instance()->yTolerance() )
	{
		return countOfMarkersInInterval > countOfMarkersInOtherInterval;
	}
	else
	{
		return realFitnessOfInterval < realFitnessOfOtherInterval;
	}
}

bool doIntervalsOverlap( const SplineInterval& interval1, const SplineInterval& interval2 )
{
	if( interval1.rightBorder() <= interval2.leftBorder() )
		return false;

	if( interval2.rightBorder() <= interval1.leftBorder() )
		return false;

	return true;
}