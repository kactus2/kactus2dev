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

#include <editors/ComponentEditor/software/systemView/SystemViewEditor.h>

#include <library/LibraryManager/libraryinterface.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/kactusExtensions/SystemView.h>

//-----------------------------------------------------------------------------
// Function: ComponentEditorSystemViewItem::ComponentEditorSystemViewItem()
//-----------------------------------------------------------------------------
ComponentEditorSystemViewItem::ComponentEditorSystemViewItem(QSharedPointer<SystemView> systemView,
    ComponentEditorTreeModel* model, 
    LibraryInterface* libHandler,
    QSharedPointer<Component> component,
    ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
    systemView_(systemView),
    editAction_(new QAction(tr("Edit"), this))
{
    Q_ASSERT(systemView_);
    connect(editAction_, SIGNAL(triggered(bool)), this, SLOT(openItem()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorSystemViewItem::~ComponentEditorSystemViewItem()
//-----------------------------------------------------------------------------
ComponentEditorSystemViewItem::~ComponentEditorSystemViewItem()
{
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorSystemViewItem::getTooltip()
//-----------------------------------------------------------------------------
QString ComponentEditorSystemViewItem::getTooltip() const
{
	return tr("Contains the details of a software view");
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorSystemViewItem::text()
//-----------------------------------------------------------------------------
QString ComponentEditorSystemViewItem::text() const
{
	return systemView_->name();
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorSystemViewItem::isValid()
//-----------------------------------------------------------------------------
bool ComponentEditorSystemViewItem::isValid() const
{
	QStringList fileSetNames = component_->getFileSetNames();
	QStringList viewNames = component_->getViewNames();

	// if system view is not valid
	if (!systemView_->isValid(fileSetNames, viewNames))
    {
		return false;
	}

	// check that the reference is found
	if (!libHandler_->contains(systemView_->getHierarchyRef()))
    {
		return false;
	}

	// system view was valid and reference was found
	return true;
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorSystemViewItem::editor()
//-----------------------------------------------------------------------------
ItemEditor* ComponentEditorSystemViewItem::editor()
{
	if (!editor_)
    {
		editor_ = new SystemViewEditor(component_, systemView_, libHandler_, NULL);
		editor_->setProtection(locked_);
		connect(editor_, SIGNAL(contentChanged()), this, SLOT(onEditorChanged()), Qt::UniqueConnection);
		connect(editor_, SIGNAL(helpUrlRequested(QString const&)), this, SIGNAL(helpUrlRequested(QString const&)));
	}

	return editor_;
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorSystemViewItem::canBeOpened()
//-----------------------------------------------------------------------------
bool ComponentEditorSystemViewItem::canBeOpened() const
{
	// if the library does not contain the referenced object
	if (!libHandler_->contains(systemView_->getHierarchyRef()))
    {
		return false;
	}

	// check that the reference has not been changed
	// if it has then there is no way to open the design because the changes have not
	// been made to the library
	QSharedPointer<Document const> libComp = libHandler_->getModelReadOnly(component_->getVlnv());
	QSharedPointer<Component const> comp = libComp.staticCast<Component const>();
	VLNV originalRef = comp->getHierSystemRef(systemView_->name());
	return originalRef == systemView_->getHierarchyRef();
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorSystemViewItem::actions()
//-----------------------------------------------------------------------------
QList<QAction*> ComponentEditorSystemViewItem::actions() const
{
    QList<QAction*> actionList;
    actionList.append(editAction_);

    return actionList;   
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorSystemViewItem::openItem()
//-----------------------------------------------------------------------------
void ComponentEditorSystemViewItem::openItem()
{
	// if item can't be opened
	if (!canBeOpened())
    {
		emit errorMessage(tr("The changes to component must be saved before view can be opened."));
		return;
	}

	QString viewName = systemView_->name();
	VLNV compVLNV = component_->getVlnv();
	emit openSystemDesign(compVLNV, viewName);
}
