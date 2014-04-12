#ifndef SPLINEGA_H
#define SPLINEGA_H

#include "ialgorithm.h"
#include "algorithmbase.h"

#include "splinepopulation.h"
#include "ui_splinegasettingsform.h"


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
	virtual void applySettings();

	virtual QString translatorPath( int language ) const;
	virtual void retranslateUi();

	static double splineHelperFunction( double base, double argument, int power );

private:
	void createFirstPopulation();
	void executeGA();

private:
	Ui::SettingsWidgetForm m_settingsWidgetForm;
	SplinePopulation m_currentPopulation;
};

#endif // SPLINEGA_H