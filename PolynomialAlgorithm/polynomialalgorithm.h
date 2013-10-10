#ifndef POLYNOMIALALGORITHM_H
#define POLYNOMIALALGORITHM_H

#include "ialgorithm.h"
#include "algorithmbase.h"

class PolynomialAlgorithm : public AlgorithmBase
{
	Q_OBJECT
    Q_INTERFACES( IAlgorithm )
public:
	PolynomialAlgorithm();
	virtual ~PolynomialAlgorithm();

	virtual void evaluate();
};

#endif // POLYNOMIALALGORITHM_H
