/* 
 *  	Created on: 28.8.2012
 *      Author: Antti Kamppi
 * 		filename: enumeratedvaluedelegate.h
 *		Project: Kactus 2
 */

#ifndef ENUMERATEDVALUEDELEGATE_H
#define ENUMERATEDVALUEDELEGATE_H

#include <QStyledItemDelegate>

/*! \brief The delegate that provides editors to add/remove/edit the enumerated values of field.
 *
 */
class EnumeratedValueDelegate : public QStyledItemDelegate {
	Q_OBJECT

public:

	//! \brief Defines the columns used in the enumeration values table.
	enum Column {
		NAME_COLUMN = 0,
		VALUE_COLUMN,
		USAGE_COLUMN,
		COLUMN_COUNT
	};

	/*! \brief The constructor
	 *
	 * \param parent Pointer to the owner of the delegate.
	 *
	*/
	EnumeratedValueDelegate(QObject *parent);	

	//! \brief The destructor
	virtual ~EnumeratedValueDelegate();

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
	EnumeratedValueDelegate(const EnumeratedValueDelegate& other);

	//! \brief No assignment
	EnumeratedValueDelegate& operator=(const EnumeratedValueDelegate& other);
};

#endif // ENUMERATEDVALUEDELEGATE_H
