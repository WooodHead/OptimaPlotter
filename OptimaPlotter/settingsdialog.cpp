#include "stdafx.h"
#include "settingsdialog.h"

#include "qsettings.h"

SettingsDialog::SettingsDialog( QWidget* parent, Qt::WindowFlags windowFlags ) : QDialog( parent, windowFlags ), m_languageNeedsToBeUpdated( false )
{
	ui.setupUi( this );

	setWindowFlags( this->windowFlags() & ~Qt::WindowContextHelpButtonHint );

	QSettings settings( "BardiSolutions", "OptimaPlotter" );
	ui.comboBoxLanguage->setCurrentIndex( settings.value( "language", Language::SETTINGS_LANG_HY ).toInt() );

	connect( ui.comboBoxLanguage, SIGNAL( currentIndexChanged( int ) ), SLOT( onLanguageChanged( int ) ) );
}

SettingsDialog::~SettingsDialog()
{

}

void SettingsDialog::onLanguageChanged( int index )
{
	m_languageNeedsToBeUpdated = true;
}

int SettingsDialog::currentLanguage() const
{
	return ui.comboBoxLanguage->currentIndex();
}

bool SettingsDialog::languageNeedsToBeUpdated() const
{
	return m_languageNeedsToBeUpdated;
}

void SettingsDialog::changeEvent( QEvent* event )
{
	if( event->type() == QEvent::LanguageChange )
		ui.retranslateUi( this );
	else
		QDialog::changeEvent( event );
}