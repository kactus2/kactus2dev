//-----------------------------------------------------------------------------
// File: editablelistview.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 12.12.2011
//
// Description:
// View-class with add, remove and move operations for lists of items.
//-----------------------------------------------------------------------------

#ifndef EDITABLELISTVIEW_H
#define EDITABLELISTVIEW_H

#include <QListView>
#include <QPoint>
#include <QMouseEvent>
#include <QContextMenuEvent>
#include <QAction>

//-----------------------------------------------------------------------------
//! View-class with add, remove and move operations for lists of items.
//-----------------------------------------------------------------------------
class EditableListView : public QListView
{
	Q_OBJECT

public:

	/*!
     *  The constructor.
	 *
	 *    @param [in] parent  The owner of this view.
	 */
	EditableListView(QWidget *parent);
	
	/*!
     *  The destructor.
     */
	virtual ~EditableListView();

signals:

    /*!
     *  Emitted when the selection has changed.
     */
    void selectionChanged(QModelIndex const& index);

	/*!
     *  A new item should be added to given index.
	 *
	 *    @param [in] index The position where new item should be added at.
     */
	void addItem(QModelIndex const& index);

	/*!
     *  An item should be removed from the model.
	 * 
	 *    @param [in] index Identifies the item that should be removed.
     */
	void removeItem(QModelIndex const& index);

	/*!
     *  Move item to another position.
	 *
	 *    @param [in] originalPos Identifies the item that should be moved.
	 *    @param [in] newPos The new position the item should be moved to.
     */
	void moveItem(QModelIndex const& originalPos, QModelIndex const& newPos);

protected:

    /*!
     *  Handler for paint events.
     *
     *    @param [in] event   The paint event.
     */
    virtual void paintEvent(QPaintEvent *event);

	/*!
     *  Handler for mouse move events
     *
     *    @param [in] event   The mouse movement event.
     */
	virtual void mouseMoveEvent(QMouseEvent* event);

	/*!
     *  Handler for mouse double click events
     *
     *    @param [in] event   The double click event.
     */
	virtual void mouseDoubleClickEvent(QMouseEvent* event);

	/*!
     *  Handler for mouse press events
     *
     *    @param [in] event   The mouse press event.
     */
	virtual void mousePressEvent(QMouseEvent* event);

	/*!
     *  Handler for mouse release events
     *
     *    @param [in] event   The mouse release event.
     */
	virtual void mouseReleaseEvent(QMouseEvent* event);

	/*!
     * Handler for context menu events
     *
     *    @param [in] event   The context menu event.
     */
	virtual void contextMenuEvent(QContextMenuEvent* event);

    /*!
     *  Handler for key press events.
     *
     *    @param [in] event   The key press event.
     */
    virtual void keyPressEvent(QKeyEvent *event);
    
    /*!
     *  Handler for changes in current selection.
     *
     *    @param [in] current     The current model index.
     *    @param [in] previous    The previous model index.
     */
    virtual void currentChanged(const QModelIndex &current, const QModelIndex &previous);

protected slots:

    /*!
     *  Called when new rows have been inserted.
     */
    virtual void rowsInserted(QModelIndex const& parent, int start, int end);

private slots:

	/*!
     *  Handler for add action.
     */
	void onAddAction();

	/*!
     *  Handler for remove action.
     */
	void onRemoveAction();

private:

	//! No copying. No assignment.
	EditableListView(const EditableListView& other);
	EditableListView& operator=(const EditableListView& other);

	/*!
     *  Set up the actions for the context menu
     */
	void setupActions();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

	//! The point where mouse was clicked
	QPoint pressedPoint_;

	//! Action to add new item
	QAction addAction_;

	//! Action to remove item
	QAction removeAction_;
};

#endif // EDITABLELISTVIEW_H
