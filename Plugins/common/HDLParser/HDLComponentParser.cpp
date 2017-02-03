//-----------------------------------------------------------------------------
// File: HDLComponentParser.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Janne Virtanen
// Date: 11.08.2016
//
// Description:
// Class used to parse relevant information from IP-XACT component for HDL generation.
//-----------------------------------------------------------------------------

#include "HDLComponentParser.h"

#include <library/LibraryManager/libraryinterface.h>

#include <IPXACTmodels/AbstractionDefinition/AbstractionDefinition.h>

#include <IPXACTmodels/common/ModuleParameter.h>
#include <IPXACTmodels/common/VLNV.h>
#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/Port.h>
#include <IPXACTmodels/Component/View.h>
#include <IPXACTmodels/Component/ComponentInstantiation.h>

#include <IPXACTmodels/Component/MemoryMap.h>
#include <IPXACTmodels/Component/MemoryRemap.h>
#include <IPXACTmodels/Component/MemoryBlockBase.h>
#include <IPXACTmodels/Component/AddressBlock.h>
#include <IPXACTmodels/Component/Register.h>
#include <IPXACTmodels/Component/Field.h>
#include <IPXACTmodels/Component/RemapState.h>
#include <IPXACTmodels/Component/RemapPort.h>

#include "editors/ComponentEditor/common/ComponentParameterFinder.h"
#include "editors/ComponentEditor/common/ExpressionFormatter.h"
#include <editors/ComponentEditor/common/ListParameterFinder.h>

//-----------------------------------------------------------------------------
// Function: HDLComponentParser::HDLComponentParser
//-----------------------------------------------------------------------------
HDLComponentParser::HDLComponentParser(LibraryInterface* library,
    MessagePasser* messages,
    QSharedPointer<Component> component,
    QSharedPointer<View> activeView) :
    MetaInstance(library,
    messages,
    component,
    activeView,
    QSharedPointer<ComponentInstance>::QSharedPointer(),
    QSharedPointer<ListParameterFinder>::QSharedPointer(),
    QSharedPointer<QList<QSharedPointer<ConfigurableElementValue> > >::QSharedPointer())
{
    // Initialize the parameter finder and formatter.
    QSharedPointer<ComponentParameterFinder> parameterFinder(new ComponentParameterFinder(component_));
    parameterFinder->setActiveView(activeView_);
    formatter_ = QSharedPointer<ExpressionFormatter>(new ExpressionFormatter(parameterFinder));

    // Cull again the parameters.
    parameters_->clear();
    cullParameters();
    // Format parameters and ports.
    formatParameters();
    formatPorts();

    // Find and parse the component stuff that (currently) does not exists in the hierarchy parsing.
    parseRemapStates();
}

//-----------------------------------------------------------------------------
// Function: HDLComponentParser::~HDLComponentParser
//-----------------------------------------------------------------------------
HDLComponentParser::~HDLComponentParser()
{
}

//-----------------------------------------------------------------------------
// Function: HDLComponentParser::formatParameters()
//-----------------------------------------------------------------------------
void HDLComponentParser::formatParameters()
{
    // Create reference parameters.
    QList<QSharedPointer<Parameter> > refParameters;
    refParameters.append(*parameters_.data());

    // Sort the parameters.
    sortParameters(refParameters, parameters_);

    // Format the parameters.
    foreach(QSharedPointer<Parameter> parameter, *parameters_)
    {
        parameter->setValue(formatter_->formatReferringExpression(parameter->getValue()));
    }
}

//-----------------------------------------------------------------------------
// Function: HDLComponentParser::formatPorts()
//-----------------------------------------------------------------------------
void HDLComponentParser::formatPorts()
{
    foreach (QSharedPointer<Port> cport, *component_->getPorts())
    {
        // Create generation port.
        QSharedPointer<MetaPort> mPort(new MetaPort);

        // Needs a reference to the IP-XACT port.
        mPort->port_ = cport;

        // Both vector and array bounds may be needed.
        mPort->arrayBounds_.first = formatter_->formatReferringExpression(cport->getArrayLeft());
        mPort->arrayBounds_.second = formatter_->formatReferringExpression(cport->getArrayRight());

        mPort->vectorBounds_.first = formatter_->formatReferringExpression(cport->getLeftBound());
        mPort->vectorBounds_.second = formatter_->formatReferringExpression(cport->getRightBound());

        // Add to the list.
        ports_->insert(cport->name(), mPort);
    }
}

//-----------------------------------------------------------------------------
// Function: HDLComponentParser::sortModuleParameters()
//-----------------------------------------------------------------------------
void HDLComponentParser::sortParameters(QList<QSharedPointer<Parameter> >& refParameters,
    QSharedPointer<QList<QSharedPointer<Parameter> > > sortParameters )
{
    // Go through existing ones on the instance.
    for (QList<QSharedPointer<Parameter> >::Iterator parameterAdd =
        refParameters.begin(); parameterAdd != refParameters.end(); ++parameterAdd)
    {
        // The first position for the second pass.
        QList<QSharedPointer<Parameter> >::Iterator minPos =
            sortParameters->begin();

        // The value of the inspected parameter.
        QString valueAdd = (*parameterAdd)->getValue();

        // First pass: Detect if the parameter depends on another parameter.
        for (QList<QSharedPointer<Parameter> >::Iterator parameterCmp =
            sortParameters->begin();
            parameterCmp != sortParameters->end(); ++parameterCmp)
        {
            if (valueAdd.contains((*parameterCmp)->getValueId()))
            {
                // A match found: The parameter must be positioned after this one!
                minPos = ++parameterCmp;

                // The first one needed is the relevant one, so break here.
                break;
            }
        }

        // If true, the parameter will be appended to the end of the list.
        bool append = true;

        // The second pass: Find the actual position before the parameter is referred.
        for (QList<QSharedPointer<Parameter> >::Iterator parameterCmp = minPos;
            parameterCmp != sortParameters->end(); ++parameterCmp)
        {
            // The value of the the compared parameter.
            QString valueCmp = (*parameterCmp)->getValue();

            // Check if it contains a reference to the inspected parameter.
            if (valueCmp.contains((*parameterAdd)->getValueId()))
            {
                // Take a direct shared pointer to the referring parameter.
                QSharedPointer<Parameter> parameterRef = *parameterCmp;

                // Remove the inspected parameter from the previous place.
                sortParameters->removeOne(*parameterAdd);

                // See where the referring parameter is located now.
                int cmpLoc = sortParameters->indexOf(*parameterCmp);

                // Then the inspected parameter comes before it is referred.
                sortParameters->insert(cmpLoc, *parameterAdd);

                // It will not be inserted twice, so break here.
                append = false;
                break;
            }
        }

        // If there was no match in the second pass, or no second pass at all, append to the end.
        if (append)
        {
            // Remove the inspected parameter from the previous place.
            sortParameters->removeOne(*parameterAdd);
            // Append at the end of the list.
            sortParameters->append(*parameterAdd);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: HDLComponentParser::parseRemapStates
//-----------------------------------------------------------------------------
void HDLComponentParser::parseRemapStates()
{
    // Go through configured remap states.
    foreach(QSharedPointer<RemapState> currentState, *component_->getRemapStates())
    {
        // Create new remap state for the generation.
        QSharedPointer<FormattedRemapState> grms(new FormattedRemapState);
        grms->state_ = currentState;
        remapStates_.append(grms);

        // Each port referred by the state must be listed.
        foreach(QSharedPointer<RemapPort> rmport, *currentState->getRemapPorts())
        {
            QSharedPointer<QPair<QSharedPointer<Port>,QString> > parsedPort(new QPair<QSharedPointer<Port>,QString>);

            // Pick the port name, and the value needed for it to remap state become effective.
            parsedPort->first = component_->getPort(rmport->getPortNameRef());
            parsedPort->second = formatter_->formatReferringExpression(rmport->getValue());

            grms->ports_.append(parsedPort);
        }
    }
}