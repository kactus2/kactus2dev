/* 
 *
 *  Created on: 28.1.2011
 *      Author: Antti Kamppi
 * 		filename: listmanager.h
 */

#ifndef LISTMANAGER_H
#define LISTMANAGER_H

#include "listmanagermodel.h"

#include <common/views/EditableListView/editablelistview.h>

#include <QWidget>
#include <QGroupBox>
#include <QPushButton>
#include <QObject>
#include <QStringList>

/*! \brief ListManager is a widget to manage a list of strings.
 *
 * This class can be used as a general widget to manage a string list with a
 * GUI.
 */
class ListManager : public QGroupBox {
	
	Q_OBJECT

public:

	/*! \brief The constructor.
	 *
	 * \param title The title to be set for the QGroupBox
	 * \param parent Pointer to the owner of this widget.
	 * \param items QStringList that contains the items to add to the widget.
	 */
	ListManager(const QString title = tr("List"), QWidget *parent = 0,
		const QStringList& items = QStringList());

	//! \brief the destructor
	virtual ~ListManager();

	/*! \brief Get the string items currently stored in the model.
	*
	* \return QStringList that is contained in this widget.
	*/
	const QStringList& items() const;

	/*! \brief Set the items to be stored in this widget.
	*
	* \param items QStringList containing the strings to add.
	*/
	void setItems(const QStringList& items);

	/*! \brief Get the number of items stored in the list.
	 *
	 * \return Number of items in the list.
	*/
	int size() const;

signals:

	//! \brief Emitted when contents of the ListManager change.
	void contentChanged();

protected:

	//! \brief The model that contains the list of strings.
	ListManagerModel model_;

	//! \brief The View to display the list of strings.
	EditableListView view_;

private:

	//! No copying
	ListManager(const ListManager& other);

	//! No assignment
	ListManager& operator=(const ListManager& other);

};

#endif // LISTMANAGER_H
