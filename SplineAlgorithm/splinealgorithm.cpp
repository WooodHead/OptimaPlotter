#include "stdafx.h"
#include "splinealgorithm.h"
#include "globals.h"

#include <qmath.h>
#include <qtranslator.h>
#include <qapplication.h>
#include <qnumeric.h>

#include <iostream>

using namespace Eigen;

bool lessThanForTwoPointsOnXAxis( const QPointF& p1, const QPointF& p2 )
{
	return p1.x() < p2.x();
}

SplineAlgorithm::SplineAlgorithm()
{
	m_settingsWidget = new QWidget();
	m_settingsWidgetForm.setupUi( m_settingsWidget );
}

SplineAlgorithm::~SplineAlgorithm()
{

}

void SplineAlgorithm::evaluate()
{
	m_samples.clear();

	bool ok;
	QVariant value;

	getPropertyValueByTagName( "samples_count", value, ok );
	if( !ok )
		return;
	const int samplesCount = value.toInt();

	applySettings();

	getPropertyValueByTagName( "spline_degree", value, ok );
	if( !ok )
		return;
	const int splineDegree = value.toInt();

	qSort( m_markers.begin(), m_markers.end(), lessThanForTwoPointsOnXAxis );
	qSort( m_knots.begin(), m_knots.end() );
	//TODO: remove dubpicates!

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
			}
			if( !qIsFinite( lambda ) )
				int breakpoint = 0;
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
				sum += lambdasOfAllBSplines.at( column ).at( j ) * 
					splineHelperFunction( m_knots.at( j + column ), m_markers.at( row ).x(), splineDegree );
			}

			if( qAbs( sum ) < 1.0e-06 || qIsNaN( sum ) )
				sum = 0;
			//if( qIsNaN( sum ) )
			//	sum = 0;
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

	std::cout << std::endl << "Result vector\n" << result << std::endl;
	std::cout << std::endl << "Matrix A\n" << matrixA << std::endl;
	//std::cout << std::endl << "Matrix A Reconstructed\n" << svd.matrixU() * matrixS *  svd.matrixV().transpose() << std::endl;
	std::cout << std::endl << "Matrix A+\n" << matrixAPlus << std::endl;

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

QString SplineAlgorithm::name() const
{
	return tr( "Spline" );
}

QString SplineAlgorithm::tagName() const
{
	return "spline";
}

QString SplineAlgorithm::translatorPath( int language ) const
{
	switch( language )
	{
	case ( int )Globals::LANG_HY:
		{
			return ":/splinealgorithm/splinealgorithm_hy.qm";
		}
	default:
		return QString();
	}
}

void SplineAlgorithm::retranslateUi()
{
	if( m_settingsWidget )
		m_settingsWidgetForm.retranslateUi( m_settingsWidget );
}

double SplineAlgorithm::splineHelperFunction( double base, double argument, int power ) const
{
	double valueToRaiseToPower = ( base - argument ) > 0 ? ( base - argument ) : 0;
	return qPow( valueToRaiseToPower, power );
}

Globals::AlgorithmFlags SplineAlgorithm::flags() const
{
	Globals::AlgorithmFlags flags = AlgorithmBase::flags();
	return flags | Globals::ALGO_FLAG_KNOT_PICKER;//  | Globals::ALGO_FLAG_RANGE_PICKER;
}

void SplineAlgorithm::applySettings()
{
	setPropertyValueByTagName( "spline_degree", m_settingsWidgetForm.splineDegreeSpinBox->value() );
}

Q_EXPORT_PLUGIN2( splinealgorithm, SplineAlgorithm );