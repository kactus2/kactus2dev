//-----------------------------------------------------------------------------
// File: FieldAccessPoliciesEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: 
// Date: 8.8.2023
//
// Description:
// 
//-----------------------------------------------------------------------------

#ifndef FIELDACCESSPOLICIESEDITOR_H
#define FIELDACCESSPOLICIESEDITOR_H

#include <common/views/EditableTableView/editabletableview.h>

#include <QGroupBox>

class FieldInterface;

class FieldAccessPoliciesEditor : public QGroupBox
{
    Q_OBJECT

public:

    FieldAccessPoliciesEditor(QString const& fieldName, FieldInterface* fieldInterface, QWidget* parent);

private:

    //! The table view to display the field access policies.
    EditableTableView* view_;
};

#endif // FIELDACCESSPOLICIESEDITOR_H