//-----------------------------------------------------------------------------
// File: PortMapTreeView.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 23.05.2016
//
// Description:
// A view for bus interface port maps.
//-----------------------------------------------------------------------------

#ifndef PORTMAPTREEVIEW_H
#define PORTMAPTREEVIEW_H

#include <designEditors/common/ComponentInstanceEditor/ConfigurableElementsView.h>

#include <QTreeView>
#include <QModelIndex>
#include <QKeyEvent>
#include <QContextMenuEvent>
#include <QAction>

class PortMapsTreeModel;

//-----------------------------------------------------------------------------
//! A tree view for the port map tree model.
//-----------------------------------------------------------------------------
class PortMapTreeView : public ConfigurableElementsView
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
     *  Add a new port map at the selected position.
     *
     *      @param [in] position    The parent index of the new port map.
     */
    void addItem(QModelIndex const& position);

    /*!
     *  Remove all port maps from the selected logical port.
     *
     *      @param [in] position    The index of the selected logical port.
     */
    void removeAllChildItemsFromIndex(QModelIndex const& position);

    /*!
     *  Automatically create port maps for the selected logical signals.
     *
     *      @param [in] logicalSignals  The selected logical signals.
     */
    void autoConnecteLogicalSignals(QStringList const& logicalSignals);

private slots:

    /*!
     *  Handler for removing all port maps from the selected logical port.
     */
    void onRemoveAllPortMaps();

    /*!
     *  Handler for removing a port map.
     */
    void onRemovePortMap();

    /*!
     *  Handler for adding a port map.
     */
    void onAddPortMap();

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

    //! The action for removing all port maps from a selected logical port.
    QAction removeAllMapsAction_;

    //! The action for adding a port map.
    QAction addMapAction_;

    //! The action for expanding all items.
    QAction expandAllItemsAction_;

    //! The action for collapsing all items.
    QAction collapseAllItemsAction_;

    //! The action for automatically connecting logical signals to physical ports.
    QAction autoConnectAction_;
};

//-----------------------------------------------------------------------------

#endif // PORTMAPTREEVIEW_H
