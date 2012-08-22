/* 
 *  	Created on: 22.8.2012
 *      Author: Antti Kamppi
 * 		filename: memorymapsdelegate.h
 *		Project: Kactus 2
 */

#ifndef MEMORYMAPSDELEGATE_H
#define MEMORYMAPSDELEGATE_H

#include <QStyledItemDelegate>

/*! \brief The delegate to provide editors to add/remove/edit the memory maps of a component.
 *
 */
class MemoryMapsDelegate : public QStyledItemDelegate {
	Q_OBJECT

public:

	//! \brief Defines the columns used in the model.
	enum Column {
		NAME_COLUMN = 0,
		AUB_COLUMN,
		DESC_COLUMN,
		COLUMN_COUNT
	};

	/*! \brief The constructor
	 *
	 * \param parent Pointer to the owner of the delegate.
	 *
	*/
	MemoryMapsDelegate(QObject *parent);
	
	//! \brief The destructor
	virtual ~MemoryMapsDelegate();

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
	MemoryMapsDelegate(const MemoryMapsDelegate& other);

	//! \brief No assignment
	MemoryMapsDelegate& operator=(const MemoryMapsDelegate& other);
};

#endif // MEMORYMAPSDELEGATE_H
