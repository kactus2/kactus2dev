/* 
 *  	Created on: 7.12.2011
 *      Author: Antti Kamppi
 * 		filename: filetypeeditordelegate.h
 *		Project: Kactus 2
 */

#ifndef FILETYPEEDITORDELEGATE_H
#define FILETYPEEDITORDELEGATE_H

#include <common/delegates/ComboDelegate/combodelegate.h>

#include <QStyledItemDelegate>

/*! \brief Provides editor to select a type for a file.
 *
 */
class FileTypeEditorDelegate : public ComboDelegate {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param parent Pointer to the owner of this delegate.
	 *
	*/
	FileTypeEditorDelegate(QObject *parent);
	
	//! \brief The destructor
	virtual ~FileTypeEditorDelegate();

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
	FileTypeEditorDelegate(const FileTypeEditorDelegate& other);

	//! \brief No assignment
	FileTypeEditorDelegate& operator=(const FileTypeEditorDelegate& other);
};

#endif // FILETYPEEDITORDELEGATE_H
