//-----------------------------------------------------------------------------
// File: RemapModeReferenceEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 17.10.2023
//
// Description:
// Editor for remap mode references.
//-----------------------------------------------------------------------------

#include "RemapModeReferenceEditor.h"

#include <editors/ComponentEditor/common/ModeReferenceEditor.h>

#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: RemapModeReferenceEditor::RemapModeReferenceEditor()
//-----------------------------------------------------------------------------
RemapModeReferenceEditor::RemapModeReferenceEditor(ModeReferenceInterface* modeRefInterface, QWidget* parent):
    QGroupBox(tr("Mode references"), parent),
    modeReferencesEditor_(new ModeReferenceEditor(modeRefInterface, parent))
{
    QVBoxLayout* topLayout = new QVBoxLayout(this);
    topLayout->addWidget(modeReferencesEditor_);

    modeReferencesEditor_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    connect(modeReferencesEditor_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
}
