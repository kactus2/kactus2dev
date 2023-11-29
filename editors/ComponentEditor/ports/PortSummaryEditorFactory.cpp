//-----------------------------------------------------------------------------
// File: PortSummaryEditorFactory.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 28.11.2023
//
// Description:
// Creates the components of port summary editor.
//-----------------------------------------------------------------------------

#include "PortSummaryEditorFactory.h"

#include "PortSummaryView.h"
#include "PortSummaryColumns.h"
#include "PortSummaryFilter.h"
#include "PortSummaryDelegate.h"
#include "PortSummaryModel.h"

#include <editors/ComponentEditor/parameters/ComponentParameterModel.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/validators/PortValidator.h>

#include <QCompleter>

//-----------------------------------------------------------------------------
// Function: PortSummaryEditorFactory::createModel()
//-----------------------------------------------------------------------------
PortsModel* PortSummaryEditorFactory::createModel(QObject* parent) const
{
    return new PortSummaryModel(expressions_.finder, portsInterface_, signalInterface_, parent);
}

//-----------------------------------------------------------------------------
// Function: PortSummaryEditorFactory::constructFilter()
//-----------------------------------------------------------------------------
PortsFilter* PortSummaryEditorFactory::createFilter(QObject* parent) const
{
    return new PortSummaryFilter(portsInterface_, parent);
}

//-----------------------------------------------------------------------------
// Function: PortSummaryEditorFactory::constructView()
//-----------------------------------------------------------------------------
PortsView* PortSummaryEditorFactory::createView(QWidget* parent) const
{
    auto view = new PortSummaryView(PortSummaryColumns::NAME, busInterface_, parent);

    view->setDefaultImportExportPath(defaultPath_);
    view->setAllowImportExport(false);
    view->setAlternatingRowColors(false);
    view->setSortingEnabled(true);
    view->setItemsDraggable(false);
    view->setFitColumnsToContent(false);

    return view;
}

//-----------------------------------------------------------------------------
// Function: PortSummaryEditorFactory::createDelegate()
//-----------------------------------------------------------------------------
PortsDelegate* PortSummaryEditorFactory::createDelegate(QObject* parent) const
{
    auto componentParametersModel = new ComponentParameterModel(expressions_.finder, parent);
    componentParametersModel->setExpressionParser(expressions_.parser);

    return new PortSummaryDelegate(
        component_, componentParametersModel, expressions_.finder, portValidator_->getTypeValidator(), parent);
}
