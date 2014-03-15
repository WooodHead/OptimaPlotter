#ifndef SPLINE_KNOT
#define SPLINE_KNOT

class SplineKnot
{
public:
	SplineKnot();
	SplineKnot( double coordinate );
	~SplineKnot();

	double coordinate() const;
	void setCoordinate( double coordinate );

	bool operator == ( const SplineKnot& other ) const;
	bool operator < ( const SplineKnot& other ) const;
	bool operator <= ( const SplineKnot& other ) const;

private:
	double m_coordinate;
};

#endif