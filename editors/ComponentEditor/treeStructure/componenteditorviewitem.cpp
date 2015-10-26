//-----------------------------------------------------------------------------
// File: componenteditorviewitem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 14.05.2012
//
// Description:
// The item for a single view in the component editor's navigation tree.
//-----------------------------------------------------------------------------

#include "componenteditorviewitem.h"
#include <editors/ComponentEditor/views/vieweditor.h>

#include <library/LibraryManager/libraryhandler.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/View.h>
#include <IPXACTmodels/Component/ComponentInstantiation.h>
#include <IPXACTmodels/Component/DesignInstantiation.h>
#include <IPXACTmodels/Component/DesignConfigurationInstantiation.h>

//-----------------------------------------------------------------------------
// Function: ComponentEditorViewItem()
//-----------------------------------------------------------------------------
ComponentEditorViewItem::ComponentEditorViewItem(QSharedPointer<View> view,
												 ComponentEditorTreeModel* model,
												 LibraryInterface* libHandler,
												 QSharedPointer<Component> component,
                                                 QSharedPointer<ParameterFinder> parameterFinder,
                                                 QSharedPointer<ExpressionFormatter> expresionFormatter,
												 ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
view_(view),
editAction_(new QAction(tr("Edit"), this))
{
    setParameterFinder(parameterFinder);
    setExpressionFormatter(expresionFormatter);

	Q_ASSERT(view_);

	setObjectName(tr("ComponentEditorViewItem: %1").arg(view->name()));

    connect(editAction_, SIGNAL(triggered(bool)), this, SLOT(openItem()), Qt::UniqueConnection); 
}

//-----------------------------------------------------------------------------
// Function: ~ComponentEditorViewItem()
//-----------------------------------------------------------------------------
ComponentEditorViewItem::~ComponentEditorViewItem()
{

}

//-----------------------------------------------------------------------------
// Function: text()
//-----------------------------------------------------------------------------
QString ComponentEditorViewItem::text() const
{
	return view_->name();
}

//-----------------------------------------------------------------------------
// Function: isValid()
//-----------------------------------------------------------------------------
bool ComponentEditorViewItem::isValid() const
{
	// check that view is valid
/*	if (!view_->isValid(component_->getFileSetNames(), component_->getChoices())) {
		return false;
	}
	// check that the reference can be found
	if (view_->isHierarchical() && !libHandler_->contains(view_->getHierarchyRef())) {
		return false;
	}
    
    if (!component_->validateParameters(view_->getParameters()))
    {
        return false;
    }*/

	// view was valid and the reference was found
	return true;
}

//-----------------------------------------------------------------------------
// Function: editor()
//-----------------------------------------------------------------------------
ItemEditor* ComponentEditorViewItem::editor()
{
	if (!editor_)
    {
        QSharedPointer<ComponentInstantiation> componentInstantiation = getComponentInstantiation();
        QSharedPointer<DesignInstantiation> designInstantiation = getDesignInstantiation();
        QSharedPointer<DesignConfigurationInstantiation> designConfigurationInstantiation =
            getDesignConfigurationInstantiation();

        editor_ = new ViewEditor(component_, view_, componentInstantiation, designInstantiation,
            designConfigurationInstantiation, libHandler_, parameterFinder_, expressionFormatter_);
		editor_->setProtection(locked_);
		connect(editor_, SIGNAL(contentChanged()), this, SLOT(onEditorChanged()), Qt::UniqueConnection);
		connect(editor_, SIGNAL(helpUrlRequested(QString const&)), this, SIGNAL(helpUrlRequested(QString const&)));

        connectItemEditorToReferenceCounter();

        connectReferenceTree();
	}
	return editor_;
}

//-----------------------------------------------------------------------------
// Function: getTooltip()
//-----------------------------------------------------------------------------
QString ComponentEditorViewItem::getTooltip() const
{
	return tr("Specifies a representation level of the component");
}

//-----------------------------------------------------------------------------
// Function: canBeOpened()
//-----------------------------------------------------------------------------
bool ComponentEditorViewItem::canBeOpened() const
{
    if (view_->isHierarchical())
    {
        QSharedPointer<VLNV> referencedVLNV;

        if (!view_->getDesignConfigurationInstantiationRef().isEmpty())
        {
            QSharedPointer<DesignConfigurationInstantiation> instantiation = getDesignConfigurationInstantiation();
            referencedVLNV = instantiation->getDesignConfigurationReference();
        }
        else if (!view_->getDesignInstantiationRef().isEmpty())
        {
            QSharedPointer<DesignInstantiation> instantiation = getDesignInstantiation();
            referencedVLNV = instantiation->getDesignReference();
        }

        QSharedPointer<Document const> libComp = libHandler_->getModelReadOnly(component_->getVlnv());
        QSharedPointer<Component const> comp = libComp.staticCast<Component const>();
        VLNV originalRef = comp->getHierRef(view_->name());
        return originalRef == *referencedVLNV.data();
    }

    return false;
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
void ComponentEditorViewItem::openItem()
{
	// if item can't be opened
	if (!canBeOpened())
    {
		return;
	}
	QString viewName = view_->name();
	VLNV compVLNV = component_->getVlnv();
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

//-----------------------------------------------------------------------------
// Function: componenteditorviewitem::getComponentInstantiation()
//-----------------------------------------------------------------------------
QSharedPointer<ComponentInstantiation> ComponentEditorViewItem::getComponentInstantiation() const
{
    foreach (QSharedPointer<ComponentInstantiation> instantiation, *component_->getComponentInstantiations())
    {
        if (instantiation->name() == view_->getComponentInstantiationRef())
        {
            return instantiation;
        }
    }

    return QSharedPointer<ComponentInstantiation> ();
}

//-----------------------------------------------------------------------------
// Function: componenteditorviewitem::getDesignInstantiation()
//-----------------------------------------------------------------------------
QSharedPointer<DesignInstantiation> ComponentEditorViewItem::getDesignInstantiation() const
{
    foreach (QSharedPointer<DesignInstantiation> instantiation, *component_->getDesignInstantiations())
    {
        if (instantiation->name() == view_->getDesignInstantiationRef())
        {
            return instantiation;
        }
    }

    return QSharedPointer<DesignInstantiation> ();
}

//-----------------------------------------------------------------------------
// Function: componenteditorviewitem::getDesignConfigurationInstantiation()
//-----------------------------------------------------------------------------
QSharedPointer<DesignConfigurationInstantiation> ComponentEditorViewItem::getDesignConfigurationInstantiation()
    const
{
    foreach (QSharedPointer<DesignConfigurationInstantiation> instantiation,
        *component_->getDesignConfigurationInstantiations())
    {
        if (instantiation->name() == view_->getDesignConfigurationInstantiationRef())
        {
            return instantiation;
        }
    }

    return QSharedPointer<DesignConfigurationInstantiation> ();
}