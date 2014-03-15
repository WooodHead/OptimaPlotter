#include "stdafx.h"
#include "splineknot.h"
#include "gasettings.h"

#include <qmath.h>

SplineKnot::SplineKnot()
{

}

SplineKnot::SplineKnot( double coordinate ) : m_coordinate( coordinate )
{

}

SplineKnot::~SplineKnot()
{

}

double SplineKnot::coordinate() const
{
	return m_coordinate;
}

void SplineKnot::setCoordinate( double coordinate )
{
	m_coordinate = coordinate;
}

bool SplineKnot::operator == ( const SplineKnot& other ) const
{
	return qAbs( m_coordinate - other.m_coordinate ) < GASettings::instance()->xTolerance();
}

bool SplineKnot::operator < ( const SplineKnot& other ) const
{
	if( *this == other )
		return false;

	return m_coordinate < other.m_coordinate;
}

bool SplineKnot::operator <= ( const SplineKnot& other ) const
{
	if( *this == other )
		return true;

	return m_coordinate <= other.m_coordinate;
}