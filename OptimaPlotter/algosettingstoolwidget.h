#ifndef ALGOSETTINGSTOOLWIDGET_H
#define ALGOSETTINGSTOOLWIDGET_H

#include <qwidget.h>

class QFocusEvent;
class QVBoxLayout;

class AlgoSettingsToolWidget: public QWidget
{
	Q_OBJECT

public:
	AlgoSettingsToolWidget( QWidget* parent= 0 );
	~AlgoSettingsToolWidget();

	void setCentralWidget( QWidget* widget );

signals:
	void closedByUser();

protected:
	virtual void focusInEvent( QFocusEvent* event );
	virtual void focusOutEvent( QFocusEvent* event );
	virtual void hideEvent ( QHideEvent* event );
	virtual void changeEvent( QEvent* event );

private:
	QVBoxLayout* m_mainLayout;
};

#endif