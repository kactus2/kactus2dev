/* 
 *  	Created on: 14.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorviewitem.cpp
 *		Project: Kactus 2
 */

#include "componenteditorviewitem.h"
#include <editors/ComponentEditor/views/vieweditor.h>

#include <QFont>
#include <QApplication>

//-----------------------------------------------------------------------------
// Function: ComponentEditorViewItem()
//-----------------------------------------------------------------------------
ComponentEditorViewItem::ComponentEditorViewItem(QSharedPointer<View> view,
												 ComponentEditorTreeModel* model,
												 LibraryInterface* libHandler,
												 QSharedPointer<Component> component,
												 ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
    view_(view),
    editAction_(new QAction(tr("Edit"), this))
{
	Q_ASSERT(view_);

	setObjectName(tr("ComponentEditorViewItem: %1").arg(view->getName()));

    connect(editAction_, SIGNAL(triggered(bool)), this, SLOT(openItem()), Qt::UniqueConnection); 
}

//-----------------------------------------------------------------------------
// Function: ~ComponentEditorViewItem()
//-----------------------------------------------------------------------------
ComponentEditorViewItem::~ComponentEditorViewItem() {
}

//-----------------------------------------------------------------------------
// Function: text()
//-----------------------------------------------------------------------------
QString ComponentEditorViewItem::text() const {
	return view_->getName();
}

//-----------------------------------------------------------------------------
// Function: isValid()
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
// Function: editor()
//-----------------------------------------------------------------------------
ItemEditor* ComponentEditorViewItem::editor() {
	if (!editor_) {
		editor_ = new ViewEditor(component_, view_, libHandler_);
		editor_->setProtection(locked_);
		connect(editor_, SIGNAL(contentChanged()),
			this, SLOT(onEditorChanged()), Qt::UniqueConnection);
		connect(editor_, SIGNAL(helpUrlRequested(QString const&)),
			this, SIGNAL(helpUrlRequested(QString const&)));
	}
	return editor_;
}

//-----------------------------------------------------------------------------
// Function: getFont()
//-----------------------------------------------------------------------------
QFont ComponentEditorViewItem::getFont() const {
	return QApplication::font();
}

//-----------------------------------------------------------------------------
// Function: getTooltip()
//-----------------------------------------------------------------------------
QString ComponentEditorViewItem::getTooltip() const {
	return tr("Specifies a representation level of the component");
}

//-----------------------------------------------------------------------------
// Function: canBeOpened()
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
// Function: ComponentEditorViewItem::actions()
//-----------------------------------------------------------------------------
QList<QAction*> ComponentEditorViewItem::actions() const
{
    QList<QAction*> actionList;
    actionList.append(editAction_);

    return actionList;   
}

//-----------------------------------------------------------------------------
// Function: openItem()
//-----------------------------------------------------------------------------
void ComponentEditorViewItem::openItem() {
	// if item can't be opened
	if (!canBeOpened()) {
		return;
	}
	QString viewName = view_->getName();
	VLNV compVLNV = *component_->getVlnv();
	emit openDesign(compVLNV, viewName);
}


//-----------------------------------------------------------------------------
// Function: isHierarchical()
//-----------------------------------------------------------------------------
bool ComponentEditorViewItem::isHierarchical() const
{
    return view_->isHierarchical();       
}


//-----------------------------------------------------------------------------
// Function: hasIcon()
//-----------------------------------------------------------------------------
bool ComponentEditorViewItem::hasIcon() const
{
    return view_->isHierarchical();       
}

//-----------------------------------------------------------------------------
// Function: getIcon()
//-----------------------------------------------------------------------------
QIcon ComponentEditorViewItem::getIcon() const
{
    if ( view_->isHierarchical() )
    {
        return QIcon(":/icons/common/graphics/hierarchy.png");
    }
    else
    {
        return QIcon();
    }
}
