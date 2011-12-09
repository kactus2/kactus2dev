/* 
 *
 *  Created on: 15.4.2011
 *      Author: Antti Kamppi
 * 		filename: filesetrefeditor.cpp
 */

#include "filesetrefeditor.h"

#include "filesetrefeditordelegate.h"

#include <models/component.h>
#include <common/dialogs/comboSelector/comboselector.h>

FileSetRefEditor::FileSetRefEditor(QSharedPointer<Component> component,
								   const QString title /*= tr("List")*/, 
								   QWidget *parent /*= 0*/, 
								   const QStringList& items /*= QStringList()*/ ):
ListManager(title, parent, items),
component_(component) {

	Q_ASSERT(component_);
	view_.setItemDelegate(new FileSetRefEditorDelegate(this, component_));
}

FileSetRefEditor::~FileSetRefEditor() {
}

