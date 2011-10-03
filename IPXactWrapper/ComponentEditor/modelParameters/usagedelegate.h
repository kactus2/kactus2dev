/* 
 *
 *  Created on: 31.3.2011
 *      Author: Antti Kamppi
 * 		filename: usagedelegate.h
 */

#ifndef USAGEDELEGATE_H
#define USAGEDELEGATE_H

#include <QStyledItemDelegate>

/*! \brief Delegate that provides widgets for editing model parameters.
 *
 */
class UsageDelegate : public QStyledItemDelegate {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param usageColumn The column which represents the usage-items.
	 * \param parent Pointer to the owner of this instance.
	 *
	*/
	UsageDelegate(int usageColumn, QObject *parent = 0);

	//! \brief The destructor
	virtual ~UsageDelegate();

	/*! \brief Create a new editor for the given item
	 *
	 * \param parent Owner for the editor.
	 * \param option Contains options for the editor.
	 * \param index Model index idetifying the item.
	 *
	 * \return Pointer to the editor to be used to edit the item.
	*/
	virtual QWidget* createEditor(QWidget* parent, 
		const QStyleOptionViewItem& option, 
		const QModelIndex& index) const;

	/*! \brief Set the data for the editor.
	 *
	 * \param editor Pointer to the editor where the data is to be set.
	 * \param index Model index identifying the item that's data is to be set.
	 *
	*/
	virtual void setEditorData(QWidget* editor, const QModelIndex& index) const;

	/*! \brief Save the data from the editor to the model.
	 *
	 * \param editor Pointer to the editor that contains the data to store.
	 * \param model Model that contains the data structure where data is to be saved to.
	 * \param index Model index identifying the item that's data is to be saved.
	 *
	*/
	virtual void setModelData(QWidget* editor, QAbstractItemModel* model, 
		const QModelIndex& index) const;

private slots:

	/*! \brief Commit the data from the sending editor and close the editor.
	 *
	*/
	void commitAndCloseEditor();

private:

	//! \brief No copying
	UsageDelegate(const UsageDelegate& other);

	//! No assignment
	UsageDelegate& operator=(const UsageDelegate& other);

	//! \brief The column in the model that contains the usage info.
	int usageColumn_;
	
};

#endif // USAGEDELEGATE_H
