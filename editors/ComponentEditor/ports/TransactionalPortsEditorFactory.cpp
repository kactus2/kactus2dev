//-----------------------------------------------------------------------------
// File: TransactionalPortsEditorFactory.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 29.04.2019
//
// Description:
// Constructs transactional ports editor items.
//-----------------------------------------------------------------------------

#include "TransactionalPortsEditorFactory.h"

#include <editors/ComponentEditor/ports/TypedPortsView.h>
#include <editors/ComponentEditor/ports/TransactionalPortColumns.h>
#include <editors/ComponentEditor/ports/TransactionalPortsFilter.h>
#include <editors/ComponentEditor/ports/TransactionalPortsDelegate.h>
#include <editors/ComponentEditor/ports/TransactionalPortsModel.h>
#include <editors/ComponentEditor/parameters/ComponentParameterModel.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/validators/PortValidator.h>

#include <QCompleter>

//-----------------------------------------------------------------------------
// Function: TransactionalPortsEditorFactory::constructModel()
//-----------------------------------------------------------------------------
PortsModel* TransactionalPortsEditorFactory::createModel(QObject* parent) const
{
    return new TransactionalPortsModel(expressions_.finder, portsInterface_, signalInterface_, parent);
}

//-----------------------------------------------------------------------------
// Function: TransactionalPortsEditorFactory::constructFilter()
//-----------------------------------------------------------------------------
PortsFilter* TransactionalPortsEditorFactory::createFilter(QObject* parent) const
{
    return new TransactionalPortsFilter(portsInterface_, parent);
}

//-----------------------------------------------------------------------------
// Function: TransactionalPortsEditorFactory::constructView()
//-----------------------------------------------------------------------------
PortsView* TransactionalPortsEditorFactory::createView(QWidget* parent) const
{
    auto view = new TypedPortsView(TransactionalPortColumns::TYPE_NAME, TransactionalPortColumns::NAME, 
        busInterface_, parent);

    view->setDefaultImportExportPath(defaultPath_);
    view->setAllowImportExport(true);
    view->setAlternatingRowColors(false);
    view->setSortingEnabled(true);
    view->setItemsDraggable(false);
    view->setFitColumnsToContent(false);

    return view;
}

//-----------------------------------------------------------------------------
// Function: TransactionalPortsEditorFactory::createDelegate()
//-----------------------------------------------------------------------------
PortsDelegate* TransactionalPortsEditorFactory::createDelegate(QObject* parent) const
{
    auto componentParametersModel = new ComponentParameterModel(expressions_.finder, parent);
    componentParametersModel->setExpressionParser(expressions_.parser);

    return new TransactionalPortsDelegate(
        component_, componentParametersModel, expressions_.finder, portValidator_->getTypeValidator(), parent);
}
