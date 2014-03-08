#ifndef SPLINEGA_H
#define SPLINEGA_H

#include "ialgorithm.h"
#include "algorithmbase.h"

class SplineGA : public AlgorithmBase
{
	Q_OBJECT
    Q_INTERFACES( IAlgorithm )
public:
	SplineGA();
	virtual ~SplineGA();

	virtual void evaluate();
	virtual QString name() const;
	virtual QString tagName() const;
	virtual Globals::AlgorithmFlags flags() const;

	virtual QString translatorPath( int language ) const;

	double splineHelperFunction( double base, double argument, int power ) const;
};

#endif // SPLINEGA_H