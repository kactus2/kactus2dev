//-----------------------------------------------------------------------------
// File: WirePortsView.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 16.02.2024
//
// Description:
// View for wire ports editor.
//-----------------------------------------------------------------------------

#ifndef WIREPORTSVIEW_H
#define WIREPORTSVIEW_H

#include "PortsView.h"

class WirePortsView : public PortsView
{
    Q_OBJECT
public:

    // Use base class constructor.
    using PortsView::PortsView;

    virtual ~WirePortsView() = default;

private slots:

    //! Handler for copy action.
    void onCopyAction() override;

    //! Handler for paste action
    virtual void onPasteAction() override;

};

#endif // WIREPORTSVIEW_H

