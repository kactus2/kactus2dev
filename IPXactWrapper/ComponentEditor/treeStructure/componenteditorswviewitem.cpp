/* 
 *  	Created on: 24.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorswviewitem.cpp
 *		Project: Kactus 2
 */

#include "componenteditorswviewitem.h"

#include <QApplication>
#include <QFont>

ComponentEditorSWViewItem::ComponentEditorSWViewItem(
	QSharedPointer<SWView> swView,
	ComponentEditorTreeModel* model,
	LibraryInterface* libHandler,
	QSharedPointer<Component> component,
	ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
swView_(swView),
editor_(component, swView, libHandler, NULL) {

	Q_ASSERT(swView_);
	connect(&editor_, SIGNAL(contentChanged()),
		this, SLOT(onEditorChanged()), Qt::UniqueConnection);
	connect(&editor_, SIGNAL(helpUrlRequested(QString const&)),
		this, SIGNAL(helpUrlRequested(QString const&)));
}

ComponentEditorSWViewItem::~ComponentEditorSWViewItem() {
}

QString ComponentEditorSWViewItem::getTooltip() const {
	return tr("Contains the details of a software view");
}

QString ComponentEditorSWViewItem::text() const {
	return swView_->getName();
}

bool ComponentEditorSWViewItem::isValid() const {

	QStringList fileSetNames = component_->getFileSetNames();
	QStringList cpuNames = component_->getCpuNames();

	// if sw view is not valid
	if (!swView_->isValid(fileSetNames, cpuNames)) {
		return false;
	}

	// check that the reference is found if its been defined
	VLNV swHierRef = swView_->getHierarchyRef();
	if (swHierRef.isValid() && !libHandler_->contains(swHierRef)) {
		return false;
	}

	// sw view was valid and reference was found
	return true;
}

ItemEditor* ComponentEditorSWViewItem::editor() {
	return &editor_;
}

const ItemEditor* ComponentEditorSWViewItem::editor() const {
	return &editor_;
}

QFont ComponentEditorSWViewItem::getFont() const {
	return QApplication::font();
}

bool ComponentEditorSWViewItem::canBeOpened() const {
	// if the library does not contain the referenced object
	if (!libHandler_->contains(swView_->getHierarchyRef())) {
		return false;
	}

	// check that the reference has not been changed
	// if it has then there is no way to open the design because the changes have not
	// been made to the library
	QSharedPointer<LibraryComponent const> libComp = libHandler_->getModelReadOnly(*component_->getVlnv());
	QSharedPointer<Component const> comp = libComp.staticCast<Component const>();
	VLNV originalRef = comp->getHierSWRef(swView_->getName());
	return originalRef == swView_->getHierarchyRef();
}

void ComponentEditorSWViewItem::openItem( bool builtinEditor /*= false*/ ) {
	// if item can't be opened
	if (!canBeOpened()) {
		emit errorMessage(tr("The changes to component must be saved before view can be opened."));
		return;
	}
	QString viewName = swView_->getName();
	VLNV compVLNV = *component_->getVlnv();
	emit openSWDesign(compVLNV, viewName);
}
