//-----------------------------------------------------------------------------
// File: groupmanager.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 04.02.2011
//
// Description:
// GroupManager is a widget to manage group names within FileSet.
//-----------------------------------------------------------------------------

#ifndef GROUPMANAGER_H
#define GROUPMANAGER_H

#include <common/widgets/listManager/listmanager.h>

//-----------------------------------------------------------------------------
//! GroupManager is a widget to manage group names within FileSet.
//-----------------------------------------------------------------------------
class GroupManager : public ListManager
{
	Q_OBJECT

public:

	/*! The constructor.
	 *
	 *      @param [in] title   The title for the QGroupBox.
	 *      @param [in] parent  The owner of this widget.
	 */
	GroupManager(QString const& title = tr("Group identifiers"), QWidget* parent = 0);

	//! The destructor
	virtual ~GroupManager();

	/*! Initialize the group manager.
	 *
	 * This function must be called after creating the group manager and before using it.
	 * 
	 *      @param [in] items   The items to add to the widget.
	*/
	virtual void initialize(QStringList const& items = QStringList());

private:

	//! No copying
	GroupManager(const GroupManager& other);
	GroupManager& operator=(const GroupManager& other);
	
};

#endif // GROUPMANAGER_H
