//-----------------------------------------------------------------------------
// File: componenteditorportsitem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 09.05.2012
//
// Description:
// The Ports-item in the component editor's navigation tree.
//-----------------------------------------------------------------------------

#include "componenteditorportsitem.h"
#include "WirePortsItem.h"
#include "TransactionalPortsItem.h"

#include <editors/ComponentEditor/ports/portseditor.h>

#include <KactusAPI/include/ExpressionParser.h>
#include <KactusAPI/include/PortsInterface.h>

#include <IPXACTmodels/Component/Component.h>

#include <IPXACTmodels/Component/validators/PortValidator.h>
#include <IPXACTmodels/Component/validators/CollectionValidators.h>

//-----------------------------------------------------------------------------
// Function: ComponentEditorPortsItem::ComponentEditorPortsItem()
//-----------------------------------------------------------------------------
ComponentEditorPortsItem::ComponentEditorPortsItem(ComponentEditorTreeModel* model, LibraryInterface* libHandler,
    QSharedPointer<Component> component, QSharedPointer<ReferenceCounter> refCounter,
    ExpressionSet expressions,
    BusInterfaceInterface* busInterface,
    ComponentEditorItem* parent) :
    ComponentEditorItem(model, libHandler, component, parent),
    expressions_(expressions),
    portValidator_(new PortValidator(expressions.parser, component->getViews())),
    portsValidator_(new PortsValidator(portValidator_)),
    portsInterface_(new PortsInterface(portValidator_, expressions.parser, expressions.formatter)),
    busInterface_(busInterface)
{
    setReferenceCounter(refCounter);
    setParameterFinder(expressions.finder);
    setExpressionFormatter(expressions.formatter);

    portsInterface_->setPorts(component->getPorts());

    auto wiresItem = QSharedPointer<WirePortsItem>(new WirePortsItem(model,
        libHandler, component, refCounter, expressions, portsInterface_, busInterface, this));
    childItems_.append(wiresItem);

    auto transactionalItem = QSharedPointer<TransactionalPortsItem>(new TransactionalPortsItem(model,
        libHandler, component, refCounter, expressions, portsInterface_, busInterface, this));
    childItems_.append(transactionalItem);

    connect(wiresItem.data(), SIGNAL(changeVendorExtensions(QString const&, QSharedPointer<Extendable>)),
        this, SIGNAL(changeVendorExtensions(QString const&, QSharedPointer<Extendable>)),
        Qt::UniqueConnection);
    connect(wiresItem.data(), SIGNAL(createInterface()), this, SIGNAL(createInterface()), Qt::UniqueConnection);

    connect(transactionalItem.data(), SIGNAL(changeVendorExtensions(QString const&, QSharedPointer<Extendable>)),
        this, SIGNAL(changeVendorExtensions(QString const&, QSharedPointer<Extendable>)),
        Qt::UniqueConnection);
    connect(transactionalItem.data(), SIGNAL(createInterface()), this, SIGNAL(createInterface()), Qt::UniqueConnection);
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
    auto portsAsNameGroup = CollectionValidators::itemListToNameGroupList(component_->getPorts());
    portsValidator_->childrenHaveUniqueNames(portsAsNameGroup);
    return ComponentEditorItem::isValid();
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorPortsItem::editor()
//-----------------------------------------------------------------------------
ItemEditor* ComponentEditorPortsItem::editor()
{
	if (!editor_)
    {
		editor_ = new PortsEditor(
            component_, libHandler_, expressions_, portValidator_, portsInterface_, busInterface_);
		editor_->setProtection(locked_);

		connect(editor_, SIGNAL(contentChanged()), this, SLOT(onEditorChanged()), Qt::UniqueConnection);
		connect(editor_, SIGNAL(helpUrlRequested(QString const&)),
			this, SIGNAL(helpUrlRequested(QString const&)), Qt::UniqueConnection);

        connectItemEditorToReferenceCounter();
        connectItemEditorToVendorExtensionsEditor();
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
