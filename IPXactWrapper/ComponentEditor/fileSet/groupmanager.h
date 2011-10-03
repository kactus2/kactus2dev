/* 
 *
 *  Created on: 4.2.2011
 *      Author: Antti Kamppi
 * 		filename: groupmanager.h
 */

#ifndef GROUPMANAGER_H
#define GROUPMANAGER_H

#include <common/widgets/listManager/listmanager.h>

/*! \brief GroupManager is a widget to manage group names within FileSet.
 *
 * This widget inherits ListManager and redefines the onAdd and onEdit slots.
 */
class GroupManager : public ListManager {
	Q_OBJECT

public:

	
	/*! \brief The constructor.
	 *
	 * \param title The title for the QGroupBox.
	 * \param parent Pointer to the owner of this widget.
	 * \param items List of items to add to the widget.
	 *
	*/
	GroupManager(const QString title = tr("Group identifiers"), 
		QWidget *parent = 0,
		const QStringList& items = QStringList());

	//! \brief The destructor
	virtual ~GroupManager();

public slots:

	//! \brief Called when user clicks "Add" button.
	virtual void onAdd();

	//! \brief Called when user clicks "Edit" button.
	virtual void onEdit();

private:

	//! No copying
	GroupManager(const GroupManager& other);

	//! No assignment
	GroupManager& operator=(const GroupManager& other);
	
};

#endif // GROUPMANAGER_H
