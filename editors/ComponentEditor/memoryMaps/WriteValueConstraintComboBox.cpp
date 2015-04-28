//-----------------------------------------------------------------------------
// File: WriteValueConstraintComboBox.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 28.04.2015
//
// Description:
// Combo box for the write value constraint of a field.
//-----------------------------------------------------------------------------

#include "WriteValueConstraintComboBox.h"

//-----------------------------------------------------------------------------
// Function: WriteValueConstraintComboBox::WriteValueConstraintComboBox()
//-----------------------------------------------------------------------------
WriteValueConstraintComboBox::WriteValueConstraintComboBox(QSharedPointer<WriteValueConstraint> writeConstraint,
    QWidget* parent):
QComboBox(parent),
writeConstraint_(writeConstraint)
{
    setEditable(false);

    addItem(tr("Write as read"));
    addItem(tr("Use enumerated values"));
    addItem(tr("Set minimum and maximum limits"));
    addItem(tr("No constraints"));

    setCurrentIndex(writeConstraint->getType());

    connect(this, SIGNAL(activated(int)), this, SLOT(onIndexChanged(int)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: WriteValueConstraintComboBox::~WriteValueConstraintComboBox()
//-----------------------------------------------------------------------------
WriteValueConstraintComboBox::~WriteValueConstraintComboBox()
{

}

//-----------------------------------------------------------------------------
// Function: WriteValueConstraintComboBox::onIndexChanged()
//-----------------------------------------------------------------------------
void WriteValueConstraintComboBox::onIndexChanged(int newIndex)
{
    writeConstraint_->setType(static_cast<WriteValueConstraint::Type>(newIndex));
}
