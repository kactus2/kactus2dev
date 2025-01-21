//-----------------------------------------------------------------------------
// File: MemoryMapsView.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 17.04.2015
//
// Description:
// View for memory maps editor.
//-----------------------------------------------------------------------------

#ifndef MEMORYMAPSVIEW_H
#define MEMORYMAPSVIEW_H

#include <common/views/EditableTreeView/EditableTreeView.h>

//-----------------------------------------------------------------------------
//! View for memory maps editor.
//-----------------------------------------------------------------------------
class MemoryMapsView : public EditableTreeView
{
	Q_OBJECT

public:

    /*!
	 *  The constructor.
	 *
	 *    @param [in] parent  Pointer to the owner of this view.
	 */
	MemoryMapsView(QWidget* parent);

    //! No copying. No assignment.
    MemoryMapsView(const MemoryMapsView& other) = delete;
    MemoryMapsView& operator=(const MemoryMapsView& other) = delete;

    /*!
     *  The destructor.
     */
    virtual ~MemoryMapsView() = default;

    /*!
	 *  Enable/disable the import/export csv-file functionality.
	 *
	 *    @param [in] allow   If true, then import/export is enabled.
	 */
	void setAllowImportExport(bool allow);

public slots:

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
	 *    @param [in] path    The path to tuse ad default.
	 */
	void setDefaultImportExportPath(const QString& path);

signals:

    /*!
     *  Copy the selected rows.
     *
     *    @param [in] indexList   List of selected indexes containing the copied rows.
     */
    void copyRows(QModelIndexList indexList);

    /*!
     *  Paste the copied rows.
     *
     *    @param [in] targetIndex     The target of the paste action.
     */
    void pasteRows(QModelIndex targetIndex);

protected:

    /*!
	 *  Handles keypress events for copy, paste, cut, delete and line separator insertion.
	 *
	 *    @param [in] event   The key event.
	 */
	virtual void keyPressEvent(QKeyEvent* event) override final;

	/*!
	 *  Creates a context menu for adding/removing/editing items in the editor.
	 *
	 *    @param [in] event   The context menu event.
	 */
	virtual void contextMenuEvent(QContextMenuEvent* event) override final;

private slots:

	/*!
	 *  Copies the text contained in the item.
	 */
	void onCopyAction();

	/*!
	 *  Pastes the text from clipboard to the item.
	 */
	void onPasteAction();

    /*!
     *  Handler for element copy action.
     */
    void onCopyRowsAction();

    /*!
     *  Handler for element paste action.
     */
    void onPasteRowsAction();

private:

	/*!
	 *  Set up the actions for the context menu.
	 */
	void setupActions();

    /*!
    *  Generates a unique name within the table for the item.
    *
    *    @param [in] original     The original name of the item.
    *    @param [in] model        The model of this view.
    *
    *    @return Unique name for the item.
    */
    QString getUniqueName(QString const& original, QAbstractItemModel* model);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

	//! The default path to use for import/export csv
	QString defaultImportExportPath_;

    //! The point where mouse was clicked
    QPoint pressedPoint_;

    //! Action for copying item texts.
    QAction copyAction_;

    //! Action for pasting item texts.
    QAction pasteAction_;

    //! Action for importing a csv-file.
    QAction importAction_;

    //! Action for exporting a csv-file.
    QAction exportAction_;

    //! The action to copy an element.
    QAction copyRowsAction_;

    //! The action to paste an element.
    QAction pasteRowsAction_;

    //! Specifies if the items can be imported/exported to a csv file..
    bool importExportable_;
};

#endif // MEMORYMAPSVIEW_H
