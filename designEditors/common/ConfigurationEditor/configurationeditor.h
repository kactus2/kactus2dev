//-----------------------------------------------------------------------------
// File: configurationeditor.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 19.08.2011
//
// Description:
// Editor to edit the design configurations for component.
//-----------------------------------------------------------------------------

#ifndef CONFIGURATIONEDITOR_H
#define CONFIGURATIONEDITOR_H

#include "activevieweditor.h"

#include <QWidget>
#include <QPushButton>
#include <QComboBox>
#include <QSharedPointer>

class DesignWidget;
class LibraryInterface;
class Component;
class View;
class SWView;
class DesignConfiguration;

//-----------------------------------------------------------------------------
//! Editor to edit the design configurations for component.
//-----------------------------------------------------------------------------
class ConfigurationEditor : public QWidget
{
	Q_OBJECT

public:

	/*!
     *  The constructor.
	 *
	 *      @param [in] handler     Pointer to the instance that manages the library.
	 *      @param [in] parent      Pointer to the owner of this widget.
	 */
	ConfigurationEditor(LibraryInterface* handler, QWidget *parent);
	
	//! The destructor.
	virtual ~ConfigurationEditor();

	/*!
     *  Set the configuration to be displayed.
	 *
	 *      @param [in] designWidget    Pointer to the design widget used to edit the design.
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

    //! Emitted when the currently active design configuration changes.
    void designConfigurationChanged(QSharedPointer<DesignConfiguration> newConfiguration);

private slots:

	//! Handler for add button clicks.
	void onAdd();

	//! Handler for remove button clicks.
	void onRemove();

	/*!
     *  Called when user changes the current configuration.
	 *
	 *      @param [in] newConfigName   The name of the new configuration to display.
	 */
	void onConfigurationChanged(QString const& newConfigName);

private:
	//! No copying.
	ConfigurationEditor(const ConfigurationEditor& other);

	//! No assignment.
	ConfigurationEditor& operator=(const ConfigurationEditor& other);

	//! Set up the layout of this widget.
	void setuplayout();

	//! Set up the signals between widgets.
	void setupConnections();

	/*!
     *  Ask user if he wants to save the previous configuration is it was modified.
	 */
	void askSave();

    /*!
     *  Remove design and design configuration instantiation references from the selected view.
     *
     *      @param [in] viewName    The name of the selected view.
     */
    void removeViewHierarchicalInstantiationRefs(QString const& viewName) const;

    /*!
     *  Finds a component sw view matching the given name.
     *
     *      @param [in] viewName    The name of the sw view being searched.
     *
     *      @return SW view matching the given name.
     */
    QSharedPointer<SWView> findComponentSWView(QString const& viewName) const;

    /*!
     *  Remove a view from component.
     *
     *      @param [in] viewName    The name of the view being removed.
     */
    void removeViewFromComponent(QString const& viewName) const;

    /*!
     *  Remove an sw view from component.
     *
     *      @param [in] viewName    The name of the sw view being removed.
     */
    void removeSWViewFromComponent(QString const& viewName) const;

    /*!
     *  Remove a system view from component.
     *
     *      @param [in] viewName    The name of the system view being removed.
     */
    void removeSystemViewFromComponent(QString const& viewName) const;

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
