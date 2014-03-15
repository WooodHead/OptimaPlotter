#ifndef SPLINE_DNA
#define SPLINE_DNA

#include <qlist.h>

#include "splineinterval.h"
#include "splineknot.h"
#include <qpoint.h>

class SplineDNA
{
public:
	SplineDNA( const QVector<QPointF>* markers );
	SplineDNA( const SplineDNA& other );
	~SplineDNA();

	SplineDNA& operator = ( const SplineDNA& other );

	double computeFitness();
	void dumpFitness();

	void addInterval( const SplineInterval& interval );
	void addKnot( const SplineKnot& knot );
	void buildIntervals();

	SplineInterval& intervalAtPosition( int position );
	const SplineInterval& intervalAtPosition( int position ) const;

	SplineInterval& leftMostInterval();
	const SplineInterval& leftMostInteval() const;

	SplineInterval& rightMostInterval();
	const SplineInterval& rightMostInterval() const;

	SplineKnot& splineKnotAtPosition( int position );
	const SplineKnot& splineKnotAtPosition( int position ) const;

	int numberOfIntervals() const;
	int numberOfKnots() const;

	void setFitnessRecalculationNeeded();

	const QList<SplineKnot>& listOfKnots() const;
	const QList<double>& listOfSplineCoefs() const;
	const QList<QList<double> >& listOfLambdas() const;
	const QList<SplineInterval>& listOfIntervals() const;
	bool isFitnessRecalculationNeeded() const;

	bool operator <( const SplineDNA& other ) const;

	void sortIntervalsOnFitness();

	friend SplineDNA crossOver( const SplineDNA& dna1, const SplineDNA& dna2 );
	void mutate();

private:
	bool duplicatesExist() const;

private:
	QList<SplineInterval> m_listOfIntervals;
	QList<SplineKnot> m_listOfKnots;
	QList<double> m_listOfSplineCoefs;
	QList<QList<double> > m_listOfLambdas; 
	const QVector<QPointF>* m_markers;

	double m_fitness;
	mutable bool m_isFitnessRecalculationNeeded;
};

#endif