/* 
 *  	Created on: 19.8.2011
 *      Author: Antti Kamppi
 * 		filename: configurationeditor.h
 *		Project: Kactus 2
 */

#ifndef CONFIGURATIONEDITOR_H
#define CONFIGURATIONEDITOR_H

#include "activevieweditor.h"
#include <models/component.h>
#include <models/designconfiguration.h>

#include <QWidget>
#include <QPushButton>
#include <QComboBox>
#include <QSharedPointer>
#include <QList>

class DesignWidget;
class LibraryInterface;

/*! \brief Editor to edit the design configurations for component.
 *
 */
class ConfigurationEditor : public QWidget {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param handler Pointer to the instance that manages the library.
	 * \param parent Pointer to the owner of this widget.
	 *
	*/
	ConfigurationEditor(LibraryInterface* handler, QWidget *parent);
	
	//! \brief The destructor
	virtual ~ConfigurationEditor();

	/*! \brief Set the configuration to be displayed.
	 *
	 * \param designWidget Pointer to the design widget used to edit the design.
	 * \param locked If true then the configuration is only displayed, and can't be
	 * edited.
	 *
	*/
	void setConfiguration(DesignWidget* designWidget, bool locked);

	/*! \brief Clear the contents of this editor.
	 *
	*/
	void clear();

public slots:

	//! \brief Handler for design widget's refreshed signal.
	void onRefresh();

	//! \brief Set the editor to locked/unlocked mode.
	void setLocked(bool locked);

signals:

	//! \brief Emitted when the editor changes some value.
	void contentChanged();

private slots:

	//! \brief Handler for add button clicks.
	void onAdd();

	//! \brief Handler for remove button clicks.
	void onRemove();

	/*! \brief Called when user changes the current configuration.
	 *
	 * \param newConfigName The name of the new configuration to display.
	 *
	*/
	void onConfigurationChanged(const QString& newConfigName);

private:
	//! \brief No copying
	ConfigurationEditor(const ConfigurationEditor& other);

	//! \brief No assignment
	ConfigurationEditor& operator=(const ConfigurationEditor& other);

	//! \brief Set up the layout of this widget
	void setuplayout();

	//! \brief Set up the signals between widgets.
	void setupConnections();

	/*! \brief Ask user if he wants to save the previous configuration is it was modified.
	 *
	*/
	void askSave();

	//! \brief Pointer to the instance that manages the library.
	LibraryInterface* handler_;

	//! \brief Button to add new configurations to top component.
	QPushButton addNewButton_;

	//! \brief Button to remove the selected configuration from the top component.
	QPushButton removeButton_;

	//! \brief Combo box to select the current configuration.
	QComboBox configurationSelector_;

	//! \brief Editor to select the active view for each component instance.
	ActiveViewEditor activeViewEditor_;

	//! \brief Pointer to the top component being edited.
	QSharedPointer<Component> component_;

	//! \brief List of design configurations the top component contains.
	//QList<QSharedPointer<DesignConfiguration> > designConfs_;

	//! \brief Pointer to the design widget that contains the design.
	DesignWidget* designWidget_;
};

#endif // CONFIGURATIONEDITOR_H
