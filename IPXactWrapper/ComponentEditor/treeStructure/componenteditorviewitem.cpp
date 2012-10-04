/* 
 *  	Created on: 14.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorviewitem.cpp
 *		Project: Kactus 2
 */

#include "componenteditorviewitem.h"

#include <QFont>
#include <QApplication>

ComponentEditorViewItem::ComponentEditorViewItem(QSharedPointer<View> view,
												 ComponentEditorTreeModel* model,
												 LibraryInterface* libHandler,
												 QSharedPointer<Component> component,
												 ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
view_(view),
editor_(component, view, libHandler) {
	
	Q_ASSERT(view_);

	setObjectName(tr("ComponentEditorViewItem: %1").arg(view->getName()));
	
	editor_.hide();

	connect(&editor_, SIGNAL(contentChanged()),
		this, SLOT(onEditorChanged()), Qt::UniqueConnection);
	connect(&editor_, SIGNAL(helpUrlRequested(QString const&)),
		this, SIGNAL(helpUrlRequested(QString const&)));
}

ComponentEditorViewItem::~ComponentEditorViewItem() {
}

QString ComponentEditorViewItem::text() const {
	return view_->getName();
}

bool ComponentEditorViewItem::isValid() const {
	// check that view is valid
	if (!view_->isValid(component_->getFileSetNames())) {
		return false;
	}
	// check that the reference can be found
	if (view_->isHierarchical() && !libHandler_->contains(view_->getHierarchyRef())) {
		return false;
	}

	// view was valid and the reference was found
	return true;
}

ItemEditor* ComponentEditorViewItem::editor() {
	return &editor_;
}

const ItemEditor* ComponentEditorViewItem::editor() const {
	return &editor_;
}

QFont ComponentEditorViewItem::getFont() const {
	return QApplication::font();
}

QString ComponentEditorViewItem::getTooltip() const {
	return tr("Specifies a representation level of the component");
}

bool ComponentEditorViewItem::canBeOpened() const {
	// if view is not hierarchical then it can't be opened
	if (!view_->isHierarchical()) {
		return false;
	}
	// if the library does not contain the referenced object
	else if (!libHandler_->contains(view_->getHierarchyRef())) {
		return false;
	}

	// check that the reference has not been changed
	// if it has then there is no way to open the design because the changes have not
	// been made to the library
	QSharedPointer<LibraryComponent const> libComp = libHandler_->getModelReadOnly(*component_->getVlnv());
	QSharedPointer<Component const> comp = libComp.staticCast<Component const>();
	VLNV originalRef = comp->getHierRef(view_->getName());
	return originalRef == view_->getHierarchyRef();
}

void ComponentEditorViewItem::openItem( bool ) {
	// if item can't be opened
	if (!canBeOpened()) {
		emit errorMessage(tr("The changes to component must be saved before view can be opened."));
		return;
	}
	QString viewName = view_->getName();
	VLNV compVLNV = *component_->getVlnv();
	emit openDesign(compVLNV, viewName);
}
