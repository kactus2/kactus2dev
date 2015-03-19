/* 
 *  	Created on: 19.8.2011
 *      Author: Antti Kamppi
 * 		filename: configurationeditor.h
 *		Project: Kactus 2
 */

#ifndef CONFIGURATIONEDITOR_H
#define CONFIGURATIONEDITOR_H

#include "activevieweditor.h"
#include <IPXACTmodels/component.h>
#include <IPXACTmodels/designconfiguration.h>

#include <QWidget>
#include <QPushButton>
#include <QComboBox>
#include <QSharedPointer>
#include <QList>

class DesignWidget;
class LibraryInterface;

/*! Editor to edit the design configurations for component.
 *
 */
class ConfigurationEditor : public QWidget {
	Q_OBJECT

public:

	/*! The constructor
	 *
	 * \param handler Pointer to the instance that manages the library.
	 * \param parent Pointer to the owner of this widget.
	 *
	*/
	ConfigurationEditor(LibraryInterface* handler, QWidget *parent);
	
	//! The destructor
	virtual ~ConfigurationEditor();

	/*! Set the configuration to be displayed.
	 *
	 * \param designWidget Pointer to the design widget used to edit the design.
	 *
	*/
    void setConfiguration(DesignWidget* designWidget);

	//! Clear the contents of this editor.
	void clear();

public slots:

	//! Handler for design widget's refreshed signal.
	void onRefresh();

	//! Set the editor to locked/unlocked mode.
	void setLocked(bool locked);

signals:

	//! Emitted when the editor changes some value.
	void contentChanged();

    //! Emitted when the active configuration changes.
	void configurationChanged(QString const& newConfigName);

private slots:

	//! Handler for add button clicks.
	void onAdd();

	//! Handler for remove button clicks.
	void onRemove();

	/*! Called when user changes the current configuration.
	 *
	 * \param newConfigName The name of the new configuration to display.
	 *
	*/
	void onConfigurationChanged(QString const& newConfigName);

private:
	//! No copying
	ConfigurationEditor(const ConfigurationEditor& other);

	//! No assignment
	ConfigurationEditor& operator=(const ConfigurationEditor& other);

	//! Set up the layout of this widget
	void setuplayout();

	//! Set up the signals between widgets.
	void setupConnections();

	/*! Ask user if he wants to save the previous configuration is it was modified.
	 *
	*/
	void askSave();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

	//! Pointer to the instance that manages the library.
	LibraryInterface* handler_;

	//! Button to add new configurations to top component.
	QPushButton addNewButton_;

	//! Button to remove the selected configuration from the top component.
	QPushButton removeButton_;

	//! Combo box to select the current configuration.
	QComboBox configurationSelector_;

	//! Editor to select the active view for each component instance.
	ActiveViewEditor activeViewEditor_;

	//! Pointer to the top component being edited.
	QSharedPointer<Component> component_;

	//! Pointer to the design widget that contains the design.
	DesignWidget* designWidget_;
};

#endif // CONFIGURATIONEDITOR_H
