//-----------------------------------------------------------------------------
// File: AccessPoliciesEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 28.8.2023
//
// Description:
// Editor for access policies found in address blocks, registers, register files etc.
//-----------------------------------------------------------------------------

#ifndef ACCESSPOLICIESEDITOR_H
#define ACCESSPOLICIESEDITOR_H

#include <QGroupBox>

class AccessPolicyInterface;
class AccessPolicy;
class EditableTableView;

//-----------------------------------------------------------------------------
//! Editor for access policies found in address blocks, registers, register files etc.
//-----------------------------------------------------------------------------
class AccessPoliciesEditor : public QGroupBox
{
    Q_OBJECT

public:

    AccessPoliciesEditor(QSharedPointer<QList<QSharedPointer<AccessPolicy> > > accessPolicies, 
        AccessPolicyInterface* accessPolicyInterface, QWidget* parent);

    virtual ~AccessPoliciesEditor() = default;

    AccessPoliciesEditor(AccessPoliciesEditor const& other) = delete;
    AccessPoliciesEditor& operator=(AccessPoliciesEditor const& other) = delete;

    /*!
     *  Reload the information from the model to the editor.
     */
    void refresh();

signals:

    /*!
     *	Emitted whenever an access policy has been edited.
     */
    void contentChanged();

private:

    //! The available access policies.
    QSharedPointer<QList<QSharedPointer<AccessPolicy> > > accessPolicies_;

    //! The access policy table view.
    EditableTableView* view_;

    //! Interface for accessing access policies.
    AccessPolicyInterface* interface_;
};

#endif // ACCESSPOLICIESEDITOR_H
