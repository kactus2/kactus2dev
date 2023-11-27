//-----------------------------------------------------------------------------
// File: TypedPortEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 23.11.2023
//
// Description:
// Editor for wire/transactional/structured ports.
//-----------------------------------------------------------------------------

#ifndef TYPED_PORT_EDITOR_H
#define TYPED_PORT_EDITOR_H

#include <KactusAPI/include/ParameterFinder.h>
#include <KactusAPI/include/ExpressionFormatter.h>

#include <editors/ComponentEditor/itemeditor.h>
#include <editors/common/ExpressionSet.h>
#include <editors/ComponentEditor/ports/MasterPortsEditor.h>

#include <QTabWidget>

class BusInterface;
class BusInterfaceWizard;
class Component;
class LibraryInterface;
class PortValidator;
class MasterPortsEditor;
class PortsInterface;
class BusInterfaceInterface;
class IPXactSystemVerilogParser;
class PortsEditorFactory;

//-----------------------------------------------------------------------------
//! Editor for wire/transactional/structured ports.
//-----------------------------------------------------------------------------
class TypedPortEditor : public ItemEditor
{
	Q_OBJECT

public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] component               The component being edited.
	 *      @param [in] handler                 The instance that manages the library.
	 *      @param [in] editorFactory           The factory for creating editor content.
     *      @param [in] portType                Type of the edited ports: wire/transactional/structured.
     *      @param [in] portsInterface          Interface for accessing ports.
     *      @param [in] busInterface            Interface for accessing bus interfaces.
	 *      @param [in] parent                  The owner of this widget.
	 */
	TypedPortEditor(QSharedPointer<Component> component,
        LibraryInterface* handler,
        PortsEditorFactory const* editorFactory,
        QString const& portType,
        QSharedPointer<PortsInterface> portsInterface,
        BusInterfaceInterface* busInterface,
        QWidget *parent = 0);

	/*!
     *  The destructor.
     */
	~TypedPortEditor() = default;

	/*!
     *  Check for the validity of the edited ports.
     *
     *      @return True if all ports are in valid state.
     */
	bool isValid() const;

	/*!
     *  Reload the information from the model to the editor.
	 */
	void refresh() final;

	/*!
     *  Enable/disable the import/export csv functionality for the view.
	 *
	 *      @param [in] allow If true then import/export is enabled.
	 */
	void setAllowImportExport(bool allow);

signals:

    /*!
     *  Emitted when a new interface should be added to the component editor tree view.
     */
    void createInterface();

protected:

    /*!
     *  Called when the editor is shown.
     */
    void showEvent(QShowEvent* event) final;

private slots:

    /*!
     *  Handler for new interface creation for new bus definition.
     *
     *      @param [in] selectedPorts   Ports to be mapped in the interface.
     */
    void onCreateNewInteface(QStringList const& selectedPorts);

    /*!
     *  Handler for new interface creation for existing bus definition.
     *
     *      @param [in] selectedPorts   Ports to be mapped in the interface.
     */
    void onCreateInterface(QStringList const& selectedPorts);

    /*!
     *  Change the vendor extensions of the vendor extensions editor.
     *
     *      @param [in] itemIndex   Index of the selected port.
     */
    void changeExtensionsEditorItem(QModelIndex const& itemIndex);


private:

    /*!
     *  Open the bus interface wizard.
     *
     *      @param [in] busIf   The new bus interface.
     *      @param [in] wizard  The selected interface wizard.
     */
    void openBusInterfaceWizard(QSharedPointer<BusInterface> busIf, BusInterfaceWizard& wizard);

    /*!
     *  Setup the editor layout.
     */
    void setupLayout();


    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Interface for accessing bus interfaces.
    BusInterfaceInterface* busInterface_{ nullptr };

    //! Editor for the ports.
    MasterPortsEditor portEditor_;

    //! Type of the edited ports: wire/transactional/structured.
    QString portType_;
};

#endif // TYPED_PORT_EDITOR_H
