#include "stdafx.h"
#include "gasettings.h"

GASettings* GASettings::s_instance = 0;

GASettings::GASettings()
{
	m_populationSize = 10;
	m_numberOfGenerations = 50;
}

GASettings::~GASettings()
{

}

GASettings* GASettings::instance()
{
	if( !s_instance )
		s_instance = new GASettings();

	return s_instance;
}

void GASettings::setXTolerance( double xTolerance )
{
	m_xTolerance = xTolerance;
}

double GASettings::xTolerance() const
{
	return m_xTolerance;
}

void GASettings::setYTolerance( double yTolerance )
{
	m_yTolerance = yTolerance;
}

double GASettings::yTolerance() const
{
	return m_yTolerance;
}

void GASettings::setNumberOfIntervals( int numberOfIntervals )
{
	m_numberOfntervals = numberOfIntervals;
}

int GASettings::numberOfIntervals() const
{
	return m_numberOfntervals;
}
	
int GASettings::numberOfKnots() const
{
	return 4 * m_numberOfntervals - 2; //TODO: change for each degree of spline
}

void GASettings::setSplineDegree( int splineDegree )
{
	m_splineDegree = splineDegree;
}

int GASettings::splineDegree() const
{
	return m_splineDegree;
}

void GASettings::setPopulationSize( int populationSize )
{
	m_populationSize = populationSize;
}
	
int GASettings::populationSize() const
{
	return m_populationSize;
}

void GASettings::setNumberOfGenerations( int numberOfGenerations )
{
	m_numberOfGenerations = numberOfGenerations;
}
	
int GASettings::numberOfGenerations() const
{
	return m_numberOfGenerations;
}