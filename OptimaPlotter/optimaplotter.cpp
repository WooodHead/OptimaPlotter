#include "stdafx.h"
#include "optimaplotter.h"
#include "plotwidget.h"
#include "settingsdialog.h"
#include "ialgorithm.h"
#include "globals.h"
#include "knotpicker.h"
#include "canvaspicker.h"
#include "markerexplorer.h"
#include "knotexplorer.h"
#include "rangeexplorer.h"

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
	m_markerExplorer = 0;
	m_knotExplorer = 0;
	m_rangeExplorer = 0;
	m_translator = new QTranslator( this );
	
	ui.setupUi( this );

	initPlotWidget();
	setupToolBar();
	setupAnimation();

	onLoadPlugins();

	setupDocks();

	readSettings();
	retranslateUi();

	connect( ui.actionPick, SIGNAL( activated() ), this, SLOT( onModeActivated() ) );
	connect( ui.actionPan, SIGNAL( activated() ), this, SLOT( onModeActivated() ) );

	connect( ui.actionPick, SIGNAL( activated() ), this, SLOT( onPickModeActivated() ) );
	connect( ui.actionPan, SIGNAL( activated() ), this, SLOT( onPanModeActivated() ) );
	connect( ui.actionExecute, SIGNAL( triggered() ), this, SLOT( onExecute() ) );
	connect( ui.actionSettings, SIGNAL( triggered() ), this, SLOT( onExecuteSettingsDialog() ) );

	connect( m_plotWidget, SIGNAL( pointPicked( const QPointF& ) ), this, SLOT( onPointAdded( const QPointF& ) ) );
	connect( m_plotWidget, SIGNAL( knotPicked( double ) ), this, SLOT( onKnotAdded( double ) ) );
	

	// Connections between the explorers and the plot widget.
	connect( m_markerExplorer, SIGNAL( itemAdded( QwtPlotItem* ) ), m_plotWidget, SLOT( onItemAdded( QwtPlotItem* ) ) );
	connect( m_knotExplorer, SIGNAL( itemAdded( QwtPlotItem* ) ), m_plotWidget, SLOT( onItemAdded( QwtPlotItem* ) ) );

	connect( m_markerExplorer, SIGNAL( selectionChanged( QList<QwtPlotItem*>&, QList<QwtPlotItem*>& ) ),
		m_plotWidget, SLOT( onSelectionChanged( QList<QwtPlotItem*>&, QList<QwtPlotItem*>& ) ) );
	connect( m_knotExplorer, SIGNAL( selectionChanged( QList<QwtPlotItem*>&, QList<QwtPlotItem*>& ) ),
		m_plotWidget, SLOT( onSelectionChanged( QList<QwtPlotItem*>&, QList<QwtPlotItem*>& ) ) );
	connect( m_rangeExplorer, SIGNAL( selectionChanged( QList<QwtPlotItem*>&, QList<QwtPlotItem*>& ) ),
		m_plotWidget, SLOT( onSelectionChanged( QList<QwtPlotItem*>&, QList<QwtPlotItem*>& ) ) );

	connect( m_plotWidget, SIGNAL( dataChanged( QwtPlotItem* ) ), m_markerExplorer, SLOT( onDataChanged( QwtPlotItem* ) ) );
	connect( m_plotWidget, SIGNAL( dataChanged( QwtPlotItem* ) ), m_knotExplorer, SLOT( onDataChanged( QwtPlotItem* ) ) );
	connect( m_plotWidget, SIGNAL( dataChanged( QwtPlotItem* ) ), m_rangeExplorer, SLOT( onDataChanged( QwtPlotItem* ) ) );

	connect( m_plotWidget, SIGNAL( itemAdded( QwtPlotItem* ) ), m_markerExplorer, SLOT( onItemAdded( QwtPlotItem* ) ) );
	connect( m_plotWidget, SIGNAL( itemAdded( QwtPlotItem* ) ), m_knotExplorer, SLOT( onItemAdded( QwtPlotItem* ) ) );
	
	connect( m_plotWidget, SIGNAL( selectionChanged( QList<QwtPlotItem*>&, QList<QwtPlotItem*>& ) ),
		m_markerExplorer, SLOT( onSelectionChangedFromPlotWidget( QList<QwtPlotItem*>&, QList<QwtPlotItem*>& ) ) );
	connect( m_plotWidget, SIGNAL( selectionChanged( QList<QwtPlotItem*>&, QList<QwtPlotItem*>& ) ),
		m_knotExplorer, SLOT( onSelectionChangedFromPlotWidget( QList<QwtPlotItem*>&, QList<QwtPlotItem*>& ) ) );
	connect( m_plotWidget, SIGNAL( selectionChanged( QList<QwtPlotItem*>&, QList<QwtPlotItem*>& ) ),
		m_rangeExplorer, SLOT( onSelectionChangedFromPlotWidget( QList<QwtPlotItem*>&, QList<QwtPlotItem*>& ) ) );

	connect( m_markerExplorer, SIGNAL( deleteSelectedItems() ), m_plotWidget, SLOT( onDeleteSelectedMarkers() ) );
	connect( m_knotExplorer, SIGNAL( deleteSelectedItems() ), m_plotWidget, SLOT( onDeleteSelectedKnots() ) );

	onPickModeActivated();
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

	m_plotWidget->setCanvasPickerEnabled( true );
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

	m_plotWidget->setCanvasPickerEnabled( false );
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

	if( m_currentAlgorithm->flags() & Globals::ALGO_FLAG_RANGE_PICKER )
		m_knotExplorer->deleteAllItems();

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

	if( m_currentAlgorithm->flags() & Globals::ALGO_FLAG_RANGE_PICKER )
	{
		double leftBorder = 0.0;
		double rightBorder = 0.0;

		m_plotWidget->rangeBorders( leftBorder, rightBorder );
		m_currentAlgorithm->initWithRange( leftBorder, rightBorder );
	}

	m_currentAlgorithm->evaluate();
	m_currentAlgorithm->outputSamples( samples );

	m_plotWidget->setCurveSamples( samples );

	if( m_currentAlgorithm->flags() & Globals::ALGO_FLAG_RANGE_PICKER )
	{
		QVector<double> knots;
		m_currentAlgorithm->outputKnots( knots );
		foreach( double coordinate, knots )
		{
			m_knotExplorer->addKnot( coordinate, false );
		}
	}
	m_plotWidget->replot();
}

void OptimaPlotter::initPlotWidget()
{
	m_plotWidget = new PlotWidget( this );

	// KnotPicker and CanvasPicker are not inited in PlotWidget constructor, because they require
	// QwtPlot as a parent and the PlotWidget is inherited from the QwtPlot class.
	KnotPicker* knotPicker = new KnotPicker( m_plotWidget );
	CanvasPicker* canvasPicker = new CanvasPicker( m_plotWidget );
	m_plotWidget->setKnotPicker( knotPicker );
	m_plotWidget->setCanvasPicker( canvasPicker );

	setCentralWidget( m_plotWidget );
}

void OptimaPlotter::onEventLoopStarted()
{
	//onPanModeActivated();
	//m_plotWidget->panToCenter();
	//onPickModeActivated();
}

void OptimaPlotter::setupDocks()
{
	m_markerExplorer = new MarkerExplorer( this );
	m_knotExplorer = new KnotExplorer( this );
	m_rangeExplorer = new RangeExplorer( m_plotWidget, this );
	
	// This "connect" is here because we need to sync the range items and they are created upon explorer view. 
	connect( m_rangeExplorer, SIGNAL( itemAdded( QwtPlotItem* ) ), m_plotWidget, SLOT( onItemAdded( QwtPlotItem* ) ) );

	ui.knotDockWidget->setWidget( m_knotExplorer );
	ui.knotDockWidget->setWindowTitle( m_knotExplorer->name() );
	ui.knotDockWidget->setFeatures( QDockWidget::NoDockWidgetFeatures );

	ui.markerDockWidget->setWidget( m_markerExplorer );
	ui.markerDockWidget->setWindowTitle( m_markerExplorer->name() );
	ui.markerDockWidget->setFeatures( QDockWidget::NoDockWidgetFeatures );

	ui.rangeDockWidget->setWidget( m_rangeExplorer );
	ui.rangeDockWidget->setWindowTitle( m_rangeExplorer->name() );
	ui.rangeDockWidget->setFeatures( QDockWidget::NoDockWidgetFeatures );

	if( m_currentAlgorithm != 0 )
	{
		ui.knotDockWidget->setVisible( m_currentAlgorithm->flags() & Globals::ALGO_FLAG_KNOT_PICKER ? true : false );
		m_knotExplorer->allowActions( m_currentAlgorithm->flags() &  Globals::ALGO_FLAG_KNOT_PICKER );
		ui.rangeDockWidget->setVisible( m_currentAlgorithm->flags() & Globals::ALGO_FLAG_RANGE_PICKER ? true : false );
	}
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

	ui.knotDockWidget->setWindowTitle( m_knotExplorer->name() );
	ui.markerDockWidget->setWindowTitle( m_markerExplorer->name() );
	ui.rangeDockWidget->setWindowTitle( m_rangeExplorer->name() );
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

				if( m_currentAlgorithm->flags() & Globals::ALGO_FLAG_RANGE_PICKER )
				{
					m_plotWidget->setRangeEnabled( true );
					if( !( m_currentAlgorithm->flags() & Globals::ALGO_FLAG_KNOT_PICKER ) )
					{
						m_plotWidget->setKnotsEditable( false );
					}
					ui.rangeDockWidget->setVisible( true );
				}
				else
				{
					m_plotWidget->setRangeEnabled( false );
					m_plotWidget->setKnotsEditable( true );
					ui.rangeDockWidget->setVisible( false );
				}

				if( ui.actionPick )
				{
					if( m_currentAlgorithm->flags() & Globals::ALGO_FLAG_KNOT_PICKER )
						m_plotWidget->setKnotPickerEnabled( true );
					else
						m_plotWidget->setKnotPickerEnabled( false );
				}

				if( m_currentAlgorithm->flags() & ( Globals::ALGO_FLAG_KNOT_PICKER |
													Globals::ALGO_FLAG_RANGE_PICKER ) )
				{
					m_plotWidget->setKnotsEnabled( true );
					ui.knotDockWidget->setVisible( true );

					if( m_knotExplorer )
						m_knotExplorer->allowActions( m_currentAlgorithm->flags() &  Globals::ALGO_FLAG_KNOT_PICKER );
				}
				else
				{
					m_plotWidget->setKnotsEnabled( false );
					ui.knotDockWidget->setVisible( false );
				}

				break;
			}
		}
	}

	m_plotWidget->replot();
}

const PlotWidget* OptimaPlotter::plotWidget() const
{
	return m_plotWidget;
}