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
file_(file) {

	dependencies_.initialize();
	exportedNames_.initialize();
	imageTypes_.initialize();

	QGridLayout* layout = new QGridLayout(this);
	layout->addWidget(&dependencies_, 0, 0, 1, 2);
	layout->addWidget(&exportedNames_, 1, 0, 1, 1);
	layout->addWidget(&imageTypes_, 1, 1, 1, 1);

	connect(&dependencies_, SIGNAL(contentChanged()),
		this, SLOT(onDependenciesChange()), Qt::UniqueConnection);
	connect(&exportedNames_, SIGNAL(contentChanged()),
		this, SLOT(onExportsChange()), Qt::UniqueConnection);
	connect(&imageTypes_, SIGNAL(contentChanged()),
		this, SLOT(onImageTypesChange()), Qt::UniqueConnection);

}

FileExtraTab::~FileExtraTab() {

}

void FileExtraTab::refresh() {
	dependencies_.setItems(file_->getDependencies());
	exportedNames_.setItems(file_->getExportedNames());
	imageTypes_.setItems(file_->getImageTypes());
}

bool FileExtraTab::isValid() const {
 return true;
}

void FileExtraTab::onDependenciesChange() {
	file_->setDependencies(dependencies_.items());
	emit contentChanged();
}

void FileExtraTab::onExportsChange() {
	file_->setExportedNames(exportedNames_.items());
	emit contentChanged();
}

void FileExtraTab::onImageTypesChange() {
	file_->setImageTypes(imageTypes_.items());
	emit contentChanged();
}

void FileExtraTab::onDefineChange() {
	emit contentChanged();
}

void FileExtraTab::showEvent( QShowEvent* event ) {
	QWidget::showEvent(event);
	emit helpUrlRequested("componenteditor/fileextra.html");
}
