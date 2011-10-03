/* 
 *  	Created on: 8.8.2011
 *      Author: Antti Kamppi
 * 		filename: objectremovedialog.h
 *		Project: Kactus 2
 */

#ifndef OBJECTREMOVEDIALOG_H
#define OBJECTREMOVEDIALOG_H

#include "objectremovemodel.h"

#include <QDialog>
#include <QTableView>
#include <QSortFilterProxyModel>
#include <QPushButton>
#include <QSize>
#include <QList>
#include <QCheckBox>

/*! \brief Dialog for user to select which items to be removed.
 *
 */
class ObjectRemoveDialog : public QDialog {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param parent Pointer to the owner of this dialog.
	 * \param f Widget flags for the dialog.
	 *
	*/
	ObjectRemoveDialog(QWidget *parent = 0, Qt::WindowFlags f = 0);
	
	//! \brief The destructor
	virtual ~ObjectRemoveDialog();

	/*! \brief Add a new item to the table.
	 *
	 * \param item Reference to the item to add.
	 *
	*/
	void addItem(const ObjectRemoveModel::Item& item);

	/*! \brief Create new item for given vlnv object.
	 *
	 * \param vlnv Reference to the vlnv item is created for.
	 *
	*/
	void createItem(const VLNV& vlnv, bool locked = false);

	/*! \brief Create new item for given file
	 *
	 * \param filePath Path to the file 
	 *
	*/
	void createItem(const QString& filePath);

	/*! \brief Get the size hint of this dialog.
	 *
	 * \return QSize contains the desired size for this dialog.
	*/
	virtual QSize sizeHint() const;

	/*! \brief Get list of items that should be removed.
	 *
	 * \return QList containing the items to remove.
	*/
	QList<ObjectRemoveModel::Item> getItemsToRemove() const;

private:

	//! \brief No copying
	ObjectRemoveDialog(const ObjectRemoveDialog& other);

	//! \brief No assignment
	ObjectRemoveDialog& operator=(const ObjectRemoveDialog& other);

	//! \brief Ok button to accept the dialog.
	QPushButton okButton_;

	//! \brief Cancel button to reject the dialog.
	QPushButton cancelButton_;

	//! \brief Check box to select/deselect all items.
	QCheckBox selectionBox_;

	//! \brief View to display the contents of the model.
	QTableView view_;

	//! \brief Filter to sort the items to display.
	QSortFilterProxyModel filter_;

	//! \brief The model that contains the items to display.
	ObjectRemoveModel model_;
	
};

#endif // OBJECTREMOVEDIALOG_H
