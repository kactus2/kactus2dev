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

public slots:

	/*!
	 *  Select all check boxes on the software side.
	 */
	void swSelectAll();

	/*!
	 *  Deselect all check boxes on the software side.
	 */
	void swDelectAll();

	/*!
	 *  Select all check boxes of the current hierarchy on the hardware side.
	 */
	void onHWSelectAll();
	
	/*!
	 *  Deselect all check boxes of the current hierarchy on the hardware side.
	 */
	void onHwDeselectAll();

	/*!
	 *  Store the current hierarchy index.
	 *
	 *      @param [in] indexHierarchy   The new hierarchy index.
	 */
	void onHierarchyChanged(int indexHierarchy);

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

	/*!
	 *  Set the check boxes to their respective lists.
	 *
	 *      @param [in] checkBoxes   The current usable check box list.
	 */
	QList <QCheckBox*> setCheckBoxes(QList <QCheckBox*> checkBoxes);

	//! A list of all used hardware hierarchical view check boxes.
	QList <QList <QCheckBox*>> hwCheckBoxes_;

	//! A list of all used software check boxes.
	QList <QCheckBox*> swCheckBoxes_;

	//! The current hierarchy index.
	int currentHierarchyIndex_;
};

#endif // COMPONENTEDITORSETTINGSPAGE_H
