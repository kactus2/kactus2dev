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

class RegisterInterface;

class AccessPoliciesEditor : public QGroupBox
{
    Q_OBJECT

public:

    AccessPoliciesEditor(RegisterInterface* interface_, QString const& registerName, QWidget* parent);

    virtual ~AccessPoliciesEditor() = default;

    AccessPoliciesEditor(AccessPoliciesEditor const& other) = delete;
    AccessPoliciesEditor& operator=(AccessPoliciesEditor const& other) = delete;

signals:

    /*!
     *	Emitted whenever an access policy has been edited.
     */
    void contentChanged();

};


#endif // ACCESSPOLICIESEDITOR_H
