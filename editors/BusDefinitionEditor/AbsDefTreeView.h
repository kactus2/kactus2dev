//-----------------------------------------------------------------------------
// File: AbsDefTreeView.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Janne Virtanen
// Date: 27.04.2017
//
// Description:
// A view for abstraction definitions.
//-----------------------------------------------------------------------------

#ifndef ABSDEFTREEVIEW_H
#define ABSDEFTREEVIEW_H

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
class AbsDefTreeView : public ConfigurableElementsView
{
    Q_OBJECT

public:
   
    /*!
     *  The constructor.
     *
     *      @param [in] parent  The parent widget.
     */
    AbsDefTreeView(QWidget* parent);

    /*!
     *  The destructor.
     */
    ~AbsDefTreeView();

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

signals:

    /*!
     *  Add a new port map at the selected position.
     *
     *      @param [in] position    The parent index of the new port map.
     */
    void addItem(QModelIndex const& position);

    //! Emitted when missing modes should be added to the selected signal.
    void addSignalOptions();

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
     *  Handler for adding a port map.
     */
    void onAdd();

    /*!
     *  Handler for removing.
     */
    void onRemove();

private:
   
    // Disable copying.
    AbsDefTreeView(AbsDefTreeView const& rhs);
    AbsDefTreeView& operator=(AbsDefTreeView const& rhs);

    /*!
     *  Setup the available context menu actions.
     */
    void setupActions();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The action for adding a port map.
    QAction addAction_;

    //! The action for expanding all items.
    QAction expandAllItemsAction_;

    //! The action for collapsing all items.
    QAction collapseAllItemsAction_;

    //! Action for adding missing modes for a signal.
    QAction addOptionsAction_;
};

//-----------------------------------------------------------------------------

#endif // ABSDEFTREEVIEW_H
