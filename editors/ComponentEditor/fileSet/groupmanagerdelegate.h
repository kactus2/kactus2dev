/* 
 *  	Created on: 9.12.2011
 *      Author: Antti Kamppi
 * 		filename: groupmanagerdelegate.h
 *		Project: Kactus 2
 */

#ifndef GROUPMANAGERDELEGATE_H
#define GROUPMANAGERDELEGATE_H

#include <common/delegates/ComboDelegate/combodelegate.h>

class BasicLineContentMatcher;

/*! Provides editor to select a group for a file set.
 *
 */
class GroupManagerDelegate : public QStyledItemDelegate {
	Q_OBJECT

public:

	/*! The constructor
	 *
	 *      @param [in] parent The owner of the delegate.
	*/
	GroupManagerDelegate(QObject *parent);
	
	//! The destructor
	virtual ~GroupManagerDelegate();

	/*! Create a new editor for the given item
	 *
	 *      @param [in] parent  Owner for the editor.
	 *      @param [in] option  Contains options for the editor.
	 *      @param [in] index   Model index identifying the item.
	 *
	 *      @return The editor to be used to edit the item.
	*/
	virtual QWidget* createEditor(QWidget* parent, QStyleOptionViewItem const& option, 
        QModelIndex const& index) const;

    virtual void setEditorData(QWidget* editor, QModelIndex const& index) const;

    virtual void setModelData(QWidget* editor, QAbstractItemModel* model, QModelIndex const& index) const;

private:
	//! No copying
	GroupManagerDelegate(const GroupManagerDelegate& other);
	GroupManagerDelegate& operator=(const GroupManagerDelegate& other);

    BasicLineContentMatcher* matcher_;
};

#endif // GROUPMANAGERDELEGATE_H
