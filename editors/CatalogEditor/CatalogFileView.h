//-----------------------------------------------------------------------------
// File: CatalogFileView.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 03.02.2017
//
// Description:
// The widget to display the ipxact files of a catalog.
//-----------------------------------------------------------------------------

#ifndef CATALOGFILEVIEW_H
#define CATALOGFILEVIEW_H


#include <QTreeView>
#include <QContextMenuEvent>
#include <QAction>
#include <QPoint>
#include <QModelIndex>
#include <QMouseEvent>

class LibraryInterface;

//-----------------------------------------------------------------------------
//! The widget to display the library in a tree.
//-----------------------------------------------------------------------------
class CatalogFileView : public QTreeView
{
	Q_OBJECT

public:

	/*! The constructor
	* 
	*      @param [in] handler  The instance that manages the library.
	*      @param [in] filter   The class that does the filtering for the view.
	*      @param [in] parent   The owner of this widget.
	*
	*/
	CatalogFileView(QWidget* parent = 0);

	//! The destructor.
	virtual ~CatalogFileView();

signals:

    //! Emitted when an item should be added in the given index.
    void addItem(QModelIndex const& index);

    //! Emitted when an item should be removed from the given index.
    void removeItem(QModelIndex const& index);

    //! Emitted when an item should be opened in the given index.
    void openItem(QModelIndex const& index);

protected:
   
    /*!
     *  Draws a row and a separator line under it.
     *
     *      @param [in] painter   The painter used for the drawing.
     *      @param [in] option    Options for the drawing.
     *      @param [in] index     Index of the item to draw.
     */
    virtual void drawRow(QPainter* painter, QStyleOptionViewItem const& option, QModelIndex const& index) const;

    /*! Handler for the context menu events.
    *
    *      @param [in] event The contextMenuEvent to be handled.
    */
    virtual void contextMenuEvent(QContextMenuEvent* event);

private slots:

    //! Handler for add row action.
    void onAddAction();

    //! Handler for remove row action.
    void onRemoveAction();

    //! Handler for copy action.
    void onCopyAction();

    //! Handler for paste action.
    void onPasteAction();

    //! Handler for cut action.
    void onCutAction();

    //! Handler for clear action.
    void onClearAction();

    //! Handler for open action.
    void onOpenAction();

private:

    //! No assignment
	CatalogFileView(const CatalogFileView& other);

	//! No copying
	CatalogFileView& operator=(const CatalogFileView& other);

	//! Set up the actions for the context menu
	void setupActions();

    /*!
     *  Sorts the given indexes and removes all but one index for each row.
     *
     *      @param [in] indexes   the index to sort and minimize.
     *
     *      @return Indexes in sorted order with only one index per row.
     */
    QModelIndexList sortAndMinimize(QModelIndexList const& indexes);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Action to add new item.
    QAction* addAction_;

    //! Action to remove item.
    QAction* removeAction_;

    //! Action to open an item.
    QAction* openAction_;

    //! Action to cut item.
    QAction* cutAction_;

    //! Action to copy item.
    QAction* copyAction_;

    //! Action to paste item.
    QAction* pasteAction_;

    //! Action to clear selected cells.
    QAction* clearAction_;

    //! Action to expand all branches.
    QAction* expandAllIAction_;

    //! Action to collapse all branches.
    QAction* collapseAllIAction_;
};

#endif // CATALOGFILEVIEW_H
