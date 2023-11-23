//-----------------------------------------------------------------------------
// File: WirePortsEditorConstructor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 29.04.2019
//
// Description:
// Constructs wire ports editor items.
//-----------------------------------------------------------------------------

#include "WirePortsEditorConstructor.h"

#include <editors/ComponentEditor/ports/PortsView.h>
#include <editors/ComponentEditor/ports/WirePortsModel.h>
#include <editors/ComponentEditor/ports/WirePortColumns.h>
#include <editors/ComponentEditor/ports/WirePortsFilter.h>
#include <editors/ComponentEditor/ports/WirePortsDelegate.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/validators/PortValidator.h>

#include <QCompleter>
#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: WirePortsEditorConstructor::constructModel()
//-----------------------------------------------------------------------------
PortsModel* WirePortsEditorConstructor::constructModel(QSharedPointer<ParameterFinder> parameterFinder,
    QSharedPointer<PortsInterface> portsInterface, QSharedPointer<PortAbstractionInterface> signalInterface,
    QSortFilterProxyModel* filter, QObject* parent) const
{
    return new WirePortsModel(parameterFinder, portsInterface, signalInterface, filter, parent);
}

//-----------------------------------------------------------------------------
// Function: WirePortsEditorConstructor::constructFilter()
//-----------------------------------------------------------------------------
PortsFilter* WirePortsEditorConstructor::constructFilter(QSharedPointer<PortsInterface> portsInterface,
    QObject* parent) const
{
    return new WirePortsFilter(portsInterface, parent);
}

//-----------------------------------------------------------------------------
// Function: WirePortsEditorConstructor::constructView()
//-----------------------------------------------------------------------------
PortsView* WirePortsEditorConstructor::constructView(QString const& defaultPath,
    BusInterfaceInterface* busInterface, QWidget* parent) const
{
    auto view = new PortsView(WirePortColumns::NAME, busInterface, parent);

    view->setDefaultImportExportPath(defaultPath);
    view->setAllowImportExport(true);
    view->setAlternatingRowColors(false);
    view->setSortingEnabled(true);
    view->setItemsDraggable(false);
    view->setFitColumnsToContent(false);

    return view;
}

//-----------------------------------------------------------------------------
// Function: WirePortsEditorConstructor::constructDelegate()
//-----------------------------------------------------------------------------
PortsDelegate* WirePortsEditorConstructor::constructDelegate(QSharedPointer<Component> component,
    QAbstractItemModel* completionModel, QSharedPointer<ParameterFinder> parameterFinder,
    QSharedPointer<PortValidator> portValidator, QObject* parent) const
{
    return new WirePortsDelegate(component, completionModel, parameterFinder, portValidator->getTypeValidator(), 
        parent);
}
