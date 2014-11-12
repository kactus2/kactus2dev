//-----------------------------------------------------------------------------
// File: ComponentEditorSettingsPage.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 11.11.2014
//
// Description:
// Component editor property settings page.
//-----------------------------------------------------------------------------
#ifndef COMPONENTEDITORSETTINGSPAGE_H
#define COMPONENTEDITORSETTINGSPAGE_H

#include "SettingsPage.h"
#include "ComponentEditorSettingsPage.h"

#include <QCheckBox>

class ComponentEditorSettingsPage : public SettingsPage
{
	Q_OBJECT

public:
	ComponentEditorSettingsPage(QSettings &settings);
	
	~ComponentEditorSettingsPage();

	/*!
	 *  Applies the changes that were done to the page.
	 */
	void apply();

private:
	//! Disable copying.
	ComponentEditorSettingsPage(ComponentEditorSettingsPage const& rhs);
	ComponentEditorSettingsPage& operator=(ComponentEditorSettingsPage const& rhs);

	/*!
	 *  Sets the page layout.
	 */
	void setupLayOut();

	/*!
	 *  Loads the current settings.
	 */
	void loadSettings();

	//! A list of all used check boxes.
	QList <QCheckBox*> checkBoxes_;
};

#endif // COMPONENTEDITORSETTINGSPAGE_H
