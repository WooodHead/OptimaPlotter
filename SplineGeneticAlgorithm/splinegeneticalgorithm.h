#ifndef SPLINEGA_H
#define SPLINEGA_H

#include "ialgorithm.h"
#include "algorithmbase.h"

#include "splinepopulation.h"

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

	static double splineHelperFunction( double base, double argument, int power );

private:
	void createFirstPopulation();
	void executeGA();

private:
	SplinePopulation m_currentPopulation;
};

#endif // SPLINEGA_H