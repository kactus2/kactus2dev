//-----------------------------------------------------------------------------
// File: MirroredMasterModeEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 06.03.2017
//
// Description:
// Display for mirrored master bus interface.
//-----------------------------------------------------------------------------

#include "MirroredMasterModeEditor.h"

#include <QLabel>
#include <QString>
#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: MirroredMasterModeEditor::MirroredMasterModeEditor()
//-----------------------------------------------------------------------------
MirroredMasterModeEditor::MirroredMasterModeEditor(BusInterfaceInterface* busInterface,
    std::string const& busName, QWidget* parent):
ModeEditorBase(busInterface, busName, tr("Mirrored master"), parent)
{
    QLabel* textLabel = new QLabel(tr("No mode specific options."));

    QVBoxLayout* verticalLayout = new QVBoxLayout(this);
    verticalLayout->addWidget(textLabel, 0, Qt::AlignTop);
}

//-----------------------------------------------------------------------------
// Function: MirroredMasterModeEditor::isValid()
//-----------------------------------------------------------------------------
bool MirroredMasterModeEditor::isValid() const
{
    return true;
}

//-----------------------------------------------------------------------------
// Function: MirroredMasterModeEditor::refresh()
//-----------------------------------------------------------------------------
void MirroredMasterModeEditor::refresh()
{
    // Intentionally empty.
}

//-----------------------------------------------------------------------------
// Function: MirroredMasterModeEditor::getInterfaceMode()
//-----------------------------------------------------------------------------
General::InterfaceMode MirroredMasterModeEditor::getInterfaceMode() const
{
    return General::MIRRORED_MASTER;
}

//-----------------------------------------------------------------------------
// Function: MirroredMasterModeEditor::saveModeSpecific()
//-----------------------------------------------------------------------------
void MirroredMasterModeEditor::saveModeSpecific()
{
    // Intentionally empty.
}
