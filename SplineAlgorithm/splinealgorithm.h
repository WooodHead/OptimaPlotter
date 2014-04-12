#ifndef SPLINEALGORITHM_H
#define SPLINEALGORITHM_H

#include "ialgorithm.h"
#include "algorithmbase.h"
#include "ui_splinesettingsform.h"

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
	virtual void applySettings();

	virtual QString translatorPath( int language ) const;
	virtual void retranslateUi();

	double splineHelperFunction( double base, double argument, int power ) const;

private:
	Ui::SettingsWidgetForm m_settingsWidgetForm;
};

#endif // SPLINEALGORITHM_H
