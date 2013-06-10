#include "stdafx.h"
#include "optimaplotter.h"
#include "plotwidget.h"

#include <qdebug.h>
#include <qpen.h>
#include <qmath.h>
#include <qspinbox.h>
#include <qlabel.h>
#include <qpropertyanimation.h>

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
	setupToolbar();
	setupAnimation();

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
	const int polynomialDegree = m_polynomialDegreeSpinBox->value();
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

void OptimaPlotter::setupToolbar()
{
	QLabel* polynomialDegreeLabel = new QLabel( tr( "Polynomial Degree:" ) );
	polynomialDegreeLabel->setMargin( 3 );
	ui.mainToolBar->addWidget( polynomialDegreeLabel );

	m_polynomialDegreeSpinBox = new QSpinBox;
	ui.mainToolBar->addWidget( m_polynomialDegreeSpinBox );
	m_polynomialDegreeSpinBox->setMinimum( 1 );
	m_polynomialDegreeSpinBox->setMaximum( 4 );
	m_polynomialDegreeSpinBox->setValue( 3 );
	m_polynomialDegreeSpinBox->setToolTip( tr( "Polynomial Degree" ) );

	polynomialDegreeLabel->setBuddy( m_polynomialDegreeSpinBox );
}

void OptimaPlotter::setupAnimation()
{
	QPropertyAnimation* animation = new QPropertyAnimation( this, "windowOpacity" );
	animation->setDuration( 1500 );
	animation->setStartValue( 0.0 );
	animation->setEndValue( 1.0 );
	animation->setEasingCurve( QEasingCurve::InOutExpo );
	animation->start();
}