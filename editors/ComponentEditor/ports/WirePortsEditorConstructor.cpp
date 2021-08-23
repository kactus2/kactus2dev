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

#include <editors/ComponentEditor/common/ParameterCompleter.h>
#include <editors/ComponentEditor/ports/PortsView.h>
#include <editors/ComponentEditor/ports/WirePortsModel.h>
#include <editors/ComponentEditor/ports/WirePortColumns.h>
#include <editors/ComponentEditor/ports/WirePortsFilter.h>
#include <editors/ComponentEditor/ports/WirePortsDelegate.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/validators/PortValidator.h>

#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: WirePortsEditorConstructor::constructModel()
//-----------------------------------------------------------------------------
PortsModel* WirePortsEditorConstructor::constructModel(QSharedPointer<ParameterFinder> parameterFinder,
    QSharedPointer<PortsInterface> portsInterface, QSharedPointer<PortAbstractionInterface> signalInterface,
    QSortFilterProxyModel* filter, QObject* parent) const
{
    WirePortsModel* wireModel =
        new WirePortsModel(parameterFinder, portsInterface, signalInterface, filter, parent);

    return wireModel;
}

//-----------------------------------------------------------------------------
// Function: WirePortsEditorConstructor::constructFilter()
//-----------------------------------------------------------------------------
PortsFilter* WirePortsEditorConstructor::constructFilter(QSharedPointer<PortsInterface> portsInterface,
    QObject* parent) const
{
    WirePortsFilter* wireFilter = new WirePortsFilter(portsInterface, parent);
    return wireFilter;
}

//-----------------------------------------------------------------------------
// Function: WirePortsEditorConstructor::constructView()
//-----------------------------------------------------------------------------
PortsView* WirePortsEditorConstructor::constructView(QString const& defaultPath,
    BusInterfaceInterface* busInterface, QWidget* parent) const
{
    PortsView* view = new PortsView(WirePortColumns::NAME, busInterface, parent);

    view->setDefaultImportExportPath(defaultPath);
    view->setAllowImportExport(true);
    view->setAlternatingRowColors(false);
    view->setSortingEnabled(true);
    view->setItemsDraggable(false);

    return view;
}

//-----------------------------------------------------------------------------
// Function: WirePortsEditorConstructor::constructDelegate()
//-----------------------------------------------------------------------------
PortsDelegate* WirePortsEditorConstructor::constructDelegate(QSharedPointer<Component> component,
    ParameterCompleter* parameterCompleter, QSharedPointer<ParameterFinder> parameterFinder,
    QSharedPointer<PortValidator> portValidator, QObject* parent) const
{
    WirePortsDelegate* wireDelegate = new WirePortsDelegate(
        component, parameterCompleter, parameterFinder, portValidator->getTypeValidator(), parent);

    return wireDelegate;
}
