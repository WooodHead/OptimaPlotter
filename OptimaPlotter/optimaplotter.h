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

class OptimaPlotter : public QMainWindow
{
	Q_OBJECT

public:
	OptimaPlotter(QWidget *parent = 0, Qt::WFlags flags = 0);
	~OptimaPlotter();

private:
	void initPlotWidget();
	void setupToolBar();
	void setupAnimation();
	void readSettings();

	void retranslateUi();
	virtual void changeEvent( QEvent* event );

public slots:
	void onReset();
	void onEventLoopStarted();

private slots:
	void onModeActivated();
	void onPickModeActivated();
	void onPanModeActivated();

	void onPointAdded( const QPointF& point );
	void onKnotAdded( double coordinate );
	void onExecute();
	void onExecuteSettingsDialog();
	void onLoadPlugins();
	void onAlgorithmSelectorIndexChanged( int index );

private:
	Ui::OptimaPlotterClass ui;
	PlotWidget* m_plotWidget;
	QLabel* m_polynomialDegreeLabel;
	QSpinBox* m_polynomialDegreeSpinBox;
	QComboBox* m_algorithmSelectorComboBox;
	QTranslator* m_translator;
	IAlgorithm* m_currentAlgorithm;
	QList<IAlgorithm*> m_availableAlgorithms;
};

#endif //OPTIMAPLOTTER_H
