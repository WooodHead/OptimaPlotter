#include "stdafx.h"
#include "splinegeneticalgorithm.h"
#include "globals.h"

#include <qmath.h>
#include <qtranslator.h>
#include <qapplication.h>

using namespace Eigen;

bool lessThanForTwoPointsOnXAxis( const QPointF& p1, const QPointF& p2 )
{
	return p1.x() < p2.x();
}

SplineGA::SplineGA()
{

}

SplineGA::~SplineGA()
{

}

void SplineGA::evaluate()
{
	
}

QString SplineGA::name() const
{
	return tr( "Spline GA" );
}

QString SplineGA::tagName() const
{
	return "splinega";
}

QString SplineGA::translatorPath( int language ) const
{
	switch( language )
	{
	case ( int )Globals::LANG_HY:
		{
			return ":/splinegeneticalgorithm/splinegeneticalgorithm_hy.qm";
		}
	default:
		return QString();
	}
}

double SplineGA::splineHelperFunction( double base, double argument, int power ) const
{
	double valueToRaiseToPower = ( base - argument ) > 0 ? ( base - argument ) : 0;
	return qPow( valueToRaiseToPower, power );
}

Globals::AlgorithmFlags SplineGA::flags() const
{
	Globals::AlgorithmFlags flags = AlgorithmBase::flags();
	return flags | Globals::ALGO_FLAG_RANGE_PICKER;
}

Q_EXPORT_PLUGIN2( splinegeneticalgorithm, SplineGA );