#include "stdafx.h"
#include "splinedna.h"
#include "splinegeneticalgorithm.h"
#include "gasettings.h"

#include "qvector.h"
#include "qmath.h"
#include "qdebug.h"
#include "qnumeric.h"

#include <iostream>

using namespace Eigen;

SplineDNA::SplineDNA( const QVector<QPointF>* markers ) : m_isFitnessRecalculationNeeded( true ), m_markers( markers )
{
	m_fitness = 0.0;
}

SplineDNA::~SplineDNA()
{

}

void SplineDNA::dumpFitness()
{
	std::cout << std::endl << "Spline Fitness:  " << computeFitness() << std::endl;
	for( int i = 0; i < m_listOfIntervals.count(); ++i )
	{
		SplineInterval intreval = m_listOfIntervals.at( i );
		std::cout << QString( "Interval %1 Fitness" ).arg( i ).toStdString() << intreval.computeFitness() << std::endl;
	}
}

double SplineDNA::computeFitness()
{
	if( !m_isFitnessRecalculationNeeded )
		return m_fitness;
	m_fitness = 0.0;

	const int samplesCount = m_listOfKnots.count();
	const int splineDegree = 3;
	
	// Fitness calculation
	qSort( m_listOfKnots.begin(), m_listOfKnots.end() );
	//TODO: remove dubpicates!

	const double step = qAbs( m_markers->first().x() -  m_markers->last().x() ) / ( samplesCount - 1 );

	const int columnCount = m_listOfKnots.count() - 1 - splineDegree;
	if( columnCount < 1 )
		return 1000000000000; // TODO: Beautify!

	const int rowCount = m_markers->size();

	MatrixXf matrixA( rowCount, columnCount );
	MatrixXf matrixAPlus( columnCount, rowCount );
	VectorXf vectorB( rowCount );
	VectorXf result( columnCount );

	int row = 0;
	foreach( QPointF point, *m_markers )
	{
		vectorB( row ) = point.y();
		++row;
	}

	m_listOfLambdas.clear();
	for( int column = 0; column < columnCount; ++column )
	{
		QList<double> lambdas;
		for( int i = 0; i <= splineDegree + 1; ++i )
		{
			double lambda = 1.0;
			for( int j = 0; j <= splineDegree + 1; ++j )
			{
				if( i == j )
					continue;

				lambda /= ( m_listOfKnots.at( i + column ).coordinate() - m_listOfKnots.at( j + column ).coordinate() );
			}
			if( qIsNaN( lambda ) )
				int breakpoint = 0;
			lambdas.append( lambda );
		}

		m_listOfLambdas.append( lambdas );
	}

	for( int column = 0; column < columnCount; ++column )
	{
		for( int row = 0; row < rowCount; ++row )
		{
			double sum = 0.0;
			for( int j = 0; j <= splineDegree + 1; ++j )
			{
				double lambda = m_listOfLambdas.at( column ).at( j );
				double markerXCoordinate = m_markers->at( row ).x();
				double knotCoordinate = m_listOfKnots.at( j + column ).coordinate();
				double splineHelperValue = SplineGA::splineHelperFunction( knotCoordinate, markerXCoordinate, splineDegree );
				sum += lambda * splineHelperValue;
			}
			if( qIsNaN( sum ) )
				int breakpoint = 0;
			if( qAbs( sum ) < 1.0e-06 || qIsNaN( sum ) )
				sum = 0;

			matrixA( row, column ) = sum;
		}
	}

	JacobiSVD<MatrixXf> svd( matrixA, ComputeFullU | ComputeFullV );

	MatrixXf matrixSPlus = MatrixXf::Zero( columnCount, rowCount );
	MatrixXf matrixS = MatrixXf::Zero( rowCount, columnCount );

	const JacobiSVD<MatrixXf>::SingularValuesType& singularValues = svd.singularValues();
	int size = singularValues.size();
	for( int i = 0; i < singularValues.size(); i++ )
	{
		float value = svd.singularValues()[i];
		if( value == 0.0 )
			int breakpoint = 0;
		matrixS( i, i ) = value;
		if( value ) //TODO: is this check needed?
		{
			if( qIsNaN( value ) )
				int breakpoint = 0;
			matrixSPlus( i, i ) = 1.0 / value;
		}
	}

	matrixAPlus = svd.matrixV() * matrixSPlus * svd.matrixU().transpose();
	result = matrixAPlus * vectorB;

	m_listOfSplineCoefs.clear();
	for( int i = 0; i < columnCount; ++i )
	{
		m_listOfSplineCoefs.append( result( i ) );
	}

	/*std::cout << std::endl << "Result vector\n" << result << std::endl;
	std::cout << std::endl << "Matrix A\n" << matrixA << std::endl;
	std::cout << std::endl << "Matrix A Reconstructed\n" << svd.matrixU() * matrixS *  svd.matrixV().transpose() << std::endl;
	std::cout << std::endl << "Matrix A+\n" << matrixAPlus << std::endl;*/

	for( int i = 0; i < m_markers->count(); ++i )
	{
		double x = m_markers->at( i ).x();
		double y = 0;
		
		for( int j = 0; j < columnCount; ++j )
		{
			double sum = 0;
			for( int k = 0; k <= splineDegree + 1; ++k )
			{
				sum += m_listOfLambdas.at( j ).at( k ) * 
					SplineGA::splineHelperFunction( m_listOfKnots.at( k + j ).coordinate(), x, splineDegree );
			}

			double coef = result( j );
			y += sum * coef;
		}

		m_fitness += qPow( m_markers->at( i ).y() - y, 2 );
	}

	m_isFitnessRecalculationNeeded = false;
	if( qIsNaN( m_fitness ) )
		int breakpoint = 0;
	return m_fitness;
}

void SplineDNA::addInterval( const SplineInterval& interval )
{
	const SplineKnot& leftBorder = interval.leftBorder();
	const SplineKnot& rightBorder = interval.rightBorder();

	if( m_listOfKnots.count( leftBorder ) == 0 )
		m_listOfKnots.append( leftBorder );

	// TODO: find a better solution, this is necessary because operator == in SplineKnot is not associative
	//		 and QT's algorithms fail.
	if( duplicatesExist() ) 
		m_listOfKnots.removeLast();

	if( m_listOfKnots.count( rightBorder ) == 0 );
		m_listOfKnots.append( rightBorder );

	// TODO: find a better solution, this is necessary because operator == in SplineKnot is not associative
	//		 and QT's algorithms fail.
	if( duplicatesExist() )
		m_listOfKnots.removeLast();

	m_isFitnessRecalculationNeeded = true;
}
	
void SplineDNA::addKnot( const SplineKnot& knot )
{
	if( !m_listOfKnots.contains( knot ) )
	{
		m_listOfKnots.append( knot );
		m_isFitnessRecalculationNeeded = true;
	}

	// TODO: find a better solution, this is necessary because operator == in SplineKnot is not associative
	//		 and QT's algorithms fail.
	if( duplicatesExist() )
	{
		m_listOfKnots.removeLast();
		m_isFitnessRecalculationNeeded = false;
	}
}

void SplineDNA::buildIntervals()
{
	m_listOfIntervals.clear();
	qSort( m_listOfKnots.begin(), m_listOfKnots.end() );

	for( int i = 1; i < m_listOfKnots.count(); ++i )
	{
		const SplineKnot& previousKnot = m_listOfKnots.at( i - 1 );
		const SplineKnot& currentKnot = m_listOfKnots.at( i );

		SplineInterval interval( this, m_markers );
		interval.setLeftBorder( previousKnot );
		interval.setRightBorder( currentKnot );
		interval.computeFitness();
		m_listOfIntervals.append( interval );
	}
}

SplineInterval& SplineDNA::intervalAtPosition( int position )
{
	return m_listOfIntervals[position];
}

const SplineInterval& SplineDNA::intervalAtPosition( int position ) const
{
	return m_listOfIntervals[position];
}

SplineInterval& SplineDNA::leftMostInterval()
{
	return m_listOfIntervals.first();
}

const SplineInterval& SplineDNA::leftMostInteval() const
{
	return m_listOfIntervals.first();
}

SplineInterval& SplineDNA::rightMostInterval()
{
	return m_listOfIntervals.last();
}

const SplineInterval& SplineDNA::rightMostInterval() const
{
	return m_listOfIntervals.last();
}

SplineKnot& SplineDNA::splineKnotAtPosition( int position )
{
	return m_listOfKnots[position];
}

const SplineKnot& SplineDNA::splineKnotAtPosition( int position ) const
{
	return m_listOfKnots[position];
}

int SplineDNA::numberOfIntervals() const
{
	return m_listOfIntervals.count();
}

int SplineDNA::numberOfKnots() const
{
	return m_listOfKnots.count();
}

void SplineDNA::setFitnessRecalculationNeeded()
{
	m_isFitnessRecalculationNeeded = true;
}

const QList<SplineKnot>& SplineDNA::listOfKnots() const
{
	return m_listOfKnots;
}
	
const QList<double>& SplineDNA::listOfSplineCoefs() const
{
	return m_listOfSplineCoefs;
}
	
const QList<QList<double> >& SplineDNA::listOfLambdas() const
{
	return m_listOfLambdas;
}
	
bool SplineDNA::isFitnessRecalculationNeeded() const
{
	return m_isFitnessRecalculationNeeded;
}

const QList<SplineInterval>& SplineDNA::listOfIntervals() const
{
	return m_listOfIntervals;
}

bool SplineDNA::operator <( const SplineDNA& other ) const
{
	return m_fitness < other.m_fitness;
}

void SplineDNA::sortIntervalsOnFitness()
{
	qSort( m_listOfIntervals.begin(), m_listOfIntervals.end() );
}

SplineDNA crossOver( const SplineDNA& dna1, const SplineDNA& dna2 )
{
	SplineDNA splineDNA( dna1.m_markers );

	QList<SplineInterval> intervalsToAdd;
	QList<SplineInterval> intervalsOfDNA1 = dna1.m_listOfIntervals;
	QList<SplineInterval> intervalsOfDNA2 = dna2.m_listOfIntervals;

	SplineInterval firstIntervalOfDNA1 = intervalsOfDNA1.first();
	SplineInterval lastIntervalOfDNA1 = intervalsOfDNA1.last();

	SplineInterval firstIntervalOfDNA2 = intervalsOfDNA2.first();
	SplineInterval lastIntervalOfDNA2 = intervalsOfDNA2.last();

	intervalsOfDNA1.removeFirst();
	intervalsOfDNA1.removeLast();

	intervalsOfDNA2.removeFirst();
	intervalsOfDNA2.removeLast();

	if( firstIntervalOfDNA1 < firstIntervalOfDNA2 )
		intervalsToAdd.append( firstIntervalOfDNA1 );
	else
		intervalsToAdd.append( firstIntervalOfDNA2 );

	if( lastIntervalOfDNA1 < lastIntervalOfDNA2 && !doIntervalsOverlap( lastIntervalOfDNA1, intervalsToAdd.first() ) )
		intervalsToAdd.append( lastIntervalOfDNA1 );
	else
		intervalsToAdd.append( lastIntervalOfDNA2 );

	QList<SplineInterval> allIntervals;
	allIntervals.append( intervalsOfDNA1 );
	allIntervals.append( intervalsOfDNA2 );

	qSort( allIntervals.begin(), allIntervals.end() );

	int numberOfIntervalsToAdd = GASettings::instance()->numberOfIntervals();

	for( int j = 0; j < allIntervals.count(); ++j )
	{
		SplineInterval currentInterval = allIntervals.at( j );
		bool overlapFound = false;
		for( int k = 0; k < intervalsToAdd.count(); ++k )
		{
			if( doIntervalsOverlap( currentInterval, intervalsToAdd.at( k ) ) )
			{
				overlapFound = true;
				break;
			}
		}

		if( !overlapFound )
		{
			intervalsToAdd.append( currentInterval );
			allIntervals.removeAt( j-- );
			if( intervalsToAdd.count() == numberOfIntervalsToAdd )
				break;
		}
	}
	
	foreach( SplineInterval interval, intervalsToAdd )
	{
		splineDNA.addInterval( interval );
	}

	if( splineDNA.duplicatesExist() )
		int breakpoint = 0;

	splineDNA.buildIntervals();
	const QList<SplineInterval>& currentIntervalsInSpline = splineDNA.listOfIntervals();
	const int numberOfKnotsToAddPerInterval = 2; //TODO: get it from GA Settings

	QList<SplineKnot> allKnots = dna1.listOfKnots();
	allKnots.append( dna2.listOfKnots() );
	
	qSort( allKnots.begin(), allKnots.end() );
	QList<SplineKnot>::iterator iterator = std::unique( allKnots.begin(), allKnots.end() );
	allKnots.erase( iterator, allKnots.end() );

	for( int i = 0; i < currentIntervalsInSpline.count(); ++i )
	{
		SplineInterval intervalToAddKnots = currentIntervalsInSpline.at( i );
		bool isIntervalAddedByCrossover = false;
		for( int j = 0; j < intervalsToAdd.count(); ++j )
		{
			SplineInterval addedInterval = intervalsToAdd.at( j );
			if( addedInterval.leftBorder() == intervalToAddKnots.leftBorder() &&
				addedInterval.rightBorder() == intervalToAddKnots.rightBorder() )
			{
				isIntervalAddedByCrossover = true;
				break;
			}
		}

		if( isIntervalAddedByCrossover )
			continue;

		SplineKnot leftBorder = intervalToAddKnots.leftBorder();
		SplineKnot rightBorder = intervalToAddKnots.rightBorder();

		int indexToContinueSearch = 0;
		for( int j = 0; j < numberOfKnotsToAddPerInterval; ++j )
		{		
			bool found = false;

			for( int k = indexToContinueSearch; k < allKnots.count(); ++k )
			{
				SplineKnot splineKnot = allKnots.at( k );
				if( splineKnot < leftBorder )
					continue;
				if( leftBorder < splineKnot &&  splineKnot < rightBorder )
				{
					found = true;
					indexToContinueSearch = k + 1;
					splineDNA.addKnot( splineKnot );
					break;
				}
			}

			if( !found )
				break;
		}
	}

	if( splineDNA.duplicatesExist() )
		int breakpoint = 0;

	return splineDNA;
}

SplineDNA::SplineDNA( const SplineDNA& other )
{
	m_listOfIntervals = other.m_listOfIntervals;
	m_listOfKnots = other.m_listOfKnots;
	m_listOfSplineCoefs = other.m_listOfSplineCoefs;
	m_listOfLambdas = other.m_listOfLambdas;
	m_markers = other.m_markers;
	m_fitness = other.m_fitness;
	m_isFitnessRecalculationNeeded = other.m_isFitnessRecalculationNeeded;

	for( int i = 0; i < m_listOfIntervals.count(); ++i )
	{
		SplineInterval& splineInterval = m_listOfIntervals[i];
		splineInterval.m_splineDNA = this;
	}
}

SplineDNA& SplineDNA::operator = ( const SplineDNA& other )
{
	if( this == &other )
		return *this;

	m_listOfIntervals = other.m_listOfIntervals;
	m_listOfKnots = other.m_listOfKnots;
	m_listOfSplineCoefs = other.m_listOfSplineCoefs;
	m_listOfLambdas = other.m_listOfLambdas;
	m_markers = other.m_markers;
	m_fitness = other.m_fitness;
	m_isFitnessRecalculationNeeded = other.m_isFitnessRecalculationNeeded;

	for( int i = 0; i < m_listOfIntervals.count(); ++i )
	{
		SplineInterval& splineInterval = m_listOfIntervals[i];
		splineInterval.m_splineDNA = this;
	}

	return *this;
}

void SplineDNA::mutate()
{
	if( duplicatesExist() )
		int breakpoint = 0;
	const int indexOfKnotToRemove = qrand() % numberOfKnots();
	m_listOfKnots.removeAt( indexOfKnotToRemove );

	while( numberOfKnots() != GASettings::instance()->numberOfKnots() )
	{
		const int indexOfIntervalToAddKnot = qrand() % ( m_markers->count() - 1 );
		double intervalLeftBorder = m_markers->at( indexOfIntervalToAddKnot ).x();
		double intervalRightBorder = m_markers->at( indexOfIntervalToAddKnot + 1 ).x();

		const int percentOfTheInterval = qrand() % 99 + 1; // 1 ... 99 %
		double knotCoordinate = intervalLeftBorder + ( intervalRightBorder - intervalLeftBorder ) * percentOfTheInterval / 100.0;

		addKnot( knotCoordinate );
	}
	if( duplicatesExist() )
		int breakpoint = 0;
}

bool SplineDNA::duplicatesExist() const
{
	for( int i = 0; i < m_listOfKnots.count(); ++i )
	{
		for( int j = i + 1; j < m_listOfKnots.count(); ++j )
		{
			if( m_listOfKnots.at( i ) == m_listOfKnots.at( j ) )
				return true;
		}
	}

	return false;
}