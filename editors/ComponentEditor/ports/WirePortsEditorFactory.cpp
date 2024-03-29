//-----------------------------------------------------------------------------
// File: WirePortsEditorFactory.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 29.04.2019
//
// Description:
// Constructs wire ports editor items.
//-----------------------------------------------------------------------------

#include "WirePortsEditorFactory.h"

#include <editors/ComponentEditor/ports/TypedPortsView.h>
#include <editors/ComponentEditor/ports/WirePortsModel.h>
#include <editors/ComponentEditor/ports/WirePortColumns.h>
#include <editors/ComponentEditor/ports/WirePortsFilter.h>
#include <editors/ComponentEditor/ports/WirePortsDelegate.h>
#include <editors/ComponentEditor/parameters/ComponentParameterModel.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/validators/PortValidator.h>

#include <QCompleter>
#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: WirePortsEditorFactory::constructModel()
//-----------------------------------------------------------------------------
PortsModel* WirePortsEditorFactory::createModel(QObject* parent) const
{
    return new WirePortsModel(expressions_.finder, portsInterface_, signalInterface_, parent);
}

//-----------------------------------------------------------------------------
// Function: WirePortsEditorFactory::constructFilter()
//-----------------------------------------------------------------------------
PortsFilter* WirePortsEditorFactory::createFilter(QObject* parent) const
{
    return new WirePortsFilter(portsInterface_, parent);
}

//-----------------------------------------------------------------------------
// Function: WirePortsEditorFactory::constructView()
//-----------------------------------------------------------------------------
PortsView* WirePortsEditorFactory::createView(QWidget* parent) const
{
    auto view = new TypedPortsView(WirePortColumns::TYPE_NAME, WirePortColumns::NAME, busInterface_, parent);

    view->setDefaultImportExportPath(defaultPath_);
    view->setAllowImportExport(true);
    view->setAlternatingRowColors(false);
    view->setSortingEnabled(true);
    view->setItemsDraggable(false);
    view->setFitColumnsToContent(false);

    return view;
}

//-----------------------------------------------------------------------------
// Function: WirePortsEditorFactory::createDelegate()
//-----------------------------------------------------------------------------
PortsDelegate* WirePortsEditorFactory::createDelegate(QObject* parent) const
{
    auto componentParametersModel = new ComponentParameterModel(expressions_.finder, parent);
    componentParametersModel->setExpressionParser(expressions_.parser);

    return new WirePortsDelegate(component_, componentParametersModel, expressions_.finder, 
        portValidator_->getTypeValidator(), parent);
}
