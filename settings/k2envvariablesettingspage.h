/* 
 *	Created on:	24.5.2013
 *	Author:		Antti Kamppi
 *	File name:	k2envvariablesettingspage.h
 *	Project:		Kactus 2
*/

#ifndef K2ENVVARIABLESETTINGSPAGE_H
#define K2ENVVARIABLESETTINGSPAGE_H

#include <common/dialogs/propertyPageDialog/PropertyPageView.h>
#include <common/views/EditableTableView/editabletableview.h>

#include <QSettings>

/*! \brief Displays the Kactus2 environment variables in settings dialog.
 *
 */
class K2EnvVariableSettingsPage : public PropertyPageView {
	Q_OBJECT

public:

	/*! \brief The constructor.
	 *
	 * Method: 		K2EnvVariableSettingsPage
	 * Full name:	K2EnvVariableSettingsPage::K2EnvVariableSettingsPage
	 * Access:		private 
	 *
	 * \param settings The settings to store.
	 *
	*/
	K2EnvVariableSettingsPage(QSettings& settings);
	
	//! \brief The destructor.
	virtual ~K2EnvVariableSettingsPage();

	/*! \brief Apply the changes made in the settings page.
	 *
	 * Method: 		apply
	 * Full name:	K2EnvVariableSettingsPage::apply
	 * Access:		virtual public 
	 *
	*/
	virtual void apply();

private:

	//! \brief No copying
	K2EnvVariableSettingsPage(const K2EnvVariableSettingsPage& other);

	//! \brief No assignment
	K2EnvVariableSettingsPage& operator=(const K2EnvVariableSettingsPage& other);

	//! \brief The settings for Kactus2.
	QSettings& settings_;

	//! \brief Displays the environment variables.
	EditableTableView* view_;
};

#endif // K2ENVVARIABLESETTINGSPAGE_H
