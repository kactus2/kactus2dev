//-----------------------------------------------------------------------------
// File: PortMapsTreeView.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 7.10.2013
//
// Description:
// A view for port map tree model.
//-----------------------------------------------------------------------------

#ifndef PORTMAPSTREEVIEW_H
#define PORTMAPSTREEVIEW_H

#include <QTreeView>

#include <QModelIndex>

class PortMapsTreeModel;
class PortMapsTreeItem;


//-----------------------------------------------------------------------------
//! A tree view for the port map tree model.
//-----------------------------------------------------------------------------
class PortMapsTreeView : public QTreeView
{
    Q_OBJECT

public:
   
    /*!
     *  Constructor.
     *
     *      @param [in] parent The parent widget.
     */
    PortMapsTreeView(QWidget* parent);

    /*!
     *  Destructor.
     */
    ~PortMapsTreeView();

    /*!
     *  Sets the item model.
     *
     *      @param [in] model The model to set.
     */
    virtual void setModel(QAbstractItemModel* model);

signals:

   
    /*!
     *  Prints a warning message to the information display.
     */
    void warningMessage(const QString& msg) const;

    /*!
     *  Emitted when user attempts to remove an item.
     */
    void removeItem(QModelIndex const& index);

    /*!
     *  Emitted when user wants to restore an item.
     */
    void restoreItem(QModelIndex const& index);

protected:
    
    /*!
     *  Handles key presses.
     */
    virtual void keyPressEvent(QKeyEvent* event);

    /*!
     *  Handles auto-collapse when rows are inserted.
     *
     *      @param [in] parent The parent model index.
     *      @param [in] start  The start index of the inserted rows.
     *      @param [in] end    The end index of the inserted rows.
     */
    virtual void rowsInserted(QModelIndex const& parent, int start, int end);

    /*!
     *  Handles auto-collapse when model is reset.
     */
    virtual void reset();


    /*!
     *  Draws a row and a separator line under it.
     *
     *      @param [in] painter   The painter used for the drawing.
     *      @param [in] option    Options for the drawing.
     *      @param [in] index     Index of the item to draw.
     */
    virtual void drawRow(QPainter* painter, QStyleOptionViewItem const& option, QModelIndex const& index) const;

    /*!
     *  Handler for context menu events.
     *
     *      @param [in] event   The context menu event.
     */
    virtual void contextMenuEvent(QContextMenuEvent* event);

private slots:
   
    /*!
     *  Called when removeAction is clicked from the context menu.
     */
    virtual void onRemoveItem();

    /*!
     *  Called when restoreAction is clicked from the context menu.
     */
    virtual void onRestoreItem();

private:
   
    // Disable copying.
    PortMapsTreeView(PortMapsTreeView const& rhs);
    PortMapsTreeView& operator=(PortMapsTreeView const& rhs);
       

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The data model.
    PortMapsTreeModel* model_;

    //! The file dependency item context menu was opened on
    QModelIndex contextMenuIndex_;

    //! Action for removing mappings.
    QAction* removeAction_;

    //! Action for restoring logical port to list.
    QAction* restoreAction_;
};

//-----------------------------------------------------------------------------

#endif // PORTMAPSTREEVIEW_H
