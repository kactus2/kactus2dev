//-----------------------------------------------------------------------------
// File: WirePortsItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 22.11.2023
//
// Description:
// The Wire ports-item in the component editor's navigation tree. 
//-----------------------------------------------------------------------------


#include "WirePortsItem.h"

#include <editors/ComponentEditor/ports/WirePortEditor.h>
#include <KactusAPI/include/ExpressionParser.h>

#include <IPXACTmodels/Component/Component.h>

#include <IPXACTmodels/Component/validators/PortValidator.h>

//-----------------------------------------------------------------------------
// Function: WirePortsItem::WirePortsItem()
//-----------------------------------------------------------------------------
WirePortsItem::WirePortsItem(ComponentEditorTreeModel* model, LibraryInterface* libHandler,
    QSharedPointer<Component> component, QSharedPointer<ReferenceCounter> refCounter,
    QSharedPointer<ParameterFinder> parameterFinder, QSharedPointer<ExpressionFormatter> expressionFormatter,
    QSharedPointer<ExpressionParser> expressionParser, BusInterfaceInterface* busInterface,
    ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
portValidator_(new PortValidator(expressionParser, component->getViews())),
busInterface_(busInterface)
{
    setReferenceCounter(refCounter);
    setParameterFinder(parameterFinder);
    setExpressionFormatter(expressionFormatter);
}

//-----------------------------------------------------------------------------
// Function: WirePortsItem::text()
//-----------------------------------------------------------------------------
QString WirePortsItem::text() const
{
	return tr("Wires");
}

//-----------------------------------------------------------------------------
// Function: WirePortsItem::isValid()
//-----------------------------------------------------------------------------
bool WirePortsItem::isValid() const
{
    QStringList portNames;
	for (QSharedPointer<Port> port : *component_->getPorts()) 
    {
        if (port->getWire() && (portNames.contains(port->name()) || !portValidator_->validate(port)))
        {
			return false;
		}

        portNames.append(port->name());
	}

	return true;
}

//-----------------------------------------------------------------------------
// Function: WirePortsItem::editor()
//-----------------------------------------------------------------------------
ItemEditor* WirePortsItem::editor()
{
	if (!editor_)
    {
		editor_ = new WirePortEditor(
            component_, libHandler_, parameterFinder_, expressionFormatter_, portValidator_, busInterface_);
		editor_->setProtection(locked_);

		connect(editor_, SIGNAL(contentChanged()), this, SLOT(onEditorChanged()), Qt::UniqueConnection);
		connect(editor_, SIGNAL(helpUrlRequested(QString const&)),
			this, SIGNAL(helpUrlRequested(QString const&)), Qt::UniqueConnection);
        connect(editor_, SIGNAL(createInterface()), 
            this, SIGNAL(createInterface()), Qt::UniqueConnection);

        connectItemEditorToReferenceCounter();
        connectItemEditorToVendorExtensionsEditor();
	}
	return editor_;
}

//-----------------------------------------------------------------------------
// Function: WirePortsItem::getTooltip()
//-----------------------------------------------------------------------------
QString WirePortsItem::getTooltip() const
{
	return tr("Contains the wire ports of the component");
}
