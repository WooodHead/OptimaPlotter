#ifndef SPLINE_POPULATION
#define SPLINE_POPULATION

#include <qlist.h>
#include <qvector.h>
#include <qpoint.h>

class SplineDNA;

class SplinePopulation
{
public:
	SplinePopulation( const QVector<QPointF>& markers );
	~SplinePopulation();

	int numberOfIndividuals() const;
	void addIndividualToPopulation( SplineDNA& individual );
	SplineDNA& individualAtPosition( int position );
	const SplineDNA& individualAtPosition( int position ) const;
	void sortIndividualsOnFitness();
	const QList<SplineDNA>& listOfIndividuals() const;
	void clear();

private:
	QList<SplineDNA> m_listOfIndividuals;
	const QVector<QPointF>& m_markers;
};

#endif