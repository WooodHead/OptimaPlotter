#include "stdafx.h"
#include "optimaplotter.h"
#include "plotwidget.h"
#include "settingsdialog.h"
#include "ialgorithm.h"
#include "globals.h"

#include <qdebug.h>
#include <qpen.h>
#include <qmath.h>
#include <qspinbox.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qpropertyanimation.h>
#include <qparallelanimationgroup.h>
#include <qtranslator.h>
#include <qsettings.h>
#include <qmessagebox.h>
#include <qdir.h>
#include <qpluginloader.h>

OptimaPlotter::OptimaPlotter( QWidget *parent, Qt::WFlags flags )
	: QMainWindow( parent, flags ), m_currentAlgorithm( 0 )
{
	m_translator = new QTranslator( this );
	
	ui.setupUi( this );

	initPlotWidget();
	setupToolBar();
	setupAnimation();

	onLoadPlugins();

	readSettings();
	retranslateUi();

	connect( ui.actionPick, SIGNAL( activated() ), this, SLOT( onPickModeActivated() ) );
	connect( ui.actionPan, SIGNAL( activated() ), this, SLOT( onPanModeActivated() ) );
	connect( ui.actionExecute, SIGNAL( triggered() ), this, SLOT( onExecute() ) );
	connect( ui.actionSettings, SIGNAL( triggered() ), this, SLOT( onExecuteSettingsDialog() ) );
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
	m_currentAlgorithm->setPropertyValueByTagName( "polynomial_degree", m_polynomialDegreeSpinBox->value() );
	m_currentAlgorithm->setPropertyValueByTagName( "samples_count", 100000 );

	QVector<QPointF> samples;
	QVector<QPointF> points;

	m_plotWidget->markerPointsVector( points );

	if( points.empty() )
		return;

	m_currentAlgorithm->initWithMarkers( points );
	m_currentAlgorithm->evaluate();
	m_currentAlgorithm->output( samples );
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

void OptimaPlotter::setupToolBar()
{
	m_algorithmSelectorComboBox = new QComboBox;
	ui.mainToolBar->addWidget( m_algorithmSelectorComboBox );

	ui.mainToolBar->addSeparator();

	m_polynomialDegreeLabel = new QLabel( tr( "Polynomial Degree:" ) );
	m_polynomialDegreeLabel->setMargin( 3 );
	ui.mainToolBar->addWidget( m_polynomialDegreeLabel );

	m_polynomialDegreeSpinBox = new QSpinBox;
	ui.mainToolBar->addWidget( m_polynomialDegreeSpinBox );
	m_polynomialDegreeSpinBox->setMinimum( 1 );
	m_polynomialDegreeSpinBox->setMaximum( 4 );
	m_polynomialDegreeSpinBox->setValue( 3 );
	m_polynomialDegreeSpinBox->setToolTip( tr( "Polynomial Degree" ) );

	m_polynomialDegreeLabel->setBuddy( m_polynomialDegreeSpinBox );
}

void OptimaPlotter::setupAnimation()
{
	QParallelAnimationGroup* animation = new QParallelAnimationGroup( this );

	QPropertyAnimation* opacityAnimation = new QPropertyAnimation( this, "windowOpacity" );
	opacityAnimation->setDuration( 1500 );
	opacityAnimation->setStartValue( 0.0 );
	opacityAnimation->setEndValue( 1.0 );
	opacityAnimation->setEasingCurve( QEasingCurve::InOutExpo );

	animation->addAnimation( opacityAnimation );
	animation->start();
}

void OptimaPlotter::onExecuteSettingsDialog()
{
	SettingsDialog settingsDialog;
	if( settingsDialog.exec() == QDialog::Accepted )
	{
		if( settingsDialog.languageNeedsToBeUpdated() )
		{
			QSettings settings( "BardiSolutions", "OptimaPlotter" );
			settings.setValue( "language", settingsDialog.currentLanguage() );
			readSettings();
		}
	}
}

void OptimaPlotter::readSettings()
{
	QSettings settings( "BardiSolutions", "OptimaPlotter" );
	int language = settings.value( "language", Globals::LANG_HY ).toInt();

	if( m_currentAlgorithm )
		m_currentAlgorithm->applyLanguage( language );

	switch( language )
	{
	case Globals::LANG_EN:
		{
			QApplication::removeTranslator( m_translator );
			break;
		}
	case Globals::LANG_HY:
		{
			m_translator->load( ":/OptimaPlotter/optimaplotter_hy.qm" );
			QApplication::installTranslator( m_translator );
			break;
		}
	default:
		{
			break;
		}
	}
}

void OptimaPlotter::retranslateUi()
{
	ui.retranslateUi( this );
	m_polynomialDegreeSpinBox->setToolTip( tr( "Polynomial Degree" ) );
	m_polynomialDegreeLabel->setText( tr( "Polynomial Degree:" ) );
	
	const int algorithmSelectorCurrentIndex = m_algorithmSelectorComboBox->currentIndex();
	m_algorithmSelectorComboBox->clear();
	
	foreach( IAlgorithm* algorithm, m_availableAlgorithms )
	{
		 m_algorithmSelectorComboBox->addItem( algorithm->name(), algorithm->tagName() );
	}
}

void OptimaPlotter::changeEvent( QEvent* event )
{
	if( event->type() == QEvent::LanguageChange )
		retranslateUi();
	else
		QMainWindow::changeEvent( event );
}

void OptimaPlotter::onLoadPlugins()
{
	 QDir pluginsDir( qApp->applicationDirPath() );

	 foreach ( QString fileName, pluginsDir.entryList( QDir::Files ) )
	 {
         QPluginLoader pluginLoader( pluginsDir.absoluteFilePath( fileName ) );
         QObject* plugin = pluginLoader.instance();
         if( plugin )
		 {
			 IAlgorithm* algorithm = qobject_cast<IAlgorithm*>( plugin );
             m_currentAlgorithm = algorithm;
			 m_availableAlgorithms.append( algorithm );
			 m_algorithmSelectorComboBox->addItem( algorithm->name(), algorithm->tagName() );
		 }
     }
}