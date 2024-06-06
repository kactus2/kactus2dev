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
#include <KactusAPI/include/ModeReferenceInterface.h>

#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: RemapModeReferenceEditor::RemapModeReferenceEditor()
//-----------------------------------------------------------------------------
RemapModeReferenceEditor::RemapModeReferenceEditor(ModeReferenceInterface* modeRefInterface, QWidget* parent, bool isRemap):
    GroupBoxWithAlertSign(tr("Mode references"), parent),
    modeRefInterface_(modeRefInterface),
    modeReferencesEditor_(new ModeReferenceEditor(modeRefInterface, parent))
{
    QVBoxLayout* topLayout = new QVBoxLayout(this);
    topLayout->addWidget(modeReferencesEditor_);

    modeReferencesEditor_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    connect(modeReferencesEditor_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    if (isRemap)
    {
        connect(modeReferencesEditor_, SIGNAL(contentChanged()), this, SLOT(checkForAlert()), Qt::UniqueConnection);
        checkForAlert();
    }
}

void RemapModeReferenceEditor::checkForAlert()
{
    if (modeRefInterface_->areAllModeReferencesValid())
    {        
        removeAlertIcon();
    }
    else 
    {
        setAlertIcon();
    }
}