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
class PortMapInterface;

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
     *      @param [in] portMapInterface    Interface for accessing port maps.
     *      @param [in] parent              The parent widget.
     */
    PortMapTreeView(PortMapInterface* portMapInterface, QWidget* parent);

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
    virtual void keyPressEvent(QKeyEvent* event) override final;

    /*!
     *  Handler for context menu events.
     *
     *      @param [in] event   The context menu event.
     */
    virtual void contextMenuEvent(QContextMenuEvent* event) override final;

    /*!
     *  Handler for drag enter events
	 *
	 *      @param [in] event The event object.
     */
	virtual void dragEnterEvent(QDragEnterEvent* event) override final;

signals:

    /*!
     *  Automatically create port maps for the selected logical signals.
     *
     *      @param [in] logicalSignals  The selected logical signals.
     */
    void autoConnecteLogicalSignals(QStringList const& logicalSignals);

private slots:

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

    //! The action for automatically connecting logical signals to physical ports.
    QAction autoConnectAction_;

    //! Interface for accessing port maps.
    PortMapInterface* mapInterface_;
};

//-----------------------------------------------------------------------------

#endif // PORTMAPTREEVIEW_H
