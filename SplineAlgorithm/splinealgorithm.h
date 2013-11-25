#ifndef POLYNOMIALALGORITHM_H
#define POLYNOMIALALGORITHM_H

#include "ialgorithm.h"
#include "algorithmbase.h"

class SplineAlgorithm : public AlgorithmBase
{
	Q_OBJECT
    Q_INTERFACES( IAlgorithm )
public:
	SplineAlgorithm();
	virtual ~SplineAlgorithm();

	virtual void evaluate();
	virtual QString name() const;
	virtual QString tagName() const;
	virtual Globals::AlgorithmFlags flags() const;

	virtual QString translatorPath( int language ) const;

	double splineHelperFunction( double base, double argument, int power ) const;
};

#endif // POLYNOMIALALGORITHM_H
