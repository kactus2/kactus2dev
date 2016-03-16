//-----------------------------------------------------------------------------
// File: SingleDesignConfigurationInstantiationItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 25.01.2016
//
// Description:
// The item for one design configuration instantiation-item in the component editor's navigation tree.
//-----------------------------------------------------------------------------

#include "SingleDesignConfigurationInstantiationItem.h"

#include <editors/ComponentEditor/instantiations/DesignConfigurationInstantiationEditor.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/validators/InstantiationsValidator.h>

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationsItem::ComponentInstantiationsItem()
//-----------------------------------------------------------------------------
SingleDesignConfigurationInstantiationItem::SingleDesignConfigurationInstantiationItem(
    ComponentEditorTreeModel* model, 
    LibraryInterface* libHandler,
    QSharedPointer<Component> component, 
    QSharedPointer<DesignConfigurationInstantiation> instantiation,
    QSharedPointer<InstantiationsValidator> validator,
    ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
    instantiation_(instantiation),
    validator_(validator)
{
    setObjectName(tr("ComponentInstantiationsItem"));
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationsItem::~ComponentInstantiationsItem()
//-----------------------------------------------------------------------------
SingleDesignConfigurationInstantiationItem::~SingleDesignConfigurationInstantiationItem()
{

}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationsItem::getTooltip()
//-----------------------------------------------------------------------------
QString SingleDesignConfigurationInstantiationItem::getTooltip() const
{
    return tr("Contains the component instantiations of the component");
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationsItem::text()
//-----------------------------------------------------------------------------
QString SingleDesignConfigurationInstantiationItem::text() const
{
	return instantiation_->name();
}

//-----------------------------------------------------------------------------
// Function: SingleDesignConfigurationInstantiationItem::isValid()
//-----------------------------------------------------------------------------
bool SingleDesignConfigurationInstantiationItem::isValid() const
{
    return validator_->validateDesignConfigurationInstantiation(instantiation_);
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationsItem::editor()
//-----------------------------------------------------------------------------
ItemEditor* SingleDesignConfigurationInstantiationItem::editor()
{
	if (!editor_)
    {
 		editor_ = new DesignConfigurationInstantiationEditor(component_, instantiation_, libHandler_, 0);
 		editor_->setProtection(locked_);
 
 		connect(editor_, SIGNAL(contentChanged()), this, SLOT(onEditorChanged()), Qt::UniqueConnection);
 		connect(editor_, SIGNAL(helpUrlRequested(QString const&)), this, SIGNAL(helpUrlRequested(QString const&)));
	}
	return editor_;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationsItem::createChild()
//-----------------------------------------------------------------------------
void SingleDesignConfigurationInstantiationItem::createChild(int)
{
    // No children allowed.
}
