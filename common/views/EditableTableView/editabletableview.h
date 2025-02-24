//-----------------------------------------------------------------------------
// File: editabletableview.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 10.12.2011
//
// Description:
// This view can be used to display contents of a table model and supports adding and removing items.
//-----------------------------------------------------------------------------

#ifndef EDITABLETABLEVIEW_H
#define EDITABLETABLEVIEW_H

#include <QTableView>
#include <QMouseEvent>
#include <QPoint>
#include <QAction>
#include <QKeyEvent>
#include <QContextMenuEvent>
#include <QDragEnterEvent>
#include <QDropEvent>

//-----------------------------------------------------------------------------
//! This view can be used to display contents of a table model and supports adding and removing items.
//-----------------------------------------------------------------------------
class EditableTableView : public QTableView
{
	Q_OBJECT

public:

	//! The minimum height for the view.
	static const int MINIMUM_TABLE_HEIGHT = 100;

    //! Number of the column for name.
    static const int NAME_COLUMN = 0; 

	/*!
     *  The constructor.
	 *
     *    @param [in] parent  Pointer to the owner of this view.
	 */
	EditableTableView(QWidget *parent);
	
	/*!
     *  The destructor.
     */
	virtual ~EditableTableView() = default;

	/*!
     *  Enable/disable the import/export csv file functionality.
	 *
	 *    @param [in] allow   If true then import/export is enabled.
	 */
	virtual void setAllowImportExport(bool allow);

    /*!
     *  Checks if import and export are allowed in the table.
     *
     *    @return True, if import and export are allowed, otherwise false.
     */
    bool importExportAllowed() const;

    /*!
     *  Enable / disable the element copying.
     *
     *    @param [in] allow   If true, element copying is allowed.
     */
    void setAllowElementCopying(bool allow);

	/*!
     *  Set the model class for this view.	 
	 *
	 *    @param [in] model The model displayed by this view.
	 */
	virtual void setModel(QAbstractItemModel* model);

    /*!
     *  Sets the text visible in the corner button. Both headers must be visible for the text to show.
     *
     *    @param [in] text   The text to set.
     */
    void setCornerButtonText(QString const& text);

    /*!
    *  Sets if the column sizes are fit to content at model load.
    *
    *    @param [in] fit   To fit or not.
    */
    void setFitColumnsToContent(bool fit);

public slots:

	/*!
     *  Set the items in the view to be draggable or not.
	 *
	 *    @param [in] draggable   If true the positions of the items can change by dragging.
     */
	void setItemsDraggable(bool draggable);

	/*!
     *  Create a csv-file of the editor contents.
	 *
	 *    @param [in] filePath    Contains an absolute file path to the file to create.
	 */
	void onCSVExport(const QString& filePath = QString());

	/*!
     *  Import contents of a csv-file to the editor.
	 *
	 *    @param [in] filePath    The path to the csv-file to import.
	 */
	void onCSVImport(const QString& filePath = QString());

	/*!
     *  Set the default path to use for import/export csv.
	 *
	 *    @param [in]  path   The path to use as default.
	 */
	virtual void setDefaultImportExportPath(const QString& path);

signals:

	/*!
     *  A new item should be added to given index.
	 *
	 *    @param [in] index   The position where new item should be added at.
	 */
	void addItem(const QModelIndex& index);

	/*!
     *  An item should be removed from the model.
	 * 
	 *  @param [in] index   Identifies the item that should be removed.
	 */
	void removeItem(const QModelIndex& index);

	/*!
     *  Move item to another position.
	 *
	 *    @param [in] originalPos     Identifies the item that should be moved.
	 *    @param [in] newPos          The new position the item should be moved to.
	 */
	void moveItem(const QModelIndex& originalPos, const QModelIndex& newPos);

    /*!
     *  Copy the selected rows.
     *
     *    @param [in] indexList   List of selected indexes containing the copied rows.
     */
    void copyRows(QModelIndexList indexList);

    /*!
     *  Paste the copied rows.
     */
    void pasteRows();

protected:

	//! Handler for mouse move events
	virtual void mouseMoveEvent(QMouseEvent* e);

	//! Handler for mouse double click events
	virtual void mouseDoubleClickEvent(QMouseEvent* event);

	//! Handler for mouse press events
	virtual void mousePressEvent(QMouseEvent* event);

	//! Handler for mouse release events
	virtual void mouseReleaseEvent(QMouseEvent* event);

	//! Handler for context menu events
	virtual void contextMenuEvent(QContextMenuEvent* event);
    
    /*!
     *  Adds basic actions for the context menu (add, remove, clear, cut, copy, paste).
     *
     *    @param [in] menu    Menu containing the actions.
     *    @param [in] index   Index of the item at the context menu event position.
     */
    void addBasicActionsForContextMenu(QMenu& menu, QModelIndex const& index);

    /*!
     *  Add actions for copying and pasting elements to the context menu.
     *
     *    @param [in] menu    Menu containing the actions.
     *    @param [in] index   Index of the item at the context menu event position.
     */
    void addElementCopyActionForContextMenu(QMenu& menu, QModelIndex const& index);

    /*!
     *  Add actions for importing and exporting to the context menu.
     *
     *    @param [in] menu    Menu containing the actions.
     */
    void addImportExportActionsForContextMenu(QMenu& menu);

    /*!
     *  Counts the number of rows in a list of indexes.
     *
     *    @param [in] indexes   The indexes to count.
     *
     *    @return The number of rows in the indexes.
     */
    int countRows(QModelIndexList const& indexes);

    /*!
     *  Handler for intercepting events.
     *
     *    @param [in] target   The target for the event.
     *    @param [in] event    The event to intercept or pass to target.
     *
     *    @return True, if the event is filtered and not passed to target, otherwise false.
     */
    virtual bool eventFilter(QObject* target, QEvent* event);

	//! The point where mouse was clicked
	QPoint pressedPoint_;

	//! Action to add new item
	QAction addAction_;

	//! Action to remove item
	QAction removeAction_;

    //! Action to cut item
    QAction cutAction_;

	//! Action to copy item
	QAction copyAction_;

	//! Action to paste item
	QAction pasteAction_;

	//! Action to clear selected cells
	QAction clearAction_;

	//! Action to import a csv-file
	QAction importAction_;

	//! Action to export a csv-file
	QAction exportAction_;

    //! The action to copy an element.
    QAction copyElementAction_;

    //! The action to paste an element.
    QAction pasteElementAction_;

	//! Specified if the items can be imported/exported to a csv file.
	bool importExportEnabled_;

    //! Specifies if the elements can be copied / pasted.
    bool elementCopyIsAllowed_;

    /*!
     *  Set up the actions for the context menu
     */
    virtual void setupActions();

protected slots:

	//! Handler for add action
	virtual void onAddAction();

	//! Handler for remove action
	virtual void onRemoveAction();

	//! Handler for copy action.
	virtual void onCopyAction();

	//! Handler for paste action
	virtual void onPasteAction();

	//! Handler for cell clear action
	virtual void onClearAction(); 

    //! Handler for cut action.
    virtual void onCutAction();

    /*!
     *  Handler for element copy action.
     */
    virtual void onCopyElementAction();

private:

	//! No copying
	EditableTableView(const EditableTableView& other);
	EditableTableView& operator=(const EditableTableView& other);



    /*!
     *  Generates a unique name within the table for the item.
     *
     *    @param [in] original     The original name of the item.
     *
     *    @return Unique name for the item.
     */
    QString getUniqueName(QString const& original);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

	//! The default path to use for import/export csv
	QString defImportExportPath_;

	//! Specifies if the items in the view can be dragged from position to another.
	bool itemsDraggable_;

    //! Specifies if the columns sizes are fit to content on model load.
    bool fitColumnsToContent_;
};

#endif // EDITABLETABLEVIEW_H
