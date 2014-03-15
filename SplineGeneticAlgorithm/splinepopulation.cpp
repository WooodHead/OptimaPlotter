#include "stdafx.h"
#include "splinepopulation.h"
#include "splinedna.h"

SplinePopulation::SplinePopulation( const QVector<QPointF>& markers ): m_markers( markers )
{

}

SplinePopulation::~SplinePopulation()
{

}

int SplinePopulation::numberOfIndividuals() const
{
	return m_listOfIndividuals.count();
}
	
void SplinePopulation::addIndividualToPopulation( SplineDNA& individual )
{
	individual.computeFitness();
	individual.buildIntervals();
	m_listOfIndividuals.append( individual );
}

SplineDNA& SplinePopulation::individualAtPosition( int position )
{
	return m_listOfIndividuals[position];
}
	
const SplineDNA& SplinePopulation::individualAtPosition( int position ) const
{
	return m_listOfIndividuals[position];
}

const QList<SplineDNA>& SplinePopulation::listOfIndividuals() const
{
	return m_listOfIndividuals;
}

void SplinePopulation::sortIndividualsOnFitness()
{
	qSort( m_listOfIndividuals.begin(), m_listOfIndividuals.end() );
}

void SplinePopulation::clear()
{
	m_listOfIndividuals.clear();
}