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
	 *
	*/
	GroupManager(const QString title = tr("Group identifiers"), 
		QWidget *parent = 0);

	//! \brief The destructor
	virtual ~GroupManager();

	/*! \brief Initialize the group manager.
	 *
	 * This function must be called after creating the group manager and before
	 * using it.
	 * 
	 * \param items QStringList that contains the items to add to the widget.
	 *
	*/
	virtual void initialize(const QStringList& items = QStringList());

private:

	//! No copying
	GroupManager(const GroupManager& other);

	//! No assignment
	GroupManager& operator=(const GroupManager& other);
	
};

#endif // GROUPMANAGER_H
