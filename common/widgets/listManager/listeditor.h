/* 
 *  	Created on: 30.5.2012
 *      Author: Antti Kamppi
 * 		filename: listeditor.h
 *		Project: Kactus 2
 */

#ifndef LISTEDITOR_H
#define LISTEDITOR_H

#include <common/views/EditableListView/editablelistview.h>
#include <common/widgets/listManager/listmanagermodel.h>

#include <QWidget>
#include <QStyledItemDelegate>

/*! \brief The editor that can be used to edit a list of QStrings.
 *
 */
class ListEditor : public QWidget {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param parent Pointer to the owner of this editor.
	 *
	*/
	ListEditor(QWidget *parent);
	
	//! \brief The destructor
	virtual ~ListEditor();

	/*! \brief Get the string items currently stored in the editor.
	*
	* \return QStringList that contains the strings.
	*/
	const QStringList& items() const;

	/*! \brief Set the items for the editor.
	*
	* \param items QStringList containing the strings to edit.
	*/
	void setItems(const QStringList& items);

	/*! \brief Get the number of items stored in the list.
	 *
	 * \return Number of items in the list.
	*/
	int size() const;

	/*! \brief Set delegate for the view.
	 *
	 * \param delegate Pointer to the delegate that provides editor for view.
	 *
	*/
	virtual void setItemDelegate(QStyledItemDelegate* delegate);

signals:

	//! \brief Emitted when contents of the editor change.
	void contentChanged();

private:
	
	//! \brief No copying
	ListEditor(const ListEditor& other);

	//! \brief No assignment
	ListEditor& operator=(const ListEditor& other);

	//! \brief The model that contains the list of strings.
	ListManagerModel model_;

	//! \brief The View to display the list of strings.
	EditableListView view_;
};

#endif // LISTEDITOR_H
