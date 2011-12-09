/* 
 *  	Created on: 9.12.2011
 *      Author: Antti Kamppi
 * 		filename: filesetrefeditordelegate.h
 *		Project: Kactus 2
 */

#ifndef FILESETREFEDITORDELEGATE_H
#define FILESETREFEDITORDELEGATE_H

#include <common/delegates/combodelegate.h>

#include <models/component.h>

#include <QSharedPointer>

class FileSetRefEditorDelegate : public ComboDelegate {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param parent The pointer to the owner of this delegate.
	 * \param component The component that's file sets are being selected.
	 *
	*/
	FileSetRefEditorDelegate(QObject *parent, QSharedPointer<Component> component);
	
	//! \brief The destructor
	virtual ~FileSetRefEditorDelegate();

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
	FileSetRefEditorDelegate(const FileSetRefEditorDelegate& other);

	//! \brief No assignment
	FileSetRefEditorDelegate& operator=(const FileSetRefEditorDelegate& other);

	//! \brief Pointer to the component that's file set are being selected.
	QSharedPointer<Component> component_;
};

#endif // FILESETREFEDITORDELEGATE_H
