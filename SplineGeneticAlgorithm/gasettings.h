#ifndef GA_SETTINGS
#define GA_SETTINGS

class GASettings
{
public:
	~GASettings();
	static GASettings* instance();

	void setXTolerance( double xTolerance );
	double xTolerance() const;

	void setYTolerance( double yTolerance );
	double yTolerance() const;

	int numberOfKnots() const;

	void setSplineDegree( int splineDegree );
	int splineDegree() const;

	void setPopulationSize( int populationSize );
	int populationSize() const;

	void setNumberOfGenerations( int numberOfGenerations );
	int numberOfGenerations() const;

	void setNumberOfIntervals( int numberOfIntervals );
	int numberOfIntervals() const;

private:
	GASettings();
	static GASettings* s_instance;

	double m_xTolerance;
	double m_yTolerance;
	int m_numberOfntervals;
	int m_splineDegree;
	int m_populationSize;
	int m_numberOfGenerations;
};

#endif