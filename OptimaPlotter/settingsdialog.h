#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include "ui_settingsdialog.h"

#include <qdialog.h>

class SettingsDialog : public QDialog
{
	Q_OBJECT

public:
	SettingsDialog( QWidget* parent = 0, Qt::WindowFlags windowFlags = 0 );
	~SettingsDialog();

	enum Language
	{
		SETTINGS_LANG_EN = 0,
		SETTINGS_LANG_HY
	};

	int currentLanguage() const;
	bool languageNeedsToBeUpdated() const;

private slots:
	void onLanguageChanged( int index );

private:
	virtual void changeEvent( QEvent *event );

private:
	bool m_languageNeedsToBeUpdated;
	Ui::SettingsDialog ui;
};

#endif //SETTINGSDIALOG_H