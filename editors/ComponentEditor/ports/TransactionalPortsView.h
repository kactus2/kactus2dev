//-----------------------------------------------------------------------------
// File: TransactionalPortsView.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 05.03.2024
//
// Description:
// View for transactional ports editor.
//-----------------------------------------------------------------------------

#ifndef TRANSACTIONALPORTSVIEW_H
#define TRANSACTIONALPORTSVIEW_H

#include "PortsView.h"

class TransactionalPortsView : public PortsView
{
    Q_OBJECT
public:

    // Use base class constructor.
    using PortsView::PortsView;

    virtual ~TransactionalPortsView() = default;

private slots:

    //! Handler for copy action.
    void onCopyAction() override;

    //! Handler for paste action
    virtual void onPasteAction() override;

};

#endif // TRANSACTIONALPORTSVIEW_H
