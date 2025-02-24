//-----------------------------------------------------------------------------
// File: portseditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 31.03.2011
//
// Description:
// Editor to edit the ports of a component.
//-----------------------------------------------------------------------------

#include "portseditor.h"

#include <KactusAPI/include/PortAbstractionInterface.h>
#include <KactusAPI/include/BusInterfaceInterface.h>
#include <KactusAPI/include/IPXactSystemVerilogParser.h>
#include <KactusAPI/include/ComponentParameterFinder.h>
#include <KactusAPI/include/PortsInterface.h>

#include <editors/ComponentEditor/parameters/ComponentParameterModel.h>
#include <editors/ComponentEditor/ports/MasterPortsEditor.h>
#include <editors/ComponentEditor/ports/WirePortsEditorFactory.h>
#include <editors/ComponentEditor/ports/TransactionalPortsEditorFactory.h>
#include <editors/ComponentEditor/ports/PortSummaryModel.h>
#include <editors/ComponentEditor/ports/PortsView.h>
#include <editors/ComponentEditor/ports/PortSummaryFilter.h>
#include <editors/ComponentEditor/ports/PortSummaryDelegate.h>

#include "PortSummaryEditorFactory.h"

#include <common/dialogs/NewBusDialog/NewBusDialog.h>
#include <common/widgets/summaryLabel/summarylabel.h>

#include <KactusAPI/include/LibraryInterface.h>

#include <wizards/BusInterfaceWizard/BusInterfaceWizard.h>

#include <IPXACTmodels/AbstractionDefinition/AbstractionDefinition.h>
#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/common/VLNV.h>
#include <IPXACTmodels/Component/validators/PortValidator.h>

#include <QCompleter>
#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: PortsEditor::PortsEditor()
//-----------------------------------------------------------------------------
PortsEditor::PortsEditor(QSharedPointer<Component> component, LibraryInterface* handler,
    ExpressionSet expressions,
    QSharedPointer<PortValidator> portValidator,
    QSharedPointer<PortsInterface> portsInterface,
    BusInterfaceInterface* busInterface,
    QWidget *parent):
ItemEditor(component, handler, parent),
    component_(component),
    handler_(handler),
    portsInterface_(portsInterface),
    busInterface_(busInterface)
{

    PortSummaryEditorFactory factory(component_, 
        expressions, 
        portValidator,
        portsInterface_, 
        QSharedPointer<PortAbstractionInterface>(new PortAbstractionInterface(expressions.parser, expressions.formatter)),
        busInterface,
        QString());

    auto model = factory.createModel(this);
    
    filter_ = factory.createFilter(this);
    
    filter_->setSourceModel(model);

    auto view = factory.createView(this);
    view->setModel(filter_);

    view->setItemDelegate(factory.createDelegate(this));



    connect(model, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(model, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),
        this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(model, SIGNAL(errorMessage(const QString&)),
        this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
    connect(model, SIGNAL(noticeMessage(const QString&)),
        this, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);

    connect(view, SIGNAL(addWire()), model, SLOT(onAddWire()), Qt::UniqueConnection);
    connect(view, SIGNAL(addTransactional()), model, SLOT(onAddTransactional()), Qt::UniqueConnection);
    connect(view, SIGNAL(removeItem(const QModelIndex&)),
        model, SLOT(onRemoveItem(const QModelIndex&)), Qt::UniqueConnection);

    connect(view->itemDelegate(), SIGNAL(increaseReferences(QString)),
        this, SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
    connect(view->itemDelegate(), SIGNAL(decreaseReferences(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);

    connect(model, SIGNAL(decreaseReferences(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);

    //connect(delegate_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);

    connect(view, SIGNAL(changeExtensionsEditorItem(QModelIndex const&)),
        filter_, SLOT(onHandleExtensionsEditorItemChange(QModelIndex const&)), Qt::UniqueConnection);
    connect(filter_, SIGNAL(changeExtensionsEditorItem(QModelIndex const&)),
        this, SLOT(changeExtensionsEditorItem(QModelIndex const&)), Qt::UniqueConnection);

    connect(model, SIGNAL(portExtensionDataChanged(QModelIndex const&)),
        this, SLOT(changeExtensionsEditorItem(QModelIndex const&)), Qt::UniqueConnection);

	auto summaryLabel = new SummaryLabel(tr("Ports summary"), this);

	// create the layout, add widgets to it
	auto layout = new QVBoxLayout(this);
 	layout->addWidget(summaryLabel, 0, Qt::AlignCenter);
    layout->addWidget(view, 1);
	layout->setContentsMargins(0, 0, 0, 0);
}

//-----------------------------------------------------------------------------
// Function: PortsEditor::isValid()
//-----------------------------------------------------------------------------
bool PortsEditor::isValid() const
{
    return true;
}

//-----------------------------------------------------------------------------
// Function: PortsEditor::refresh()
//-----------------------------------------------------------------------------
void PortsEditor::refresh()
{
    filter_->invalidate();
    busInterface_->setBusInterfaces(component_);
}

//-----------------------------------------------------------------------------
// Function: PortsEditor::showEvent()
//-----------------------------------------------------------------------------
void PortsEditor::showEvent(QShowEvent* event)
{
    ItemEditor::showEvent(event);
    emit helpUrlRequested("componenteditor/ports.html");
}

//-----------------------------------------------------------------------------
// Function: PortsEditor::setComponent()
//-----------------------------------------------------------------------------
void PortsEditor::setComponent(QSharedPointer<Component> component)
{
    component_ = component;

    portsInterface_->setPorts(component_->getPorts());

}

//-----------------------------------------------------------------------------
// Function: PortsEditor::changeExtensionsEditorItem()
//-----------------------------------------------------------------------------
void PortsEditor::changeExtensionsEditorItem(QModelIndex const& itemIndex)
{
    if (!itemIndex.isValid())
    {
        emit changeVendorExtensions(QStringLiteral("Component: ") + component()->getVlnv().toString(), component());
    }
    else if (QSharedPointer<Port> selectedPort = 
        portsInterface_->getPort(portsInterface_->getIndexedItemName(itemIndex.row())))
    {
        emit changeVendorExtensions(QStringLiteral("Port: ") + selectedPort->name(), selectedPort);
    }
}
