//-----------------------------------------------------------------------------
// File: PortsImportSummaryEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 07.10.2025
//
// Description:
// Editor for displaying a summary of ports in import.
//-----------------------------------------------------------------------------

#ifndef PORTSIMPORTSUMMARYEDITOR_H
#define PORTSIMPORTSUMMARYEDITOR_H

#include <editors/ComponentEditor/ports/portseditor.h>

//-----------------------------------------------------------------------------
//! Editor for displaying a summary of ports in import.
//-----------------------------------------------------------------------------
class PortsImportSummaryEditor : public PortsEditor
{
	Q_OBJECT

public:

	/*!
	 *  The constructor.
	 *
	 *    @param [in] component         The component being edited.
	 *    @param [in] handler           The instance that manages the library.
	 *    @param [in] expressions       Expression handling utilities.
     *    @param [in] portValidator     Validator used for ports.
     *    @param [in] portsInterface    Interface for accessing ports.
     *    @param [in] busInterface      Interface for accessing bus interfaces.
	 *    @param [in] parent            The owner of this widget.
	 */
    PortsImportSummaryEditor(QSharedPointer<Component> component,
        LibraryInterface* handler,
        ExpressionSet expressions,
        QSharedPointer<PortValidator> portValidator,
        QSharedPointer<PortsInterface> portsInterface,
        BusInterfaceInterface* busInterface,
        QWidget *parent = 0);

	/*!
     *  The destructor.
     */
    virtual ~PortsImportSummaryEditor() = default;

    /*!
     *  Sets the edited component.
     *
     *    @param [in] component   The component whose ports to edit.
     */
    virtual void setComponent(QSharedPointer<Component> component);

    //! No copying.
    PortsImportSummaryEditor(const PortsImportSummaryEditor& other) = delete;
    PortsImportSummaryEditor& operator=(const PortsImportSummaryEditor& other) = delete;

private:

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------


    //! The original component.
    QSharedPointer<Component> originalComponent_ = { nullptr };

};

#endif // PORTSIMPORTSUMMARYEDITOR_H
