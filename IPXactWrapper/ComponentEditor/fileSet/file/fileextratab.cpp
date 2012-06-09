/* 
 *
 *  Created on: 7.2.2011
 *      Author: Antti Kamppi
 * 		filename: fileextratab.cpp
 */

#include "fileextratab.h"

#include <QGridLayout>
#include <QTableView>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QSortFilterProxyModel>

FileExtraTab::FileExtraTab(LibraryInterface* handler,
						   QSharedPointer<Component> component,
						   QSharedPointer<File> file, 
						   QWidget *parent ):
QWidget(parent), 
dependencies_(tr("Dependencies"), handler, component, this),
exportedNames_(tr("Exported names"), this),
imageTypes_(tr("Image types"), this),
defineView_(this),
defineModel_(this, file),
file_(file) {

	dependencies_.initialize();
	exportedNames_.initialize();
	imageTypes_.initialize();

	// create a proxy model to sort the define items and connect it between
	// the defineView and defineModel
	QSortFilterProxyModel* proxy = new QSortFilterProxyModel(this);
	proxy->setSourceModel(&defineModel_);
	defineView_.setModel(proxy);

	QGroupBox* defineBox = new QGroupBox(tr("Defines"), this);
	QVBoxLayout* groupLayout = new QVBoxLayout(defineBox);
	groupLayout->addWidget(&defineView_);

	QGridLayout* layout = new QGridLayout(this);
	layout->addWidget(&dependencies_, 0, 0, 1, 2);
	layout->addWidget(&exportedNames_, 1, 0, 1, 1);
	layout->addWidget(&imageTypes_, 1, 1, 1, 1);
	layout->addWidget(defineBox, 2, 0, 1, 2);

	connect(&dependencies_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&exportedNames_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&imageTypes_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	// TODO add signal from definemodel

}

FileExtraTab::~FileExtraTab() {

}

void FileExtraTab::apply() {
	file_->setDependencies(dependencies_.items());
	file_->setExportedNames(exportedNames_.items());
	file_->setImageTypes(imageTypes_.items());
}

void FileExtraTab::restore() {
	dependencies_.setItems(file_->getDependencies());
	exportedNames_.setItems(file_->getExportedNames());
	imageTypes_.setItems(file_->getImageTypes());
}

bool FileExtraTab::isValid() const {
 return true;
}
