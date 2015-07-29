/* 
 *  	Created on: 6.6.2012
 *      Author: Antti Kamppi
 * 		filename: filesdelegate.h
 *		Project: Kactus 2
 */

#ifndef FILESDELEGATE_H
#define FILESDELEGATE_H

#include <QStyledItemDelegate>
#include <editors/ComponentEditor/common/MultilineDescriptionDelegate.h>

class FilesDelegate : public MultilineDescriptionDelegate
{
	Q_OBJECT

public:

	//! \brief The minimum height for the editor.
	static const int LIST_EDITOR_MIN_HEIGHT = 100;

	/*! \brief The constructor
	 *
	 * \param parent Pointer to the owner of the delegate.
	 *
	*/
	FilesDelegate(QObject *parent);
	
	//! \brief The destructor
	~FilesDelegate();

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

protected:

    //! Gets the descriptions column.
    virtual int descriptionColumn() const;

private slots:

	/*! \brief Commit the data from the sending editor and close the editor.
	 *
	*/
	void commitAndCloseEditor();

private:
	//! \brief No copying
	FilesDelegate(const FilesDelegate& other);

	//! \brief No assignment
	FilesDelegate& operator=(const FilesDelegate& other);


};

#endif // FILESDELEGATE_H
