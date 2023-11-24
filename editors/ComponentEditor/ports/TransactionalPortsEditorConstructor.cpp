//-----------------------------------------------------------------------------
// File: TransactionalPortsEditorConstructor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 29.04.2019
//
// Description:
// Constructs transactional ports editor items.
//-----------------------------------------------------------------------------

#include "TransactionalPortsEditorConstructor.h"

#include <editors/ComponentEditor/ports/PortsView.h>
#include <editors/ComponentEditor/ports/TransactionalPortColumns.h>
#include <editors/ComponentEditor/ports/TransactionalPortsFilter.h>
#include <editors/ComponentEditor/ports/TransactionalPortsDelegate.h>
#include <editors/ComponentEditor/ports/TransactionalPortsModel.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/validators/PortValidator.h>

#include <QCompleter>

//-----------------------------------------------------------------------------
// Function: TransactionalPortsEditorConstructor::constructModel()
//-----------------------------------------------------------------------------
PortsModel* TransactionalPortsEditorConstructor::constructModel(QObject* parent) const
{
    return new TransactionalPortsModel(parameterFinder_, portsInterface_, signalInterface_, parent);;
}

//-----------------------------------------------------------------------------
// Function: TransactionalPortsEditorConstructor::constructFilter()
//-----------------------------------------------------------------------------
PortsFilter* TransactionalPortsEditorConstructor::createFilter(QObject* parent) const
{
    TransactionalPortsFilter* transactionalFilter = new TransactionalPortsFilter(portsInterface_, parent);
    return transactionalFilter;
}

//-----------------------------------------------------------------------------
// Function: TransactionalPortsEditorConstructor::constructView()
//-----------------------------------------------------------------------------
PortsView* TransactionalPortsEditorConstructor::createView(QWidget* parent) const
{
    PortsView* view = new PortsView(TransactionalPortColumns::NAME, busInterface_, parent);

    view->setDefaultImportExportPath(defaultPath_);
    view->setAllowImportExport(true);
    view->setAlternatingRowColors(false);
    view->setSortingEnabled(true);
    view->setItemsDraggable(false);
    view->setFitColumnsToContent(false);

    return view;
}

//-----------------------------------------------------------------------------
// Function: TransactionalPortsEditorConstructor::constructDelegate()
//-----------------------------------------------------------------------------
PortsDelegate* TransactionalPortsEditorConstructor::constructDelegate(QObject* parent) const
{
    return new TransactionalPortsDelegate(
        component_, completionModel_, parameterFinder_, portValidator_->getTypeValidator(), parent);
}
