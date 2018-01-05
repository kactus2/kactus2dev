//-----------------------------------------------------------------------------
// File: PortMapTreeView.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 23.05.2016
//
// Description:
// A view for bus interface port maps.
//-----------------------------------------------------------------------------

#ifndef PORTMAPTREEVIEW_H
#define PORTMAPTREEVIEW_H

#include <common/views/EditableTreeView/EditableTreeView.h>

#include <QTreeView>
#include <QModelIndex>
#include <QKeyEvent>
#include <QContextMenuEvent>
#include <QAction>

class PortMapsTreeModel;

//-----------------------------------------------------------------------------
//! A tree view for the port map tree model.
//-----------------------------------------------------------------------------
class PortMapTreeView : public EditableTreeView
{
    Q_OBJECT

public:
   
    /*!
     *  The constructor.
     *
     *      @param [in] parent  The parent widget.
     */
    PortMapTreeView(QWidget* parent);

    /*!
     *  The destructor.
     */
    ~PortMapTreeView();

protected:
    
    /*!
     *  Handles key press events.
     *
     *      @param [in] event   The key event.
     */
    virtual void keyPressEvent(QKeyEvent* event);

    /*!
     *  Handler for context menu events.
     *
     *      @param [in] event   The context menu event.
     */
    virtual void contextMenuEvent(QContextMenuEvent* event);

    /*!
     *  Handler for drag enter events
	 *
	 *      @param [in] event The event object.
     */
	virtual void dragEnterEvent(QDragEnterEvent* event);

signals:

    /*!
     *  Automatically create port maps for the selected logical signals.
     *
     *      @param [in] logicalSignals  The selected logical signals.
     */
    void autoConnecteLogicalSignals(QStringList const& logicalSignals);

private slots:

    /*!
     *  Handler for expanding all items.
     */
    void onExpandAll();

    /*!
     *  Handler for collapsing all items.
     */
    void onCollapseAll();

    /*!
     *  Handler for automatically creating port maps for selected logical signals.
     */
    void onAutoConnect();

private:
   
    // Disable copying.
    PortMapTreeView(PortMapTreeView const& rhs);
    PortMapTreeView& operator=(PortMapTreeView const& rhs);

    /*!
     *  Setup the available context menu actions.
     */
    void setupActions();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The action for expanding all items.
    QAction expandAllItemsAction_;

    //! The action for collapsing all items.
    QAction collapseAllItemsAction_;

    //! The action for automatically connecting logical signals to physical ports.
    QAction autoConnectAction_;
};

//-----------------------------------------------------------------------------

#endif // PORTMAPTREEVIEW_H
