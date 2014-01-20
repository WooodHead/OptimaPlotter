#include "stdafx.h"
#include "markerpickerclickpointmachine.h"
#include "qwt_event_pattern.h"
#include "qevent.h"

MarkerPickerClickPointMachine::MarkerPickerClickPointMachine() : QwtPickerClickPointMachine()
{

}

QList<QwtPickerMachine::Command> MarkerPickerClickPointMachine::transition( const QwtEventPattern& eventPattern, const QEvent* event )
{
    QList<QwtPickerMachine::Command> cmdList;

    switch ( event->type() )
    {
        case QEvent::MouseButtonPress:
        {
            if ( eventPattern.mouseMatch( QwtEventPattern::MouseSelect2, 
                static_cast<const QMouseEvent *>( event ) ) )
            {
                if ( state() == 0 )
                {
                    cmdList += Begin;
                    cmdList += Append;
                    setState( 1 );
                }
            }
            break;
        }
        case QEvent::MouseMove:
        case QEvent::Wheel:
        {
            if ( state() != 0 )
                cmdList += Move;
            break;
        }
        case QEvent::MouseButtonRelease:
        {
            if ( state() != 0 )
            {
                cmdList += End;
                setState( 0 );
            }
            break;
        }
        case QEvent::KeyPress:
        {
            if ( eventPattern.keyMatch( QwtEventPattern::KeySelect1, 
                static_cast<const QKeyEvent *>( event ) ) )
            {
                if ( state() == 0 )
                {
                    cmdList += Begin;
                    cmdList += Append;
                    setState( 1 );
                }
                else
                {
                    cmdList += End;
                    setState( 0 );
                }
            }
            break;
        }
        default:
            break;
    }

    return cmdList;
}