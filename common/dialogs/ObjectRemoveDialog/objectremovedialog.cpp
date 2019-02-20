//-----------------------------------------------------------------------------
// File: ObjectRemoveDialog.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 08.08.2011
//
// Description:
// Dialog for selecting items to be removed.
//-----------------------------------------------------------------------------

#include "objectremovedialog.h"

//-----------------------------------------------------------------------------
// Function: objectremovedialog::ObjectRemoveDialog()
//-----------------------------------------------------------------------------
ObjectRemoveDialog::ObjectRemoveDialog(QWidget *parent, Qt::WindowFlags f):
ObjectSelectionDialog(tr("Delete"), QStringLiteral(":icons/common/graphics/delete.png"),
    QStringLiteral(":icons/common/graphics/cross.png"), tr("Select items to delete"), parent, f)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    setupLayout();
}

//-----------------------------------------------------------------------------
// Function: objectremovedialog::~ObjectRemoveDialog()
//-----------------------------------------------------------------------------
ObjectRemoveDialog::~ObjectRemoveDialog()
{

}

//-----------------------------------------------------------------------------
// Function: objectremovedialog::setupLayout()
//-----------------------------------------------------------------------------
void ObjectRemoveDialog::setupLayout()
{
    QString introLabel = tr("Delete");
    QString introText = tr("Delete the selected items. The items will be removed from the library and the hard drive.");
    QWidget* introWidget = setupIntroWidget(introLabel, introText);

    QVBoxLayout* overallLayout = new QVBoxLayout(this);
    overallLayout->addWidget(introWidget);
    overallLayout->addWidget(getItemList(), 1);
    overallLayout->addLayout(setupButtonLayout());
}
