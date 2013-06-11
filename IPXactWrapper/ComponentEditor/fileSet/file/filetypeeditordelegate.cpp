/* 
 *  	Created on: 7.12.2011
 *      Author: Antti Kamppi
 * 		filename: filetypeeditordelegate.cpp
 *		Project: Kactus 2
 */

#include "filetypeeditordelegate.h"
#include <common/widgets/fileTypeSelector/filetypeselector.h>

#include <QStringList>

FileTypeEditorDelegate::FileTypeEditorDelegate(QObject *parent):
QStyledItemDelegate(parent) {
}

FileTypeEditorDelegate::~FileTypeEditorDelegate() {
}

QWidget* FileTypeEditorDelegate::createEditor( QWidget* parent, 
											  const QStyleOptionViewItem& /*option*/,
											  const QModelIndex& /*index*/ ) const {

	FileTypeSelector* fileCombo = new FileTypeSelector(parent);
	return fileCombo;
}

void FileTypeEditorDelegate::setEditorData( QWidget* editor, const QModelIndex& index ) const {
	QString text = index.model()->data(index, Qt::DisplayRole).toString();

	FileTypeSelector* fileCombo = qobject_cast<FileTypeSelector*>(editor);
	Q_ASSERT(fileCombo);

	fileCombo->refresh();
	fileCombo->selectFileType(text);
}

void FileTypeEditorDelegate::setModelData( QWidget* editor, QAbstractItemModel* model, const QModelIndex& index ) const {
	FileTypeSelector* combo = qobject_cast<FileTypeSelector*>(editor);
	Q_ASSERT(combo);
	QString text = combo->currentText();
	model->setData(index, text, Qt::EditRole);
}
