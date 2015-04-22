//-----------------------------------------------------------------------------
// File: MemoryMapsView.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 17.04.2015
//
// Description:
// View for memory maps editor.
//-----------------------------------------------------------------------------

#ifndef MEMORYMAPSVIEW_H
#define MEMORYMAPSVIEW_H

#include <common/views/EditableTableView/editabletableview.h>

#include <designEditors/common/ComponentInstanceEditor/ConfigurableElementsView.h>

//-----------------------------------------------------------------------------
//! View for memory maps editor.
//-----------------------------------------------------------------------------
class MemoryMapsView : public ConfigurableElementsView
{
	Q_OBJECT

public:

    /*!
	 *  The constructor.
	 *
	 *      @param [in] parent  Pointer to the owner of this view.
	 */
	MemoryMapsView(QWidget* parent);

    /*!
     *  The destructor.
     */
    virtual ~MemoryMapsView();

    /*!
	 *  Enable/disable the import/export csv-file functionality.
	 *
	 *      @param [in] allow   If true, then import/export is enabled.
	 */
	virtual void setAllowImportExport(bool allow);

public slots:

	/*!
	 *  Create a casv-file of the editor contents.
	 *
	 *      @param [in] filePath    Contains an absolute file path to the file to create.
	 */
	void onCSVExport(const QString& filePath = QString());

    /*!
	 *  Import contents of a csv-file to the editor.
	 *
	 *      @param [in] filePath    The path to the csv-file to import.
	 */
	void onCSVImport(const QString& filePath = QString());

    /*!
	 *  Set the default path to use for import/export csv.
	 *
	 *      @param [in] path    The path to tuse ad default.
	 */
	virtual void setDefaultImportExportPath(const QString& path);

signals:

    /*!
     *  Add a new memory map.
     *
     *      @param [in] index   The index of the new memory map.
     */
    void addItem(const QModelIndex& index);

    /*!
     *  Add a new memory remap.
     *
     *      @param [in] index   The index of the new memory remap.
     */
    void addMemoryRemapItem(const QModelIndex& index);

protected:

    /*!
	 *  Handles keypress events for copy, paste, cut, delete and line separator insertion.
	 *
	 *      @param [in] event   The key event.
	 */
	virtual void keyPressEvent(QKeyEvent* event);

	/*!
	 *  Creates a new memory map to the model.
	 *
	 *      @param [in] event   The mouse event.
	 */
	virtual void mouseDoubleClickEvent(QMouseEvent* event);

	/*!
	 *  Creates a context menu for adding/removing/editing items in the editor.
	 *
	 *      @param [in] event   The context menu event.
	 */
	virtual void contextMenuEvent(QContextMenuEvent* event);

	//! The point where mouse was clicked
	QPoint pressedPoint_;

    //! Action for adding memory remaps.
    QAction addMemoryRemapAction_;

	//! Action for adding new items.
	QAction addAction_;

	//! Action for copying item texts.
	QAction copyAction_;

	//! Action for pasting item texts.
	QAction pasteAction_;

	//! Action for clearing selected cells.
	QAction clearAction_;

	//! Action for importing a csv-file.
	QAction importAction_;

	//! Action for exporting a csv-file.
	QAction exportAction_;

	//! Specifies if the items can be imported/exported to a csv file..
	bool importExportable_;

private slots:

	/*!
	 *  Adds a new memory map to the model.
	 */
	virtual void onAddAction();

	/*!
	 *  Removes an item from the model.
	 */
	virtual void onRemoveAction();

	/*!
	 *  Copies the text contained in the item.
	 */
	virtual void onCopyAction();

	/*!
	 *  Pastes the text from clipboard to the item.
	 */
	virtual void onPasteAction();

	/*!
	 *  Clears the contents of the item.
	 */
	virtual void onClearAction(); 

    /*!
     *  Adds a new memory remap under a memory map..
     */
    virtual void onAddMemoryRemapAction();

private:

	//! No copying.
    MemoryMapsView(const MemoryMapsView& other);

	//! No assignment.
    MemoryMapsView& operator=(const MemoryMapsView& other);

	/*!
	 *  Set upt the actions for the context menu.
	 */
	void setupActions();

    /*!
    *  Generates a unique name within the table for the item.
    *
    *      @param [in] original     The original name of the item.
    *      @param [in] model        The model of this view.
    *
    *      @return Unique name for the item.
    */
    QString getUniqueName(QString const& original, QAbstractItemModel* model);

	//! The default path to use for import/export csv
	QString defaultImportExportPath_;
};

#endif // MEMORYMAPSVIEW_H
