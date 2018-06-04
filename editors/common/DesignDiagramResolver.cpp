//-----------------------------------------------------------------------------
// File: DesignDiagramResolver.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 04.05.2016
//
// Description:
// Resolver class containing functionality for calculating expressions within design diagrams.
//-----------------------------------------------------------------------------

#include "DesignDiagramResolver.h"

#include <editors/ComponentEditor/common/IPXactSystemVerilogParser.h>
#include <editors/ComponentEditor/common/ComponentParameterFinder.h>
#include <editors/ComponentEditor/common/ExpressionFormatter.h>
#include <editors/ComponentEditor/common/MultipleParameterFinder.h>
#include <editors/ComponentEditor/common/ListParameterFinder.h>

#include <editors/HWDesign/AdHocItem.h>
#include <editors/HWDesign/HWComponentItem.h>

#include <IPXACTmodels/Component/Component.h>

//-----------------------------------------------------------------------------
// Function: DesignDiagramResolver::DesignDiagramResolver()
//-----------------------------------------------------------------------------
DesignDiagramResolver::DesignDiagramResolver(QSharedPointer<ListParameterFinder> designParameterFinder):
    designParameterFinder_(designParameterFinder),
    designAndInstanceParameterFinder_(new MultipleParameterFinder()),
    expressionParser_(new IPXactSystemVerilogParser(designAndInstanceParameterFinder_)),
    expressionFormatter_(new ExpressionFormatter(designAndInstanceParameterFinder_)),
    valueFormatter_()
{
    designAndInstanceParameterFinder_->addFinder(designParameterFinder);
}

//-----------------------------------------------------------------------------
// Function: DesignDiagramResolver::setContext()
//-----------------------------------------------------------------------------
void DesignDiagramResolver::setContext(QSharedPointer<Component const> component)
{
    designAndInstanceParameterFinder_->removeAllFinders();

    QSharedPointer<ComponentParameterFinder> newParameterFinder(new ComponentParameterFinder(component));
    designAndInstanceParameterFinder_->addFinder(designParameterFinder_);
    designAndInstanceParameterFinder_->addFinder(newParameterFinder);
}

//-----------------------------------------------------------------------------
// Function: DesignDiagramResolver::parse()
//-----------------------------------------------------------------------------
QString DesignDiagramResolver::parseToConstant(QString const& expression) const
{
    if (expression.isEmpty())
    {
        return QString();
    }

    return expressionParser_->parseExpression(expression);
}

//-----------------------------------------------------------------------------
// Function: DesignDiagramResolver::resolveAdhocTieOff()
//-----------------------------------------------------------------------------
void DesignDiagramResolver::resolveAdhocTieOff(QString const& tieOff, AdHocItem* tieOffPort)
{
    if (tieOffPort && !tieOff.isEmpty())
    {
        QSharedPointer<Component> ownerComponent = tieOffPort->getOwnerComponent();
        
        setContext(ownerComponent);
        QString parsedTieOff = getParsedTieOffValue(tieOff, tieOffPort->name(), ownerComponent);

        bool canConvertToInt = true; 
        parsedTieOff.toInt(&canConvertToInt);

        int expressionBase = expressionParser_->baseForExpression(tieOff);
        QString tieOffForBase = valueFormatter_.format(parsedTieOff, expressionBase);
        if (tieOffForBase.isEmpty())
        {
            tieOffForBase = tieOff;
        }

        tieOffPort->changeTieOffLabel(
            expressionFormatter_->formatReferringExpression(tieOff), tieOffForBase, canConvertToInt);
    }
}

//-----------------------------------------------------------------------------
// Function: DesignDiagramResolver::getParsedTieOffValue()
//-----------------------------------------------------------------------------
QString DesignDiagramResolver::getParsedTieOffValue(QString const& tieOffValue, QString const& portName,
    QSharedPointer<Component const> ownerComponent) const
{
    QString parsedTieOff;
    if (QString::compare(tieOffValue, "default", Qt::CaseInsensitive) == 0)
    {
        QSharedPointer<Port> adhocPort = ownerComponent->getPort(portName);
        QString portDefaultValue = adhocPort->getDefaultValue();
        parsedTieOff = expressionParser_->parseExpression(portDefaultValue);
    }
    else if (QString::compare(tieOffValue, "open", Qt::CaseInsensitive) == 0)
    {
        parsedTieOff = "";
    }
    else
    {
        parsedTieOff = expressionParser_->parseExpression(tieOffValue);
    }

    return parsedTieOff;
}
