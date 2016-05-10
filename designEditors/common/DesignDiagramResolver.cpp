//-----------------------------------------------------------------------------
// File: DesignDiagramResolver.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 04.05.2016
//
// Description:
// Resolver class containing functionality for calculating expressions within design diagrams.
//-----------------------------------------------------------------------------

#include "DesignDiagramResolver.h"

#include <editors/ComponentEditor/common/IPXactSystemVerilogParser.h>
#include <editors/ComponentEditor/common/ComponentParameterFinder.h>

#include <designEditors/HWDesign/AdHocItem.h>
#include <designEditors/HWDesign/HWComponentItem.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Design/AdHocConnection.h>

//-----------------------------------------------------------------------------
// Function: DesignDiagramResolver::DesignDiagramResolver()
//-----------------------------------------------------------------------------
DesignDiagramResolver::DesignDiagramResolver():
componentFinder_(new ComponentParameterFinder(QSharedPointer<Component>(0))),
expressionParser_(new IPXactSystemVerilogParser(componentFinder_))
{

}

//-----------------------------------------------------------------------------
// Function: DesignDiagramResolver::~DesignDiagramResolver()
//-----------------------------------------------------------------------------
DesignDiagramResolver::~DesignDiagramResolver()
{

}

//-----------------------------------------------------------------------------
// Function: DesignDiagramResolver::resolveAdhocTieOff()
//-----------------------------------------------------------------------------
void DesignDiagramResolver::resolveAdhocTieOff(QString const& tieOff, AdHocItem* tieOffPort)
{
    if (tieOffPort && !tieOff.isEmpty())
    {
        QSharedPointer<Component> ownerComponent = tieOffPort->getOwnerComponent();
        componentFinder_->setComponent(ownerComponent);

        QString parsedTieOff = expressionParser_->parseExpression(tieOff);

        bool canConvertToInt = true; 
        int tieOffInInt = parsedTieOff.toInt(&canConvertToInt);

        if (!canConvertToInt)
        {
            tieOffPort->createNonResolvableTieOff();
        }
        else if (tieOffInInt == 1)
        {
            tieOffPort->createHighTieOff();
        }
        else if (tieOffInInt == 0)
        {
            tieOffPort->createLowTieOff();
        }
        else
        {
            tieOffPort->createNumberedTieOff();
        }
    }
}
