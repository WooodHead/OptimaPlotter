#ifndef MARKERPICKERCLICKPOINTMACHINE_H
#define MARKERPICKERCLICKPOINTMACHINE_H

#include "qwt_picker_machine.h"

class QEvent;

class MarkerPickerClickPointMachine : public QwtPickerClickPointMachine
{
public:
	MarkerPickerClickPointMachine();
	virtual QList<Command> transition( const QwtEventPattern& eventPattern, const QEvent* event );
};

#endif