/* 
 *  	Created on: 7.12.2011
 *      Author: Antti Kamppi
 * 		filename: dirlistmanagerdelegate.h
 *		Project: Kactus 2
 */

#ifndef DIRLISTMANAGERDELEGATE_H
#define DIRLISTMANAGERDELEGATE_H

#include <common/delegates/lineeditdelegate.h>

#include <QFileInfo>

/*! \brief The delegate creates an editor to select a directory in DirListManager.
 *
 */
class DirListManagerDelegate : public LineEditDelegate {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param parent Pointer to the owner of this delegate.
	 * \param baseFile The location used as base for relative paths
	 *
	*/
	DirListManagerDelegate(QObject *parent, const QFileInfo& baseFile);
	
	//! \brief The destructor
	virtual ~DirListManagerDelegate();

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

	/*! \brief Save the data from the editor to the model.
	 *
	 * \param editor Pointer to the editor that contains the data to store.
	 * \param model Model that contains the data structure where data is to be saved to.
	 * \param index Model index identifying the item that's data is to be saved.
	 *
	*/
	virtual void setModelData(QWidget* editor, QAbstractItemModel* model, 
		const QModelIndex& index) const;

private:
	//! \brief No copying
	DirListManagerDelegate(const DirListManagerDelegate& other);

	//! \brief No assignment
	DirListManagerDelegate& operator=(const DirListManagerDelegate& other);

	//! \brief The base file to calculate the relative path to.
	QFileInfo baseFile_;
};

#endif // DIRLISTMANAGERDELEGATE_H
