//-----------------------------------------------------------------------------
// File: MirroredInitiatorModeEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 29.6.2023
//
// Description:
// Editor to edit mirrored initiator settings of a bus interface.
//-----------------------------------------------------------------------------

#include "MirroredInitiatorModeEditor.h"

#include <QLabel>
#include <QString>
#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: MirroredInitiatorModeEditor::MirroredInitiatorModeEditor()
//-----------------------------------------------------------------------------
MirroredInitiatorModeEditor::MirroredInitiatorModeEditor(BusInterfaceInterface* busInterface,
    std::string const& busName, QWidget* parent):
ModeEditorBase(busInterface, busName, tr("Mirrored initiator"), parent)
{
    QLabel* textLabel = new QLabel(tr("No mode specific options."));

    QVBoxLayout* verticalLayout = new QVBoxLayout(this);
    verticalLayout->addWidget(textLabel, 0, Qt::AlignTop);
}

//-----------------------------------------------------------------------------
// Function: MirroredInitiatorModeEditor::isValid()
//-----------------------------------------------------------------------------
bool MirroredInitiatorModeEditor::isValid() const
{
    return true;
}

//-----------------------------------------------------------------------------
// Function: MirroredInitiatorModeEditor::refresh()
//-----------------------------------------------------------------------------
void MirroredInitiatorModeEditor::refresh()
{
    // Intentionally empty.
}

//-----------------------------------------------------------------------------
// Function: MirroredInitiatorModeEditor::getInterfaceMode()
//-----------------------------------------------------------------------------
General::InterfaceMode MirroredInitiatorModeEditor::getInterfaceMode() const
{
    return General::MIRRORED_INITIATOR;
}

//-----------------------------------------------------------------------------
// Function: MirroredInitiatorModeEditor::saveModeSpecific()
//-----------------------------------------------------------------------------
void MirroredInitiatorModeEditor::saveModeSpecific()
{
    // Intentionally empty.
}
