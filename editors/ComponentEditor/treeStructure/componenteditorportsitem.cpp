//-----------------------------------------------------------------------------
// File: componenteditorportsitem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 09.05.2012
//
// Description:
// The Ports-item in the component editor's navigation tree.
//-----------------------------------------------------------------------------

#include "componenteditorportsitem.h"

#include <editors/ComponentEditor/ports/portseditor.h>
#include <editors/ComponentEditor/common/ExpressionParser.h>

#include <IPXACTmodels/Component/Component.h>

#include <IPXACTmodels/Component/validators/PortValidator.h>

//-----------------------------------------------------------------------------
// Function: ComponentEditorPortsItem::ComponentEditorPortsItem()
//-----------------------------------------------------------------------------
ComponentEditorPortsItem::ComponentEditorPortsItem(ComponentEditorTreeModel* model, LibraryInterface* libHandler,
    QSharedPointer<Component> component, QSharedPointer<ReferenceCounter> refCounter,
    QSharedPointer<ParameterFinder> parameterFinder, QSharedPointer<ExpressionFormatter> expressionFormatter,
    QSharedPointer<ExpressionParser> expressionParser, ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
portValidator_(new PortValidator(expressionParser, component->getViews()))
{
    setReferenceCounter(refCounter);
    setParameterFinder(parameterFinder);
    setExpressionFormatter(expressionFormatter);
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorPortsItem::~ComponentEditorPortsItem()
//-----------------------------------------------------------------------------
ComponentEditorPortsItem::~ComponentEditorPortsItem()
{
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorPortsItem::getFont()
//-----------------------------------------------------------------------------
QFont ComponentEditorPortsItem::getFont() const
{
    QFont font(ComponentEditorItem::getFont());
    font.setBold(component_->hasPorts());
    return font;
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorPortsItem::text()
//-----------------------------------------------------------------------------
QString ComponentEditorPortsItem::text() const
{
	return tr("Ports");
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorPortsItem::isValid()
//-----------------------------------------------------------------------------
bool ComponentEditorPortsItem::isValid() const
{
    QStringList portNames;
	foreach (QSharedPointer<Port> port, *component_->getPorts()) 
    {
        if (portNames.contains(port->name()) || !portValidator_->validate(port))
        {
			return false;
		}

        portNames.append(port->name());
	}

	return true;
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorPortsItem::editor()
//-----------------------------------------------------------------------------
ItemEditor* ComponentEditorPortsItem::editor()
{
	if (!editor_)
    {
		editor_ = new PortsEditor(component_, libHandler_, parameterFinder_, expressionFormatter_, portValidator_);
		editor_->setProtection(locked_);

		connect(editor_, SIGNAL(contentChanged()), this, SLOT(onEditorChanged()), Qt::UniqueConnection);
		connect(editor_, SIGNAL(helpUrlRequested(QString const&)),
			this, SIGNAL(helpUrlRequested(QString const&)), Qt::UniqueConnection);
        connect(editor_, SIGNAL(createInterface()), 
            this, SIGNAL(createInterface()), Qt::UniqueConnection);

        connectItemEditorToReferenceCounter();
	}
	return editor_;
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorPortsItem::getTooltip()
//-----------------------------------------------------------------------------
QString ComponentEditorPortsItem::getTooltip() const
{
	return tr("Contains the external ports of the component");
}
