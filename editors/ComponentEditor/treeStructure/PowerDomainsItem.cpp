//-----------------------------------------------------------------------------
// File: PowerDomainsItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 05.10.2023
//
// Description:
// The Power domains-item in the component editor's navigation tree.
//-----------------------------------------------------------------------------

#include "PowerDomainsItem.h"

#include <IPXACTmodels/Component/Component.h>

#include <editors/ComponentEditor/powerDomains/PowerDomainsEditor.h>

//-----------------------------------------------------------------------------
// Function: PowerDomainsItem::PowerDomainsItem()
//-----------------------------------------------------------------------------
PowerDomainsItem::PowerDomainsItem(ComponentEditorTreeModel* model, LibraryInterface* libHandler,
    QSharedPointer<Component> component, QSharedPointer<ReferenceCounter> referenceCounter, 
    ExpressionSet expressions, ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
expressions_(expressions)
{

    setReferenceCounter(referenceCounter);
}

//-----------------------------------------------------------------------------
// Function: PowerDomainsItem::~PowerDomainsItem()
//-----------------------------------------------------------------------------
PowerDomainsItem::~PowerDomainsItem()
{

}

//-----------------------------------------------------------------------------
// Function: PowerDomainsItem::getFont()
//-----------------------------------------------------------------------------
QFont PowerDomainsItem::getFont() const
{
    QFont font(ComponentEditorItem::getFont());
    font.setBold(!component_->getPowerDomains()->isEmpty());
    return font;
}

//-----------------------------------------------------------------------------
// Function: PowerDomainsItem::text()
//-----------------------------------------------------------------------------
QString PowerDomainsItem::text() const
{
	return tr("Power domains");
}

//-----------------------------------------------------------------------------
// Function: PowerDomainsItem::editor()
//-----------------------------------------------------------------------------
ItemEditor* PowerDomainsItem::editor()
{
	if (!editor_)
    {
        editor_ = new PowerDomainsEditor(component_, libHandler_, expressions_);
        editor_->setProtection(locked_);
        connect(editor_, SIGNAL(contentChanged()), this, SLOT(onEditorChanged()), Qt::UniqueConnection);
        connect(editor_, SIGNAL(helpUrlRequested(QString const&)), this, SIGNAL(helpUrlRequested(QString const&)));

        connectItemEditorToReferenceCounter();
	}

	return editor_;
}

//-----------------------------------------------------------------------------
// Function: PowerDomainsItem::getTooltip()
//-----------------------------------------------------------------------------
QString PowerDomainsItem::getTooltip() const
{
	return tr("Contains the power domains of the component");
}

//-----------------------------------------------------------------------------
// Function: PowerDomainsItem::isValid()
//-----------------------------------------------------------------------------
bool PowerDomainsItem::isValid() const
{
    return true;
}
