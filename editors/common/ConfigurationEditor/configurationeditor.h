//-----------------------------------------------------------------------------
// File: configurationeditor.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 19.08.2011
//
// Description:
// Editor to edit the design configurations for component.
//-----------------------------------------------------------------------------

#ifndef CONFIGURATIONEDITOR_H
#define CONFIGURATIONEDITOR_H

#include "activevieweditor.h"

#include <common/dialogs/createConfigurationDialog/createconfigurationdialog.h>

#include <QComboBox>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QSharedPointer>
#include <QWidget>

class DesignWidget;
class LibraryInterface;
class Component;
class View;
class DesignConfiguration;
class VLNVDisplayer;

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
	 *      @param [in] handler     The instance that manages the library.
	 *      @param [in] parent      The owner of this widget.
	 */
	ConfigurationEditor(LibraryInterface* handler, QWidget *parent);

    //! No copying.
    ConfigurationEditor(const ConfigurationEditor& other) = delete;

	//! The destructor.
	virtual ~ConfigurationEditor() = default;

    //! No assignment.
    ConfigurationEditor& operator=(const ConfigurationEditor& other) = delete;

	/*!
     *  Set the configuration to be displayed.
	 *
	 *      @param [in] designWidget    The design widget used to edit the design.
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

	/*!
     *  Called when user changes the current configuration.
	 *
	 *      @param [in] newConfigName   The name of the new configuration to display.
	 */
	void onConfigurationChanged(QString const& newConfigName);

private:
    
    /*!
     *  Finds the view names possible for the current design.
     *
     *      @return The possible view names.
     */
    QStringList findPossibleViews();
 
    /*!
     *  Creates a new design configuration and saves it in the library.
     *
     *      @param [in] dialog   The dialog containing the configuration details.
     */
    void createAndSaveConfiguration(CreateConfigurationDialog const& dialog);

    /*!
     *  Creates a reference to the configuration in the top component.
     *
     *      @param [in] configurationVLNV       The VLNV of the configuration.
     *      @param [in] viewName                The name of the view referencing the configuration.
     *      @param [in] designImplementation    The implementation type of the design.
     */
    void createTopLevelReferenceToConfiguration(VLNV const& configurationVLNV, QString const& viewName, 
        const KactusAttribute::Implementation designImplementation);

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
     *  Remove a view from component.
     *
     *      @param [in] viewName    The name of the view being removed.
     */
    void removeViewFromComponent(QString const& viewName) const;

    /*!
     *  Remove a system view from component.
     *
     *      @param [in] viewName    The name of the system view being removed.
     */
    void removeSystemViewFromComponent(QString const& viewName) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

	//! The instance that manages the library.
	LibraryInterface* library_;

	//! Button to add new configurations to top component.
	QPushButton* addNewButton_;

	//! Combo box to select the current configuration.
	QComboBox* configurationSelector_;

    VLNVDisplayer* configurationDisplay_;

	//! Editor to select the active view for each component instance.
	ActiveViewEditor* activeViewEditor_;

	//! The top component being edited.
	QSharedPointer<Component> topComponent_;

	//! The design widget that contains the design.
	DesignWidget* designWidget_;
};

#endif // CONFIGURATIONEDITOR_H
