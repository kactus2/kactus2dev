//-----------------------------------------------------------------------------
// File: TypedPortsView.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 16.02.2024
//
// Description:
// View for typed ports editors.
//-----------------------------------------------------------------------------

#ifndef TYPEDPORTSVIEW_H
#define TYPEDPORTSVIEW_H

#include "PortsView.h"

class BusInterfaceInterface;

class TypedPortsView : public PortsView
{
    Q_OBJECT
public:

    // Use base class constructor.
    TypedPortsView(int typeColumn, int nameColumn, BusInterfaceInterface* busInterface, QWidget* parent);

    virtual ~TypedPortsView() = default;

private slots:

    //! Handler for copy action.
    virtual void onCopyAction() override;

    //! Handler for paste action
    virtual void onPasteAction() override;

private:

    //! The number of the column containing the type definitions information.
    int typeColumn_;
};

#endif // TYPEDPORTSVIEW_H

