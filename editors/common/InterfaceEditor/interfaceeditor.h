//-----------------------------------------------------------------------------
// File: interfaceeditor.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 10.10.2011
//
// Description:
// Editor to display/edit the details of a bus/api/com interface.
//-----------------------------------------------------------------------------

#ifndef INTERFACEEDITOR_H
#define INTERFACEEDITOR_H

#include <common/widgets/vlnvDisplayer/vlnvdisplayer.h>

#include <editors/ComponentEditor/software/PropertyValueEditor.h>

#include <KactusAPI/include/LibraryInterface.h>

#include <QWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QLabel>
#include <QPlainTextEdit>
#include <QStackedWidget>

class ActiveInterface;
class ComDefinition;
class Design;
class HierInterface;
class ConnectionEndpoint;
class EditableTableView;
class IEditProvider;
class InterfacePortMapModel;
class DesignConfiguration;

//-----------------------------------------------------------------------------
//! Editor to display/edit the details of a bus/api/com interface.
//-----------------------------------------------------------------------------
class InterfaceEditor : public QStackedWidget
{
	Q_OBJECT

public:

	/*!
     *  The constructor.
	 *
	 *      @param [in] parent      The owner of this widget.
     *      @param [in] handler     The library handler.
	 */
	InterfaceEditor(QWidget *parent, LibraryInterface* handler);
	
	//! The destructor.
	virtual ~InterfaceEditor() = default;

	/*!
     *  Set the interface for the editor.
	 *
	 *      @param [in] interface           The interface to edit.
     *      @param [in] containingDesign    The design containing the end point.
     *      @param [in] configuration       Design configuration of the design.
     *      @param [in] activeTopView       The currently active top component view.
	 */
	void setInterface(ConnectionEndpoint* interface, QSharedPointer<Design> containingDesign,
        QSharedPointer<DesignConfiguration> configuration, QString const& activeTopView,
        QSharedPointer<IEditProvider> editProvider, bool locked);

signals:

    //! Informs of a content change in the editor.
    void contentChanged();

public slots:

	//! Refresh the editor contents without changing the interface.
	void refresh();

	//! Clear the editor so no interface details are shown
    void clear();

private slots:

	/*!
     *  Handler for change in interface name.
	 *
	 *      @param [in] newName     The new name of the interface.
	 */
	void onInterfaceNameChanged(QString const& newName);

	/*!
     *  Handler for change in interface mode.
	 *
	 *      @param [in] newMode     The new mode of the interface.
	 */
	void onInterfaceModeChanged(QString const& newMode);

	/*!
     *  Handler for changes in the description editor.
	 */
	void onDescriptionChanged();

    //! Handler for changes in dependency direction.
    void onDependencyDirectionChanged(QString const& newDir);

    //! Handler for changes in COM direction.
    void onComDirectionChanged(QString const& newDir);

    //! Handler for changes in COM transfer type.
    void onComTransferTypeChanged(QString const& newTransferType);

    //! Handler for changes in COM property values.
    void onComPropertyValuesChanged();

private:
	//! No copying.
	InterfaceEditor(const InterfaceEditor& other);

	//! No assignment.
	InterfaceEditor& operator=(const InterfaceEditor& other);

    // Sets the editor for editing a bus interface.
    void setBusInterface();
        
    /*!
     *  Sets the name and description of the interface to the given editors.
     *
     *      @param [in] nameEditor          The name editor to use.
     *      @param [in] descriptionEditor   The description editor to use.
     */
    void setNameAndDescription(QLineEdit* nameEditor, QPlainTextEdit* descriptionEditor);

    /*!
     *  Get a list of active interfaces referencing the selected interface.
     *
     *      @return A list of active interfaces referencing the selected interface.
     */
    QList<QSharedPointer<ActiveInterface> > getActiveInterfaces() const;

    /*!
     *  Check if the selected active interface contains a reference to the selected end point.
     *
     *      @param [in] currentInterface    The selected active interface.
     *      @param [in] endPoint            The selected end point.
     *
     *      @return True, if the active interface contains a reference to the end point, otherwise false.
     */
    bool activeInterfaceReferencesBusInterface(QSharedPointer<ActiveInterface> currentInterface) const;

    QList<QSharedPointer<HierInterface> > getDesignInterfaces() const;

    // Sets the editor for editing a COM interface.
    void setComInterface();

    // Sets the editor for editing an API interface.
    void setApiInterface();

    // Initializes the editor for editing a bus interface.
    void setupBusEditor();

    // Initializes the editor for editing a COM interface.
    void setupComEditor();

    // Initializes the editor for editing an API interface.
    void setupApiEditor();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The currently active top component view.
    QString activeTopView_;

    //! The instance that manages the library.
    LibraryInterface* library_;

	//! Widget to display the bus type.
	VLNVDisplayer busType_;

	//! Widget to display the abstraction type.
	VLNVDisplayer absType_;

    //! The editor for the bus interface name.
    QLineEdit busNameEditor_;

    //! The editor for the interface mode.
    QComboBox modeSelector_;

    //! The view for the port mappings.
    EditableTableView portMapsView_;

    //! The model for the port mappings.
    InterfacePortMapModel* portMapsModel_;

    //! Editor for the description of the interface.
    QPlainTextEdit busDescriptionEditor_;

    //! Widget to display the COM type.
    VLNVDisplayer comType_;

    //! The editor for the COM interface name.
    QLineEdit comNameEditor_;
    
    //! Data type editor for COM interface.
    QComboBox transferTypeCombo_;

    //! The editor for the COM direction.
    QComboBox comDirectionCombo_;

    //! Editor for the description of the COM interface.
    QPlainTextEdit comDescriptionEditor_;

    //! Widget to display the API type.
    VLNVDisplayer apiType_;

    //! The editor for the api interface name.
    QLineEdit apiNameEditor_;

    //! The editor for dependency direction.
    QComboBox dependencyDirCombo_;

    //! Editor for the description of the interface.
    QPlainTextEdit apiDescriptionEditor_;

    //! Property value editor for SW properties.
    PropertyValueEditor propertyValueEditor_;

    //! The interface being edited.
    ConnectionEndpoint* interface_;

    //! The design containing the selected interface.
    QSharedPointer<Design> containingDesign_;

    //! The active design configuration.
    QSharedPointer<DesignConfiguration> containingConfiguration_;

    //! Undo/redo manager to use.
    QSharedPointer<IEditProvider> editProvider_;

    //! Set when the widget should be locked.
    bool locked_;
};

#endif // INTERFACEEDITOR_H
