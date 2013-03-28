//-----------------------------------------------------------------------------
// File: ComponentEditorSystemViewItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 13.7.2012
//
// Description:
// The item for a single software view in the component editor's navigation tree.
//-----------------------------------------------------------------------------

#include "ComponentEditorSystemViewItem.h"

#include <QApplication>
#include <QFont>

ComponentEditorSystemViewItem::ComponentEditorSystemViewItem(
	QSharedPointer<SystemView> systemView,
	ComponentEditorTreeModel* model,
	LibraryInterface* libHandler,
	QSharedPointer<Component> component,
	ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
systemView_(systemView),
editor_(component, systemView, libHandler, NULL) {

	Q_ASSERT(systemView_);
	connect(&editor_, SIGNAL(contentChanged()),
		this, SLOT(onEditorChanged()), Qt::UniqueConnection);
	connect(&editor_, SIGNAL(helpUrlRequested(QString const&)),
		this, SIGNAL(helpUrlRequested(QString const&)));
}

ComponentEditorSystemViewItem::~ComponentEditorSystemViewItem() {
}

QString ComponentEditorSystemViewItem::getTooltip() const {
	return tr("Contains the details of a software view");
}

QString ComponentEditorSystemViewItem::text() const {
	return systemView_->getName();
}

bool ComponentEditorSystemViewItem::isValid() const {

	QStringList fileSetNames = component_->getFileSetNames();
	QStringList viewNames = component_->getViewNames();

	// if system view is not valid
	if (!systemView_->isValid(fileSetNames, viewNames)) {
		return false;
	}

	// check that the reference is found
	if (!libHandler_->contains(systemView_->getHierarchyRef())) {
		return false;
	}

	// system view was valid and reference was found
	return true;
}

ItemEditor* ComponentEditorSystemViewItem::editor() {
	return &editor_;
}

const ItemEditor* ComponentEditorSystemViewItem::editor() const {
	return &editor_;
}

QFont ComponentEditorSystemViewItem::getFont() const {
	return QApplication::font();
}

bool ComponentEditorSystemViewItem::canBeOpened() const {

	// if the library does not contain the referenced object
	if (!libHandler_->contains(systemView_->getHierarchyRef())) {
		return false;
	}

	// check that the reference has not been changed
	// if it has then there is no way to open the design because the changes have not
	// been made to the library
	QSharedPointer<LibraryComponent const> libComp = libHandler_->getModelReadOnly(*component_->getVlnv());
	QSharedPointer<Component const> comp = libComp.staticCast<Component const>();
	VLNV originalRef = comp->getHierSystemRef(systemView_->getName());
	return originalRef == systemView_->getHierarchyRef();
}

void ComponentEditorSystemViewItem::openItem( bool builtinEditor /*= false*/ ) {
	// if item can't be opened
	if (!canBeOpened()) {
		emit errorMessage(tr("The changes to component must be saved before view can be opened."));
		return;
	}

	QString viewName = systemView_->getName();
	VLNV compVLNV = *component_->getVlnv();
	emit openSystemDesign(compVLNV, viewName);
}
