#ifndef OPTIMAPLOTTER_H
#define OPTIMAPLOTTER_H

#include <QtGui/QMainWindow>
#include "ui_optimaplotter.h"

class PlotWidget;
class IAlgorithm;
class QSpinBox;
class QTranslator;
class QLabel;
class QComboBox;
class MarkerExplorer;
class KnotExplorer;
class RangeExplorer;
class AlgoSettingsToolWidget;

class OptimaPlotter : public QMainWindow
{
	Q_OBJECT

public:
	OptimaPlotter(QWidget *parent = 0, Qt::WFlags flags = 0);
	~OptimaPlotter();

	const PlotWidget* plotWidget() const;
private:
	void initPlotWidget();
	void setupDocks();
	void setupToolBar();
	void setupAnimation();
	void readSettings();

	void retranslateUi();
	virtual void changeEvent( QEvent* event );

public slots:
	void onEventLoopStarted();

private slots:
	void onModeActivated();
	void onPickModeActivated();
	void onPanModeActivated();

	void onPointAdded( const QPointF& point );
	void onKnotAdded( double coordinate );
	void onExecute();
	void onAlgorithmSettingsToolWidgetClosed();
	void onShowAlgorithmSettingsToolWidget();
	void onExecuteSettingsDialog();
	void onLoadPlugins();
	void onAlgorithmSelectorIndexChanged( int index );

private:
	Ui::OptimaPlotterClass ui;
	PlotWidget* m_plotWidget;
	QComboBox* m_algorithmSelectorComboBox;
	QTranslator* m_translator;
	IAlgorithm* m_currentAlgorithm;
	QList<IAlgorithm*> m_availableAlgorithms;

	
	MarkerExplorer* m_markerExplorer;
	KnotExplorer* m_knotExplorer;
	RangeExplorer* m_rangeExplorer;

	AlgoSettingsToolWidget* m_algoSettingsToolWidget;
};

#endif //OPTIMAPLOTTER_H
