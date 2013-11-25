#include "stdafx.h"
#include "optimaplotter.h"
#include "plotwidget.h"
#include "settingsdialog.h"
#include "ialgorithm.h"
#include "globals.h"
#include "knotpicker.h"

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

	connect( ui.actionPick, SIGNAL( activated() ), this, SLOT( onModeActivated() ) );
	connect( ui.actionPan, SIGNAL( activated() ), this, SLOT( onModeActivated() ) );

	connect( ui.actionPick, SIGNAL( activated() ), this, SLOT( onPickModeActivated() ) );
	connect( ui.actionPan, SIGNAL( activated() ), this, SLOT( onPanModeActivated() ) );
	connect( ui.actionExecute, SIGNAL( triggered() ), this, SLOT( onExecute() ) );
	connect( ui.actionSettings, SIGNAL( triggered() ), this, SLOT( onExecuteSettingsDialog() ) );
	connect( ui.actionReset, SIGNAL( triggered() ), this, SLOT( onReset() ) );

	connect( m_plotWidget, SIGNAL( pointPicked( const QPointF& ) ), this, SLOT( onPointAdded( const QPointF& ) ) );
	connect( m_plotWidget, SIGNAL( knotPicked( double ) ), this, SLOT( onKnotAdded( double ) ) );
}

OptimaPlotter::~OptimaPlotter()
{

}

void OptimaPlotter::onModeActivated()
{
	QList<QAction*> actions;
	actions << ui.actionPick << ui.actionPan;

	foreach( QAction* action, actions )
	{
		if( sender() != action )
			action->setChecked( false );
	}
}

void OptimaPlotter::onPickModeActivated()
{
	if( m_currentAlgorithm == 0 )
		return;

	m_plotWidget->setPickerEnabled( true );
	m_plotWidget->setPannerEnabled( false );
	m_plotWidget->setMagnifierEnabled( false );

	if( m_currentAlgorithm->flags() & Globals::ALGO_FLAG_KNOT_PICKER ) 
		m_plotWidget->setKnotPickerEnabled( true );
}
	
void OptimaPlotter::onPanModeActivated()
{
	if( m_currentAlgorithm == 0 )
		return;

	m_plotWidget->setPickerEnabled( false );
	m_plotWidget->setPannerEnabled( true );
	m_plotWidget->setMagnifierEnabled( true );

	if( m_currentAlgorithm->flags() & Globals::ALGO_FLAG_KNOT_PICKER ) 
		m_plotWidget->setKnotPickerEnabled( false );
}

void OptimaPlotter::onPointAdded( const QPointF& point )
{
	qDebug() << point;
	m_plotWidget->insertMarker( point );
	m_plotWidget->replot();
}

void OptimaPlotter::onKnotAdded( double coordinate )
{
	m_plotWidget->insertKnot( coordinate );
	m_plotWidget->replot();
}

void OptimaPlotter::onExecute()
{
	if( m_currentAlgorithm == 0 )
		return;

	m_currentAlgorithm->setPropertyValueByTagName( "polynomial_degree", m_polynomialDegreeSpinBox->value() );
	m_currentAlgorithm->setPropertyValueByTagName( "samples_count", 100000 );

	QVector<QPointF> samples;
	QVector<QPointF> points;

	m_plotWidget->markerPointsVector( points );

	if( points.empty() )
		return;

	m_currentAlgorithm->initWithMarkers( points );

	if( m_currentAlgorithm->flags() & Globals::ALGO_FLAG_KNOT_PICKER )
	{
		QVector<double> knots;
		m_plotWidget->knotsVector( knots );
		m_currentAlgorithm->initWithKnots( knots );
	}

	m_currentAlgorithm->evaluate();
	m_currentAlgorithm->output( samples );

	m_plotWidget->setCurveSamples( samples );
	m_plotWidget->replot();
}

void OptimaPlotter::initPlotWidget()
{
	m_plotWidget = new PlotWidget( this );

	// KnotPicker is not inited in PlotWidget constructor, because it requires
	// QwtPlot as a parent and the PlotWidget is inherited from the QwtPlot class.
	KnotPicker* knotPicker = new KnotPicker( m_plotWidget );
	m_plotWidget->setKnotPicker( knotPicker );

	setCentralWidget( m_plotWidget );
}

void OptimaPlotter::onEventLoopStarted()
{
	onPanModeActivated();
	//m_plotWidget->panToCenter();
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

	m_polynomialDegreeLabel = new QLabel( tr( "Degree:" ) );
	m_polynomialDegreeLabel->setMargin( 3 );
	ui.mainToolBar->addWidget( m_polynomialDegreeLabel );

	m_polynomialDegreeSpinBox = new QSpinBox;
	ui.mainToolBar->addWidget( m_polynomialDegreeSpinBox );
	m_polynomialDegreeSpinBox->setMinimum( 1 );
	m_polynomialDegreeSpinBox->setMaximum( 4 );
	m_polynomialDegreeSpinBox->setValue( 3 );
	m_polynomialDegreeSpinBox->setToolTip( tr( "Degree" ) );

	m_polynomialDegreeLabel->setBuddy( m_polynomialDegreeSpinBox );

	connect( m_algorithmSelectorComboBox, SIGNAL( currentIndexChanged( int ) ), this, SLOT( onAlgorithmSelectorIndexChanged( int ) ) );
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

	foreach( IAlgorithm* algorithm, m_availableAlgorithms )
	{
		algorithm->applyLanguage( language );
	}

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
	m_polynomialDegreeSpinBox->setToolTip( tr( "Degree" ) );
	m_polynomialDegreeLabel->setText( tr( "Degree:" ) );
	
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

void OptimaPlotter::onAlgorithmSelectorIndexChanged( int index )
{
	QString itemData = m_algorithmSelectorComboBox->itemData( index ).toString();
	if( !itemData.isNull() )
	{
		foreach( IAlgorithm* algorithm, m_availableAlgorithms )
		{
			if( algorithm->tagName() == itemData )
			{
				m_currentAlgorithm = algorithm;

				if( m_currentAlgorithm->flags() & Globals::ALGO_FLAG_KNOT_PICKER )
				{
					m_plotWidget->setKnotsEnabled( true );
					if( ui.actionPick )
						m_plotWidget->setKnotPickerEnabled( true );
				}
				else
				{
					m_plotWidget->setKnotsEnabled( false );
					if( ui.actionPick )
						m_plotWidget->setKnotPickerEnabled( false );
				}
				break;
			}
		}
	}

	m_plotWidget->replot();
}