/* 
 *  	Created on: 9.12.2011
 *      Author: Antti Kamppi
 * 		filename: filesetrefeditordelegate.cpp
 *		Project: Kactus 2
 */

#include "filesetrefeditordelegate.h"

#include <QComboBox>
#include <QLineEdit>

FileSetRefEditorDelegate::FileSetRefEditorDelegate( QObject *parent, 
												   QSharedPointer<Component> component ):
ComboDelegate(parent), 
component_(component) {
	Q_ASSERT(component_);
}

FileSetRefEditorDelegate::~FileSetRefEditorDelegate() {
}

QWidget* FileSetRefEditorDelegate::createEditor( QWidget* parent,
												const QStyleOptionViewItem& option,
												const QModelIndex& index) const {

	Q_ASSERT(component_);
	QComboBox* combo = qobject_cast<QComboBox*>(
		ComboDelegate::createEditor(parent, option, index));
	Q_ASSERT(combo);

	QStringList filesetNames = component_->getFileSetNames();
	combo->addItems(filesetNames);
	return combo;
}
