//-----------------------------------------------------------------------------
// File: EditableTreeView.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 30.11.2017
//
// Description:
// A view for editable trees.
//-----------------------------------------------------------------------------

#ifndef EDITABLETREEVIEW_H
#define EDITABLETREEVIEW_H

#include <QTreeView>
#include <QContextMenuEvent>
#include <QModelIndex>
#include <QAction>
#include <QModelIndexList>

//-----------------------------------------------------------------------------
//! A tree view for the configurable element tree model.
//-----------------------------------------------------------------------------
class EditableTreeView : public QTreeView
{
    Q_OBJECT

public:
   
    /*!
     *  The constructor.
     *
     *      @param [in] allowNewRows        Flag for allowing new items to be added.
     *      @param [in] addItemText         Text for adding new items.
     *      @param [in] addSubItemText      Text for adding new sub items.
     *      @param [in] removeItemText      Text for removing items.
     *      @param [in] removeSubItemsText  Text for removing all sub items.
     *      @param [in] parent              The owner of this view.
     */
    EditableTreeView(bool allowNewRows, QString const& addItemText, QString const& addSubItemText,
        QString const& removeItemText, QString const& removeSubItemsText, QWidget* parent);

    /*!
     *  The destructor.
     */
    virtual ~EditableTreeView();

signals:

    /*!
     *  Add items to the selected indexes.
     *
     *      @param [in] indexes     The selected indexes.
     */
    void addItems(QModelIndexList const& indexes);

    /*!
     *  Add sub items to the selected indexes.
     *
     *      @param [in] indexes     The selected indexes.
     */
    void addSubItem(QModelIndexList const& indexes);

    /*!
     *  Emitted when the user attempts to remove an item.
     *
     *      @param [in] index   The indexes of the item to be removed.
     */
    void removeSelectedItems(QModelIndexList const& indexes);

    /*!
     *  Emitted when the user attempts to remove all sub items from the selected items.
     *
     *      @param [in] index   The indexes of the items whose sub items are removed.
     */
    void removeAllSubItems(QModelIndexList const& indexes);

protected:
    
    /*!
	 *  Handle the double click.
	 *
	 *      @param [in] event   The mouse event.
	 */
	virtual void mouseDoubleClickEvent(QMouseEvent* event) override;

    /*!
     *  Draws a row and.
     *
     *      @param [in] painter   The painter used for the drawing.
     *      @param [in] option    Options for the drawing.
     *      @param [in] index     Index of the item to draw.
     */
    virtual void drawRow(QPainter* painter, QStyleOptionViewItem const& option, QModelIndex const& index)
        const override;

    /*!
     *  Handler for context menu events.
     *
     *      @param [in] event   The context menu event.
     */
    virtual void contextMenuEvent(QContextMenuEvent* event) override;

    /*!
     *  Gets the action for adding items.
     *
     *      @return The add item action.
     */
    QAction* getAddItemAction() const;

    /*!
     *  Gets the action for adding sub items.
     *
     *      @return The add sub item action.
     */
    QAction* getAddSubItemAction() const;

    /*!
     *  Gets the action for the remove command.
     *
     *      @return The action for the remove command.
     */
    QAction* getRemoveAction() const;

    /*!
     *  Gets the action for removing all sub items.
     *
     *      @return The remove all sub items action.
     */
    QAction* getRemoveAllSubItemsAction() const;

    /*!
     *  Gets the action for clearing an item.
     *
     *      @return The clear item action.
     */
    QAction* getClearAction() const;

    /*!
     *  Gets the action for displaying all child items of an item.
     *
     *      @return The expand all item action.
     */
    QAction* getExpandAllAction() const;

    /*!
     *  Gets the action for hiding all child items of an item.
     *
     *      @return The collapse all item action.
     */
    QAction* getCollapseAllAction() const;

protected slots:

    /*!
     *  Handles the clearing of the selected cell.
     */
    void onClearAction();

    /*!
     *  Handles the item addition.
     */
    void onAddItem();

    /*!
     *  Handles the sub item addition.
     */
    void onAddSubItem();

    /*!
     *  Handles the removal of the items.
     */
    void onRemoveSelectedItems();

    /*!
     *  Handles the removal of all sub items of the selected index.
     */
    void onRemoveAllSubItems();

private:
   
    // No copying. No assignment.
    EditableTreeView(EditableTreeView const& rhs);
    EditableTreeView& operator=(EditableTreeView const& rhs);

    /*!
     *  Setup the actions.
     *
     *      @param [in] addItemText         Text for adding new items.
     *      @param [in] addSubItemText      Text for adding new sub items.
     *      @param [in] removeItemText      Text for removing items.
     *      @param [in] removeSubItemsText  Text for removing all sub items.
     */
    void setupActions(QString const& addItemText, QString const& addSubItemText, QString const& removeItemText,
        QString const& removeSubItemsText);

    /*!
     *  Connect the actions to their corresponding slots.
     */
    void connectActions();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Flag for allowing new items to be added.
    bool allowNewRows_;

    //! Action for adding new items.
    QAction* addItemAction_;

    //! Action for adding new sub items.
    QAction* addSubItemAction_;

    //! Action for removing items.
    QAction* removeAction_;

    //! Action for removing all sub items.
    QAction* removeAllSubItemsAction_;

    //! Action for clearing a cell.
    QAction* clearAction_;

    //! Action for displaying all child items.
    QAction* expandAllItemsAction_;

    //! Action for hiding all child items.
    QAction* collapseAllItemsAction_;
};

//-----------------------------------------------------------------------------

#endif // CONFIGURABLEELEMENTSVIEW_H
