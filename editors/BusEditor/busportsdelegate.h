/*
 *  Created on: 27.6.2011
 *      Author: Antti Kamppi
 * 		filename: busportsdelegate.h
 */

#ifndef BUSPORTSDELEGATE_H
#define BUSPORTSDELEGATE_H

#include <QStyledItemDelegate>

/*! \brief The delegate that provides editors to edit the settings of port in Abstraction Definition.
 *
 */
class BusPortsDelegate : public QStyledItemDelegate {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param parent Pointer to the owner of this instance
	 *
	*/
	BusPortsDelegate(QObject *parent);
	
	//! \brief The destructor
	virtual ~BusPortsDelegate();

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
	BusPortsDelegate(const BusPortsDelegate& other);

	//! \brief No assignment
	BusPortsDelegate& operator=(const BusPortsDelegate& other);
};

#endif // BUSPORTSDELEGATE_H
