/* 
 *  	Created on: 9.12.2011
 *      Author: Antti Kamppi
 * 		filename: groupmanagerdelegate.h
 *		Project: Kactus 2
 */

#ifndef GROUPMANAGERDELEGATE_H
#define GROUPMANAGERDELEGATE_H

#include <common/delegates/ComboDelegate/combodelegate.h>

/*! \brief Provides editor to select a group for a file set.
 *
 */
class GroupManagerDelegate : public ComboDelegate {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param parent Pointer to the owner of the delegate.
	 *
	*/
	GroupManagerDelegate(QObject *parent);
	
	//! \brief The destructor
	virtual ~GroupManagerDelegate();

	/*! \brief Create a new editor for the given item
	 *
	 * \param parent Owner for the editor.
	 * \param option Contains options for the editor.
	 * \param index Model index identifying the item.
	 *
	 * \return Pointer to the editor to be used to edit the item.
	*/
	virtual QWidget* createEditor(QWidget* parent, 
		const QStyleOptionViewItem& option, 
		const QModelIndex& index) const;

private:
	//! \brief No copying
	GroupManagerDelegate(const GroupManagerDelegate& other);

	//! \brief No assignment
	GroupManagerDelegate& operator=(const GroupManagerDelegate& other);
};

#endif // GROUPMANAGERDELEGATE_H
