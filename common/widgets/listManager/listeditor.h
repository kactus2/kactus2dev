//-----------------------------------------------------------------------------
// File: ListEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 30.05.2012
//
// Description:
// Editor for a list of items.
//-----------------------------------------------------------------------------

#ifndef LISTEDITOR_H
#define LISTEDITOR_H

#include <common/views/EditableListView/editablelistview.h>
#include <common/widgets/listManager/listmanagermodel.h>

#include <QWidget>
#include <QStyledItemDelegate>

//-----------------------------------------------------------------------------
//! Editor for a list of items.
//-----------------------------------------------------------------------------
class ListEditor : public QWidget
{
	Q_OBJECT

public:

	/*!
     *  The constructor.
	 *
	 *      @param [in] parent  Pointer to the owner of this editor.
	 */
	ListEditor(QWidget *parent);
	
	/*!
     *  The destructor.
     */
	virtual ~ListEditor();

	/*!
     *  Get the string items currently stored in the editor.
     *
     *      @return QStringList that contains the strings.
     */
	const QStringList& items() const;

	/*!
     *  Set the items for the editor.
     *
     *      @param [in] items   QStringList containing the strings to edit.
     */
	void setItems(const QStringList& items);

	/*!
     *  Get the number of items stored in the list.
	 *
	 *      @return Number of items in the list.
     */
	int size() const;

	/*!
     *  Set delegate for the view.
	 *
     *      @param [in] delegate    Pointer to the delegate that provides editor for view.
     */
	virtual void setItemDelegate(QStyledItemDelegate* delegate);

signals:

	/*!
     *  Emitted when contents of the editor change.
     */
	void contentChanged();

//     void closingEditor(QStringList const& listItems);
//     void closingEditor();

private:
	
	//! No copying. No assignment.
	ListEditor(const ListEditor& other);
	ListEditor& operator=(const ListEditor& other);

	//! The model that contains the list of strings.
	ListManagerModel model_;

	//! The View to display the list of strings.
	EditableListView view_;
};

#endif // LISTEDITOR_H
