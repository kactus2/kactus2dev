/* 
 *	Created on:	24.5.2013
 *	Author:		Antti Kamppi
 *	File name:	k2envvariablesettingspage.h
 *	Project:		Kactus 2
*/

#ifndef K2ENVVARIABLESETTINGSPAGE_H
#define K2ENVVARIABLESETTINGSPAGE_H

#include "SettingsPage.h"
#include <common/views/EditableTableView/editabletableview.h>
#include <Plugins/PluginSystem/PluginManager.h>

#include <QSettings>

class K2EnvVarModel;

/*! \brief Displays the Kactus2 environment variables in settings dialog.
 *
 */
class K2EnvVariableSettingsPage : public SettingsPage 
{
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
	K2EnvVariableSettingsPage(QSettings& settings,  PluginManager& pluginMgr);
	
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

    /*! \brief Checks that all variables have a defined name.
     *
     * Method: 		prevalidate
     * Full name:	K2EnvVariableSettingsPage::prevalidate
     * Access:		virtual public 
     *
     *
     * \return False if at least one variable name is empty.
    */
    virtual bool prevalidate() const;

    /*! \brief Checks that all variables have a defined name. 
     *
     * If at least one name is empty, an error dialog is displayed.
     *
     * Method: 		validate
     * Full name:	K2EnvVariableSettingsPage::validate
     * Access:		virtual public 
     *
     *
     * \return False if at least one variable name is empty.
    */
    virtual bool validate();

private:

	//! \brief No copying
	K2EnvVariableSettingsPage(const K2EnvVariableSettingsPage& other);

	//! \brief No assignment
	K2EnvVariableSettingsPage& operator=(const K2EnvVariableSettingsPage& other);

	//! \brief Displays the environment variables.
	EditableTableView* view_;

	//! \brief The model which manages the environment variables.
	K2EnvVarModel* model_;
};

#endif // K2ENVVARIABLESETTINGSPAGE_H
