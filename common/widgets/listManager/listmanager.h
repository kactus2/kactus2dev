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


//-----------------------------------------------------------------------------
//! ListManager is a widget to manage a list of strings.
//-----------------------------------------------------------------------------
class ListManager : public QGroupBox 
{
	Q_OBJECT

public:

	/*! The constructor.
	 *
	 *      @param [in] title   The title to be set for the QGroupBox
	 *      @param [in] parent  The owner of this widget.
	 */
	ListManager(QString const& title = tr("List"), QWidget *parent = 0);

	//! the destructor
	virtual ~ListManager();

	/*! Initialize the list manager.
	 *
	 * This function must be called after creating the list manager and before using it.
	 * 
	 *      @param [in] items QStringList that contains the items to add to the widget.
	 */
	virtual void initialize(const QStringList& items = QStringList());

	/*! Get the string items currently stored in the model.
	 *
	 *       @return QStringList that is contained in this widget.
	 */
	QStringList items() const;

	/*! Set the items to be stored in this widget.
	 *
	 *      @param [in] items QStringList containing the strings to add.
	 */
	void setItems(const QStringList& items);

	/*! Get the number of items stored in the list.
	 *
	 *      @return Number of items in the list.
	 */
	int size() const;

signals:
	//! Emitted when contents of the ListManager change.
	void contentChanged();

protected:

	//! The model that contains the list of strings.
	ListManagerModel* model_;

	//! The View to display the list of strings.
	EditableListView* view_;

private:

	//! No copying
	ListManager(const ListManager& other);

	//! No assignment
	ListManager& operator=(const ListManager& other);

};

#endif // LISTMANAGER_H
