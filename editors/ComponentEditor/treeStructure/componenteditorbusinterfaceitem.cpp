//-----------------------------------------------------------------------------
// File: componenteditorbusinterfaceitem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 14.05.2012
//
// Description:
// The item for single bus interface in the component editor's navigation tree.
//-----------------------------------------------------------------------------

#include "componenteditorbusinterfaceitem.h"

#include <KactusAPI/include/LibraryInterface.h>

#include <editors/ComponentEditor/busInterfaces/businterfaceeditor.h>
#include <KactusAPI/include/ExpressionParser.h>

#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/PortMap.h>
#include <IPXACTmodels/Component/validators/BusInterfaceValidator.h>

//-----------------------------------------------------------------------------
// Function: ComponentEditorBusInterfaceItem::ComponentEditorBusInterfaceItem()
//-----------------------------------------------------------------------------
ComponentEditorBusInterfaceItem::ComponentEditorBusInterfaceItem(QSharedPointer<BusInterface> busif,
    ComponentEditorTreeModel* model, LibraryInterface* libHandler, QSharedPointer<Component> component,
    QSharedPointer<ReferenceCounter> referenceCounter, ExpressionSet expressions,
    QSharedPointer<BusInterfaceValidator> validator, BusInterfaceInterface* busInterface,
    PortMapInterface* portMapInterface, ComponentEditorItem* parent, QWidget* parentWnd) :
    ParameterizableItem(model, libHandler, component, parent),
    busIf_(busif),
    parentWnd_(parentWnd),
    editBusDefAction_(new QAction(tr("Edit Bus definition"), this)),
    expressions_(expressions),
    validator_(validator),
    busInterface_(busInterface),
    portMapInterface_(portMapInterface)
{
    setParameterFinder(expressions.finder);
    setExpressionFormatter(expressions.formatter);
    setReferenceCounter(referenceCounter);

    connect(editBusDefAction_, SIGNAL(triggered(bool)), this, SLOT(openBusSlot()), Qt::UniqueConnection);

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
    return busIf_->name();
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorBusInterfaceItem::isValid()
//-----------------------------------------------------------------------------
bool ComponentEditorBusInterfaceItem::isValid() const
{
    if (!validator_->validate(busIf_, component_->getRevision()))
    {
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorBusInterfaceItem::editor()
//-----------------------------------------------------------------------------
ItemEditor* ComponentEditorBusInterfaceItem::editor()
{
    if (!editor_)
    {
        editor_ = new BusInterfaceEditor(libHandler_, component_, busIf_, expressions_,
            busInterface_, portMapInterface_, 0, parentWnd_);
        editor_->setProtection(locked_);
        connect(editor_, SIGNAL(contentChanged()), this, SLOT(onEditorChanged()), Qt::UniqueConnection);
        connect(editor_, SIGNAL(errorMessage(const QString&)),
            this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
        connect(editor_, SIGNAL(helpUrlRequested(QString const&)), this, SIGNAL(helpUrlRequested(QString const&)));

        connectItemEditorToReferenceCounter();

        connectReferenceTree();
    }
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
// Function: ComponentEditorBusInterfaceItem::actions()
//-----------------------------------------------------------------------------
QList<QAction*> ComponentEditorBusInterfaceItem::actions()
{
    QList<QAction*> actionList;
    if (validator_->hasValidBusType(busIf_))
    {
        actionList.append(editBusDefAction_);
    }

    setAbstractionDefinitionActions(actionList);

    return actionList;
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorBusInterfaceItem::openAbsDefSlot()
//-----------------------------------------------------------------------------
bool ComponentEditorBusInterfaceItem::openAbsDefSlot()
{
    // Checking if the function was called by action (from the drop down menu on right 
    // click on bus interface under Bus interface in Component editor tree).
    QAction* action = qobject_cast<QAction*>(QObject::sender());
    if (action && action->data().value<VLNV*>())
    {
        emit openAbsDef(*(action->data().value<VLNV*>()));
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorBusInterfaceItem::openBusDef()
//-----------------------------------------------------------------------------
void ComponentEditorBusInterfaceItem::openBusSlot()
{
    if (validator_->hasValidBusType(busIf_))
    {
        VLNV busdefVLNV = busIf_->getBusType();
        emit openBus(busdefVLNV);
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorBusInterfaceItem::setAbstractionDefinitionActions()
//-----------------------------------------------------------------------------
void ComponentEditorBusInterfaceItem::setAbstractionDefinitionActions(QList<QAction*>& actionList)
{
    QSharedPointer<QList<QSharedPointer<AbstractionType>>> absDefs = busIf_->getAbstractionTypes();
    if (absDefs)
    {
        for (const QSharedPointer<AbstractionType> absDef : *absDefs)
        {
            QSharedPointer<ConfigurableVLNVReference> abstractionRef = absDef->getAbstractionRef();
            VLNV* absDefVLNV = dynamic_cast<VLNV*>(abstractionRef.data());
            if (absDefVLNV && absDefVLNV->isValid())
            {
                QString actionName = "Edit " + absDefVLNV->getName();

                QAction* openAbsDefAction = new QAction(actionName, this);
                //editAbsDef->absRefVLNV = absDefVLNV;
                QVariant v = QVariant::fromValue(absDefVLNV);
                openAbsDefAction->setData(v);
                actionList.append(openAbsDefAction);

                connect(openAbsDefAction, SIGNAL(triggered(bool)), this, SLOT(openAbsDefSlot()), Qt::UniqueConnection);
            }           
        }
    }
}
