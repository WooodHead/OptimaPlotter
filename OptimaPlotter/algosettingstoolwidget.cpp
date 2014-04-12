#include "stdafx.h"
#include "algosettingstoolwidget.h"

#include <qevent.h>
#include <qparallelanimationgroup.h>
#include <qpropertyanimation.h>
#include <qboxlayout.h>

AlgoSettingsToolWidget::AlgoSettingsToolWidget( QWidget* parent ) : QWidget( parent )
{
	setWindowFlags( Qt::Tool );
	setFocusPolicy( Qt::StrongFocus );
	setWindowOpacity( 0.6 );
	//setFocus( Qt::MouseFocusReason );
	

	m_mainLayout = new QVBoxLayout();
	m_mainLayout->setMargin( 0 );
	
	setLayout( m_mainLayout );
}

AlgoSettingsToolWidget::~AlgoSettingsToolWidget()
{

}

void AlgoSettingsToolWidget::focusInEvent ( QFocusEvent* event )
{
}

void AlgoSettingsToolWidget::focusOutEvent ( QFocusEvent* event )
{
}

void AlgoSettingsToolWidget::hideEvent ( QHideEvent* event )
{
	emit closedByUser();
}

void AlgoSettingsToolWidget::setCentralWidget( QWidget* widget )
{
	static QWidget* previousWidget = 0;
	if( previousWidget )
	{
		m_mainLayout->removeWidget( previousWidget );
		
		//previousWidget->hide();
		previousWidget->setParent( 0 );
	}
	delete m_mainLayout;
	//setLayout( 0 );

	m_mainLayout = new QVBoxLayout();
	m_mainLayout->setMargin( 0 );
	m_mainLayout->addWidget( widget );
	//widget->resize( widget->sizeHint() );
	//widget->show();
	
	setLayout( m_mainLayout );
	
	//resize( sizeHint() );

	if( widget )
		previousWidget = widget;
}

void AlgoSettingsToolWidget::changeEvent( QEvent* event ) 
{
	QEvent::Type type = event->type();
	if( event->type() == QEvent::ActivationChange )
	{
		if( isActiveWindow() )
			setWindowOpacity( 1.0 );
		else
		{
			QParallelAnimationGroup* animation = new QParallelAnimationGroup( this );

			QPropertyAnimation* opacityAnimation = new QPropertyAnimation( this, "windowOpacity" );
			opacityAnimation->setDuration( 500 );
			opacityAnimation->setStartValue( 1.0 );
			opacityAnimation->setEndValue( 0.6 );
			opacityAnimation->setEasingCurve( QEasingCurve::Linear );

			animation->addAnimation( opacityAnimation );
			animation->start();
		}
	}
}
