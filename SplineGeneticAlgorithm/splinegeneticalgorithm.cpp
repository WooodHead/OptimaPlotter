#include "stdafx.h"
#include "splinegeneticalgorithm.h"
#include "globals.h"
#include "splinedna.h"
#include "gasettings.h"
#include "splinepopulation.h"

#include <qmath.h>
#include <qnumeric.h>
#include <qtranslator.h>
#include <qapplication.h>
#include <qtconcurrentmap.h>

#include <iostream>

using namespace Eigen;

bool lessThanForTwoPointsOnXAxis( const QPointF& p1, const QPointF& p2 )
{
	return p1.x() < p2.x();
}

bool lessThanForTwoPointsOnYAxis( const QPointF& p1, const QPointF& p2 )
{
	return p1.y() < p2.y();
}

bool markersEqual( const QPointF& p1, const QPointF& p2 )
{
	if( p1.x() == p2.x() )
		return true;

	if( qAbs( p1.x() - p2.x() ) < GASettings::instance()->xTolerance() &&
		qAbs( p1.y() - p2.y() ) < GASettings::instance()->yTolerance() )
		return true;
	return false;
}

SplineDNA crossoverAndMutate( const QPair<SplineDNA, SplineDNA>& pairOfDNAs );

SplineGA::SplineGA() : m_currentPopulation( m_markers )
{

}

SplineGA::~SplineGA()
{

}

void SplineGA::evaluate()
{
	m_samples.clear();
	m_knots.clear();

	bool ok;
	QVariant value;

	getPropertyValueByTagName( "samples_count", value, ok );
	if( !ok )
		return;
	const int samplesCount = value.toInt();

	getPropertyValueByTagName( "polynomial_degree", value, ok );
	if( !ok )
		return;
	const int splineDegree = value.toInt();

	GASettings* gaSettings = GASettings::instance();
	gaSettings->setSplineDegree( splineDegree );
	gaSettings->setNumberOfIntervals( 4 ); //TODO: dynamically change

	QPointF minPointOnY = *std::min_element( m_markers.begin(), m_markers.end(), lessThanForTwoPointsOnYAxis );
	QPointF maxPointOnY = *std::max_element( m_markers.begin(), m_markers.end(), lessThanForTwoPointsOnYAxis );

	double intervalLengthOnY = maxPointOnY.y() - minPointOnY.y();
	gaSettings->setYTolerance( intervalLengthOnY * 5.0 / 100.0 );

	qSort( m_markers.begin(), m_markers.end(), lessThanForTwoPointsOnXAxis );
	double intervalLengthOnX = m_markers.last().x() - m_markers.first().x();
	//gaSettings->setXTolerance( intervalLengthOnX == 0 ? 0.1 : intervalLengthOnX / 1000.0 );
	gaSettings->setXTolerance( qAbs( m_rangeRightBorder - m_rangeLeftBorder ) / 1000.0 );
	
	for( int i = 0; i < m_markers.count(); ++i )
	{
		if( m_markers.at( i ).x() <= m_rangeLeftBorder ||
			m_markers.at( i ).x() >= m_rangeRightBorder )
		{
			m_markers.remove( i );
			--i;
		}
	}

	m_markers.prepend( QPointF( m_rangeLeftBorder, 0 ) );
	m_markers.append( QPointF( m_rangeRightBorder, 0 ) );

	QVector<QPointF>::iterator it1 = std::unique( m_markers.begin(), m_markers.end(), markersEqual );
	m_markers.erase( it1, m_markers.end() );

	executeGA();

	//TODO: remove?
	qSort( m_knots.begin(), m_knots.end() );
	QVector<double>::iterator it2 = std::unique( m_knots.begin(), m_knots.end() );
	m_knots.erase( it2, m_knots.end() );

	const double step = qAbs( m_markers.first().x() -  m_markers.last().x() ) / ( samplesCount - 1 );

	const int columnCount = m_knots.count() - 1 - splineDegree;
	if( columnCount < 1 )
		return;

	const int rowCount = m_markers.size();

	MatrixXf matrixA( rowCount, columnCount );
	MatrixXf matrixAPlus( columnCount, rowCount );
	VectorXf vectorB( rowCount );
	VectorXf result( columnCount );

	int row = 0;
	foreach( QPointF point, m_markers )
	{
		vectorB( row ) = point.y();
		++row;
	}

	QList<QList<double> > lambdasOfAllBSplines; 
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

				lambda /= ( m_knots.at( i + column ) - m_knots.at( j + column ) );
				if( !qIsFinite( lambda ) )
					int breakpoint = 0;
			}
			lambdas.append( lambda );
		}

		lambdasOfAllBSplines.append( lambdas );
	}

	for( int column = 0; column < columnCount; ++column )
	{
		for( int row = 0; row < rowCount; ++row )
		{
			double sum = 0.0;
			for( int j = 0; j <= splineDegree + 1; ++j )
			{
				double lambda = lambdasOfAllBSplines.at( column ).at( j );
				sum += lambda * 
					splineHelperFunction( m_knots.at( j + column ), m_markers.at( row ).x(), splineDegree );
				
				if( qIsNaN( sum ) )
					int breakpoint = 0;
			}

			if( qAbs( sum ) < 1.0e-06 || qIsNaN( sum ) )
				sum = 0;

			/*if( !qIsFinite( sum ) )
				sum = 1.0;*/

			matrixA( row, column ) = sum;
		}
	}

	JacobiSVD<MatrixXf> svd( matrixA, ComputeFullU | ComputeFullV );

	MatrixXf matrixSPlus = MatrixXf::Zero( columnCount, rowCount );

	const JacobiSVD<MatrixXf>::SingularValuesType& singularValues = svd.singularValues();
	for( int i = 0; i < singularValues.size(); i++ )
	{
		float value = svd.singularValues()[i];
		if( value ) //TODO: is this check needed?
		{
			matrixSPlus( i, i ) = 1.0 / value;
		}
	}

	matrixAPlus = svd.matrixV() * matrixSPlus * svd.matrixU().transpose();
	result = matrixAPlus * vectorB;

	//std::cout << std::endl << "Result vector\n" << result << std::endl;
	//std::cout << std::endl << "Matrix A\n" << matrixA << std::endl;
	//std::cout << std::endl << "Matrix A+\n" << matrixAPlus << std::endl;

	int i = 0;
	double x = m_markers.first().x();
	int pointIndex = 1;

	while( i != samplesCount + m_markers.size() )
	{
		double y = 0;
		double xTemp = 1;
		
		for( int j = 0; j < columnCount; ++j )
		{
			double sum = 0;
			for( int k = 0; k <= splineDegree + 1; ++k )
			{
				sum += lambdasOfAllBSplines.at( j ).at( k ) * 
					splineHelperFunction( m_knots.at( k + j ), x, splineDegree );
			}

			y += sum * result( j );
		}

		m_samples.append( QPointF( x, y ) );
		//TODO: optimize
		if( pointIndex < m_markers.size() && x + step >= m_markers.at( pointIndex ).x() )
		{
			x = m_markers.at( pointIndex ).x();
			++pointIndex;
		}
		else
		{
			x += step;
		}
		++i;
	}
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

double SplineGA::splineHelperFunction( double base, double argument, int power )
{
	double valueToRaiseToPower = ( base - argument ) > 0 ? ( base - argument ) : 0;
	return qPow( valueToRaiseToPower, power );
}

Globals::AlgorithmFlags SplineGA::flags() const
{
	Globals::AlgorithmFlags flags = AlgorithmBase::flags();
	return flags | Globals::ALGO_FLAG_RANGE_PICKER;
}

void SplineGA::createFirstPopulation()
{
	const int markersCount = m_markers.count();
	if( markersCount < 3 )
		return;

	GASettings* gaSettings = GASettings::instance();
	const int numberOfKnots = gaSettings->numberOfKnots();
	const int populationSize = gaSettings->populationSize();

	for( int i = 0; i < populationSize; ++i )
	{
		SplineDNA splineDNA( &m_markers );

		while( splineDNA.numberOfKnots() != numberOfKnots )
		{
			const int indexOfIntervalToAddKnot = qrand() % ( markersCount - 1 );
			double intervalLeftBorder = m_markers.at( indexOfIntervalToAddKnot ).x();
			double intervalRightBorder = m_markers.at( indexOfIntervalToAddKnot + 1 ).x();

			const int percentOfTheInterval = qrand() % 99 + 1; // 1 ... 99 %
			double knotCoordinate = intervalLeftBorder + ( intervalRightBorder - intervalLeftBorder ) * percentOfTheInterval / 100.0;

			splineDNA.addKnot( knotCoordinate );
		}

		splineDNA.computeFitness();
		splineDNA.buildIntervals();
		m_currentPopulation.addIndividualToPopulation( splineDNA );
	}

	m_currentPopulation.sortIndividualsOnFitness();

	/*population.sortIndividualsOnFitness();
	SplineDNA bestDNA = population.individualAtPosition( 0 );
	bestDNA.buildIntervals();
	std::cout << "Best DNA fitness: " << std::endl;
	bestDNA.dumpFitness();

	SplineDNA secondBestDNA = population.individualAtPosition( 1 );
	secondBestDNA.buildIntervals();
	std::cout << "Second Best DNA fitness: " << std::endl;
	secondBestDNA.dumpFitness();

	SplineDNA crossoverDNA = crossOver( bestDNA, secondBestDNA );
	crossoverDNA.mutate();
	crossoverDNA.buildIntervals();
	for( int i = 0; i < crossoverDNA.numberOfKnots(); ++i )
	{
		m_knots.append( crossoverDNA.splineKnotAtPosition( i ).coordinate() );
	}
	
	std::cout << "Crossover DNA fitness: " << std::endl;
	crossoverDNA.dumpFitness();*/
}

void SplineGA::executeGA()
{
	createFirstPopulation();
	GASettings* gaSettings = GASettings::instance();
	const int numberOfKnots = gaSettings->numberOfKnots();
	const int populationSize = gaSettings->populationSize();
	const int numberOfGenerations = gaSettings->numberOfGenerations();

	const QList<SplineDNA>& listOfIndividuals = m_currentPopulation.listOfIndividuals();
	
	QList<SplineDNA> individualsToCrossover;
	const int countOfIntervalsToCrossover = ( int( qSqrt( 1 + 16 * populationSize ) ) + 2 ) / 2 + 1;

	int generationNumber = 0;
	while( generationNumber < numberOfGenerations )
	{
		SplinePopulation nextGenPopulation( m_markers );
		QList<QPair<SplineDNA, SplineDNA> > listOfPairDNAsToCrossoverAndMutate;

		for( int i = 0; i < countOfIntervalsToCrossover, i < populationSize; ++i )
		{
			const SplineDNA& splineDNA1 = listOfIndividuals.at( i );
			for( int j = i + 1; j < countOfIntervalsToCrossover, j < populationSize; ++j )
			{
				const SplineDNA& splineDNA2 = listOfIndividuals.at( j );
				listOfPairDNAsToCrossoverAndMutate.append( QPair<SplineDNA, SplineDNA>( splineDNA1, splineDNA2 ) );
			}
		}

		QFuture<SplineDNA> future = QtConcurrent::mapped( listOfPairDNAsToCrossoverAndMutate, crossoverAndMutate );
		while( future.isRunning() ) ;
		QFuture<SplineDNA>::const_iterator iterator = future.constBegin();
		for( ; iterator != future.constEnd(); ++iterator )
		{
			SplineDNA dna = *iterator;
			nextGenPopulation.addIndividualToPopulation( dna );
		}

		nextGenPopulation.sortIndividualsOnFitness();
		m_currentPopulation.clear();
		for( int i = 0; i < populationSize, i < nextGenPopulation.numberOfIndividuals(); ++i )
		{
			SplineDNA splineDNA = nextGenPopulation.individualAtPosition( i );
			m_currentPopulation.addIndividualToPopulation( splineDNA );
		}

		generationNumber++;
	}

	m_currentPopulation.sortIndividualsOnFitness();
	SplineDNA bestDNA = m_currentPopulation.individualAtPosition( 0 );

	bestDNA.buildIntervals();
	for( int i = 0; i < bestDNA.numberOfKnots(); ++i )
	{
		m_knots.append( bestDNA.splineKnotAtPosition( i ).coordinate() );
	}
	
	std::cout << "BEST DNA fitness: " << std::endl;
	bestDNA.dumpFitness();

}

SplineDNA crossoverAndMutate( const QPair<SplineDNA, SplineDNA>& pairOfDNAs )
{
	SplineDNA resultSpline = crossOver( pairOfDNAs.first, pairOfDNAs.second );
	resultSpline.mutate();
	resultSpline.computeFitness();
	resultSpline.buildIntervals();
	return resultSpline;
}

Q_EXPORT_PLUGIN2( splinegeneticalgorithm, SplineGA );