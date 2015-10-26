//-----------------------------------------------------------------------------
// File: componenteditorbusinterfaceitem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 14.05.2012
//
// Description:
// The item for single bus interface in the component editor's navigation tree.
//-----------------------------------------------------------------------------

#include "componenteditorbusinterfaceitem.h"

#include <library/LibraryManager/libraryinterface.h>

#include <editors/ComponentEditor/busInterfaces/businterfaceeditor.h>
#include <editors/ComponentEditor/common/ExpressionParser.h>

#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/PortMap.h>

//-----------------------------------------------------------------------------
// Function: ComponentEditorBusInterfaceItem::ComponentEditorBusInterfaceItem()
//-----------------------------------------------------------------------------
ComponentEditorBusInterfaceItem::ComponentEditorBusInterfaceItem(QSharedPointer<BusInterface> busif,
    ComponentEditorTreeModel* model, LibraryInterface* libHandler, QSharedPointer<Component> component,
    QSharedPointer<ReferenceCounter> referenceCounter,
    QSharedPointer<ParameterFinder> parameterFinder,
    QSharedPointer<ExpressionFormatter> expressionFormatter,
    QSharedPointer<ExpressionParser> expressionParser,
    ComponentEditorItem* parent, QWidget* parentWnd):
ComponentEditorItem(model, libHandler, component, parent),
    busif_(busif),
    parentWnd_(parentWnd),
    editAction_(new QAction(tr("Edit"), this)),
    expressionParser_(expressionParser)
{
    setParameterFinder(parameterFinder);
    setExpressionFormatter(expressionFormatter);
    setReferenceCounter(referenceCounter);

    connect(editAction_, SIGNAL(triggered(bool)), this, SLOT(openItem()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorBusInterfaceItem::~ComponentEditorBusInterfaceItem()
//-----------------------------------------------------------------------------
ComponentEditorBusInterfaceItem::~ComponentEditorBusInterfaceItem()
{
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorBusInterfaceItem::text()
//-----------------------------------------------------------------------------
QString ComponentEditorBusInterfaceItem::text() const
{
	return busif_->name();
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorBusInterfaceItem::isValid()
//-----------------------------------------------------------------------------
bool ComponentEditorBusInterfaceItem::isValid() const
{
	// check that the bus interface is valid
	/*if (!busif_->isValid(component_->getPortBounds(), component_->getMemoryMapNames(), 
        component_->getAddressSpaceNames(), component_->getChoices()))
    {
		return false;
	}

	// check that the bus definition is found
	else*/ if (!libHandler_->contains(busif_->getBusType()))
    {
		return false;
	}

    // if there is an abstraction definition then check that it is found
    /*else if (busif_->getAbstractionTypes().isValid()) {
        if (!libHandler_->contains(busif_->getAbstractionType())) {
            return false;
        }

        QSharedPointer<Document> libComp = libHandler_->getModel(busif_->getAbstractionType());
        QSharedPointer<AbstractionDefinition> absDef = libComp.dynamicCast<AbstractionDefinition>();

        if (absDef)
        {
            // If the port directions in port map do not match.
            foreach (QSharedPointer<PortMap> portMap, busif_->getPortMaps())
            {
                General::Direction logDir = absDef->getPortDirection(portMap->logicalPort(), busif_->getInterfaceMode());

                General::Direction physDir =  component_->getPortDirection(portMap->physicalPort());
                if (logDir != General::INOUT &&
                    physDir != General::INOUT &&
                    physDir != logDir) 
                {
                    return false;
                }        
            }
        }
    }*/

    /*if (!component_->validateParameters(busif_->getParameters()))
    {
        return false;
    }*/

	return true;
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorBusInterfaceItem::editor()
//-----------------------------------------------------------------------------
ItemEditor* ComponentEditorBusInterfaceItem::editor()
{
	if (!editor_)
    {
		editor_ = new BusInterfaceEditor(libHandler_, component_, busif_, parameterFinder_, expressionFormatter_,
            expressionParser_, 0, parentWnd_);
		editor_->setProtection(locked_);
		connect(editor_, SIGNAL(contentChanged()), this, SLOT(onEditorChanged()), Qt::UniqueConnection);
        connect(editor_, SIGNAL(errorMessage(const QString&)),
            this, SIGNAL(errorMessage(const QString&)),Qt::UniqueConnection);
		connect(editor_, SIGNAL(helpUrlRequested(QString const&)), this, SIGNAL(helpUrlRequested(QString const&)));

        connectItemEditorToReferenceCounter();

        connectReferenceTree();
	}
    setHighlight(false);
	return editor_;
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorBusInterfaceItem::getTooltip()
//-----------------------------------------------------------------------------
QString ComponentEditorBusInterfaceItem::getTooltip() const
{
	return tr("Defines properties of a specific interface in a component");
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorBusInterfaceItem::canBeOpened()
//-----------------------------------------------------------------------------
bool ComponentEditorBusInterfaceItem::canBeOpened() const
{
	// if the bus type is valid vlnv
	if (busif_->getBusType().isValid())
    {
		// if bus definition exists then it can be opened
		return libHandler_->contains(busif_->getBusType());
	}
	else
    {
		return false;
	}
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorBusInterfaceItem::actions()
//-----------------------------------------------------------------------------
QList<QAction*> ComponentEditorBusInterfaceItem::actions() const
{
    QList<QAction*> actionList;
    actionList.append(editAction_);
    
    return actionList;   
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorBusInterfaceItem::openItem()
//-----------------------------------------------------------------------------
void ComponentEditorBusInterfaceItem::openItem() 
{
	VLNV busdefVLNV = busif_->getBusType();
	VLNV absdefVLNV = *busif_->getAbstractionTypes()->first()->getAbstractionRef();
	emit openBus(busdefVLNV, absdefVLNV);
}
