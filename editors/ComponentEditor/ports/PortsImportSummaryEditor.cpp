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

#include "PortsImportSummaryEditor.h"

#include <editors/ComponentEditor/ports/portsmodel.h>

#include <IPXACTmodels/Component/Component.h>

//-----------------------------------------------------------------------------
// Function: PortsImportSummaryEditor::PortsImportSummaryEditor()
//-----------------------------------------------------------------------------
PortsImportSummaryEditor::PortsImportSummaryEditor(QSharedPointer<Component> component, LibraryInterface* handler,
    ExpressionSet expressions,
    QSharedPointer<PortValidator> portValidator,
    QSharedPointer<PortsInterface> portsInterface,
    BusInterfaceInterface* busInterface,
    QWidget *parent):
PortsEditor(component, handler, expressions, portValidator, portsInterface, busInterface, parent),
originalComponent_(component)
{
}

//-----------------------------------------------------------------------------
// Function: PortsImportSummaryEditor::setComponent()
//-----------------------------------------------------------------------------
void PortsImportSummaryEditor::setComponent(QSharedPointer<Component> component)
{
    PortsEditor::setComponent(component);

    getModel()->resetModel();
}
