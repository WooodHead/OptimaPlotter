#include "stdafx.h"
#include "optimaplotter.h"
#include "plotwidget.h"

#include <qdebug.h>
#include <qpen.h>
#include <qmath.h>

using namespace Eigen;

bool lessThanForTwoPointsOnXAxis( const QPointF& p1, const QPointF& p2 )
{
	return p1.x() < p2.x();
}

OptimaPlotter::OptimaPlotter( QWidget *parent, Qt::WFlags flags )
	: QMainWindow( parent, flags )
{
	ui.setupUi( this );

	initPlotWidget();

	connect( ui.actionPick, SIGNAL( activated() ), this, SLOT( onPickModeActivated() ) );
	connect( ui.actionPan, SIGNAL( activated() ), this, SLOT( onPanModeActivated() ) );
	connect( ui.actionExecute, SIGNAL( triggered() ), this, SLOT( onExecute() ) );
	connect( ui.actionReset, SIGNAL( triggered() ), this, SLOT( onReset() ) );

	connect( m_plotWidget, SIGNAL( pointPicked( const QPointF& ) ), this, SLOT( onPointAdded( const QPointF& ) ) );
}

OptimaPlotter::~OptimaPlotter()
{

}

void OptimaPlotter::onPickModeActivated()
{
	m_plotWidget->setPickerEnabled( true );
	m_plotWidget->setPannerEnabled( false );
	m_plotWidget->setMagnifierEnabled( false );
}
	
void OptimaPlotter::onPanModeActivated()
{
	m_plotWidget->setPickerEnabled( false );
	m_plotWidget->setPannerEnabled( true );
	m_plotWidget->setMagnifierEnabled( true );
}

void OptimaPlotter::onPointAdded( const QPointF& point )
{
	qDebug() << point;
	m_plotWidget->insertMarker( point );
	m_plotWidget->replot();
}

void OptimaPlotter::onExecute()
{
	const int polynomialDegree = 4;
	const int samplesCount = 100000;
	QVector<QPointF> samples;

	QVector<QPointF> points;

	m_plotWidget->markerPointsVector( points );

	if( points.empty() )
	{
		return;
	}

	qSort( points.begin(), points.end(), lessThanForTwoPointsOnXAxis );

	const double step = qAbs( points.first().x() -  points.last().x() ) / ( samplesCount - 1 );

	const int columnCount = polynomialDegree + 1;
	const int rowCount = points.size();

	MatrixXf matrixA( rowCount, columnCount );
	MatrixXf matrixAPlus( columnCount, rowCount );
	VectorXf vectorB( rowCount );
	VectorXf result( columnCount );

	int row = 0;
	foreach( QPointF point, points )
	{
		vectorB( row ) = point.y();

		for( int column = 0; column < columnCount; ++column )
		{
			matrixA( row, column ) = qPow( point.x(), column );
		}

		++row;
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

	int i = 0;
	double x = points.first().x();
	int pointIndex = 1;

	while( i != samplesCount + points.size() )
	{
		double y = 0;
		double xTemp = 1;
		
		for( int j = 0; j <= polynomialDegree; j++ )
		{
			y += xTemp * result( j );
			xTemp *= x;
		}

		samples.append( QPointF( x, y ) );
		//TODO: optimize
		if( pointIndex < points.size() && x + step >= points.at( pointIndex ).x() )
		{
			x = points.at( pointIndex ).x();
			++pointIndex;
		}
		else
		{
			x += step;
		}
		++i;
	}

	m_plotWidget->setCurveSamples( samples );
	m_plotWidget->replot();
}

void OptimaPlotter::initPlotWidget()
{
	m_plotWidget = new PlotWidget( this );
	setCentralWidget( m_plotWidget );
}

void OptimaPlotter::onEventLoopStarted()
{
	onPanModeActivated();
	m_plotWidget->panToCenter();
	onPickModeActivated();
}

void OptimaPlotter::onReset()
{
	m_plotWidget->reset();
}