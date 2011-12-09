/* 
 *  	Created on: 7.12.2011
 *      Author: Antti Kamppi
 * 		filename: dirlistmanagerdelegate.cpp
 *		Project: Kactus 2
 */

#include "dirlistmanagerdelegate.h"

#include <models/generaldeclarations.h>

#include <common/widgets/dirLineEdit/dirlineedit.h>

#include <QDir>
#include <QPoint>

DirListManagerDelegate::DirListManagerDelegate(QObject *parent,
											   const QFileInfo& baseFile):
LineEditDelegate(parent),
baseFile_(baseFile) {
}

DirListManagerDelegate::~DirListManagerDelegate() {
}

QWidget* DirListManagerDelegate::createEditor( QWidget* parent, 
											  const QStyleOptionViewItem&, 
											  const QModelIndex&) const {


	DirLineEdit* edit = new DirLineEdit(parent, baseFile_);
	connect(edit, SIGNAL(editingFinished()),
		this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
	edit->setMinimumHeight(LineEditDelegate::MINIMUM_EDITOR_HEIGHT);
	return edit;
}

void DirListManagerDelegate::setModelData( QWidget* editor, QAbstractItemModel* model, 
										  const QModelIndex& index ) const {

	DirLineEdit* edit = qobject_cast<DirLineEdit*>(editor);
	Q_ASSERT(edit);

	QString dir = edit->text();

	//create QDir instance that refers to the base location.
	QDir baseDir(baseFile_.absoluteDir());

	// get the relative directory path
	QString relDir = QDir::cleanPath(baseDir.relativeFilePath(dir));

	// if relative directory could not be constructed
	if (relDir.isEmpty()) {
		model->setData(index, tr("error: Could not create a relative path."), Qt::EditRole);
	}

	model->setData(index, relDir, Qt::EditRole);
}
