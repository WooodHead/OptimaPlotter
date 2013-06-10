#ifndef OPTIMAPLOTTER_H
#define OPTIMAPLOTTER_H

#include <QtGui/QMainWindow>
#include "ui_optimaplotter.h"

class PlotWidget;
class QSpinBox;

class OptimaPlotter : public QMainWindow
{
	Q_OBJECT

public:
	OptimaPlotter(QWidget *parent = 0, Qt::WFlags flags = 0);
	~OptimaPlotter();

private:
	void initPlotWidget();
	void setupToolbar();
	void setupAnimation();

public slots:
	void onReset();
	void onEventLoopStarted();

private slots:
	void onPickModeActivated();
	void onPanModeActivated();
	void onPointAdded( const QPointF& point );
	void onExecute();

private:
	Ui::OptimaPlotterClass ui;
	PlotWidget* m_plotWidget;
	QSpinBox* m_polynomialDegreeSpinBox;
};

#endif //OPTIMAPLOTTER_H
