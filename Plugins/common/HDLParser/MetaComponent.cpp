//-----------------------------------------------------------------------------
// File: MetaComponent.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Janne Virtanen
// Date: 03.02.2017
//
// Description:
// Class used to parse relevant information from IP-XACT component for HDL generation.
//-----------------------------------------------------------------------------

#include "MetaComponent.h"

#include <IPXACTmodels/Component/RemapState.h>
#include <IPXACTmodels/Component/RemapPort.h>

#include <editors/ComponentEditor/common/ComponentParameterFinder.h>
#include <editors/ComponentEditor/common/ExpressionFormatter.h>

//-----------------------------------------------------------------------------
// Function: MetaComponent::MetaComponent
//-----------------------------------------------------------------------------
MetaComponent::MetaComponent(MessagePasser* messages,
    QSharedPointer<Component> component,
    QSharedPointer<View> activeView) :
    messages_(messages),
    component_(component),
    activeView_(activeView),
    parameters_(new QList<QSharedPointer<Parameter> >()),
    ports_(new QMap<QString,QSharedPointer<MetaPort> >()),
    remapStates_(new QList<QSharedPointer<FormattedRemapState> >()),
    fileSets_(new QList<QSharedPointer<FileSet> >())
{
    // Try to find a component instantiation for the view.
    if (activeView_)
    {
        activeInstantiation_ = component_->getModel()->
            findComponentInstantiation(activeView_->getComponentInstantiationRef());

        if (activeInstantiation_)
        {
            // If there is a named component instantiation, its module name shall be used.
            moduleName_ = activeInstantiation_->getModuleName();
            parsesFileSets();
        }
    }

    if (moduleName_.isEmpty())
    {
        // No module name. -> Take the name from the VLNV of the component.
        moduleName_ = component_->getVlnv().getName();
    }

    // Must cull the parameters before can use them!
    cullParameters();
}

//-----------------------------------------------------------------------------
// Function: MetaComponent::~MetaComponent
//-----------------------------------------------------------------------------
MetaComponent::~MetaComponent()
{
}

//-----------------------------------------------------------------------------
// Function: MetaComponent::parsesFileSets()
//-----------------------------------------------------------------------------
void MetaComponent::parsesFileSets()
{
    foreach(QString fileSetRef, *activeInstantiation_->getFileSetReferences())
    {
        QSharedPointer<FileSet> fileSet = component_->getFileSet(fileSetRef);

        if (fileSet)
        {
            fileSets_->append(fileSet);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MetaComponent::sortModuleParameters()
//-----------------------------------------------------------------------------
void MetaComponent::sortParameters(QSharedPointer<QList<QSharedPointer<Parameter> > > sortParameters)
{
    // Create reference parameters.
    QList<QSharedPointer<Parameter> > refParameters;
    refParameters.append(*sortParameters.data());

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
// Function: MetaComponent::cullParameters()
//-----------------------------------------------------------------------------
void MetaComponent::cullParameters()
{
    // Cull all the component parameters for the original parameters.
    foreach(QSharedPointer<Parameter> parameterOrig, *component_->getParameters())
    {
        QSharedPointer<Parameter> parameterCpy(new Parameter(*parameterOrig));
        parameters_->append(parameterCpy);
    }

    // If there is an active component instantiation, take its module parameters as well.
    if (activeInstantiation_)
    {
        foreach(QSharedPointer<ModuleParameter> parameterOrig, *activeInstantiation_->getModuleParameters())
        {
            QSharedPointer<Parameter> parameterCpy(new Parameter(*parameterOrig));
            parameters_->append(parameterCpy);
        }
    }
}

//-----------------------------------------------------------------------------
// Function:  MetaComponent::formatComponent()
//-----------------------------------------------------------------------------
void MetaComponent::formatComponent()
{
    // Initialize the parameter finder and formatter.
    QSharedPointer<ComponentParameterFinder> parameterFinder(new ComponentParameterFinder(component_));
    //! The formatter for expressions.
    QSharedPointer<ExpressionFormatter> formatter = 
        QSharedPointer<ExpressionFormatter>(new ExpressionFormatter(parameterFinder));

    // Format parameters and ports.
    formatParameters(formatter);
    formatPorts(formatter);

    // Find and parse the component stuff that (currently) does not exists in the hierarchy parsing.
    parseRemapStates(formatter);
}

//-----------------------------------------------------------------------------
// Function: MetaComponent::formatParameters()
//-----------------------------------------------------------------------------
void MetaComponent::formatParameters(QSharedPointer<ExpressionFormatter> formatter)
{
    // Sort the parameters.
    sortParameters(parameters_);

    // Format the parameters.
    foreach(QSharedPointer<Parameter> parameter, *parameters_)
    {
        parameter->setValue(formatter->formatReferringExpression(parameter->getValue()));
    }
}

//-----------------------------------------------------------------------------
// Function: MetaComponent::formatPorts()
//-----------------------------------------------------------------------------
void MetaComponent::formatPorts(QSharedPointer<ExpressionFormatter> formatter)
{
    foreach (QSharedPointer<Port> cport, *component_->getPorts())
    {
        // Create generation port.
        QSharedPointer<MetaPort> mPort(new MetaPort);

        // Needs a reference to the IP-XACT port.
        mPort->port_ = cport;

        // Both vector and array bounds may be needed.
        mPort->arrayBounds_.first = formatter->formatReferringExpression(cport->getArrayLeft());
        mPort->arrayBounds_.second = formatter->formatReferringExpression(cport->getArrayRight());

        mPort->vectorBounds_.first = formatter->formatReferringExpression(cport->getLeftBound());
        mPort->vectorBounds_.second = formatter->formatReferringExpression(cport->getRightBound());

        // Add to the list.
        ports_->insert(cport->name(), mPort);
    }
}

//-----------------------------------------------------------------------------
// Function: MetaComponent::parseRemapStates
//-----------------------------------------------------------------------------
void MetaComponent::parseRemapStates(QSharedPointer<ExpressionFormatter> formatter)
{
    // Go through configured remap states.
    foreach(QSharedPointer<RemapState> currentState, *component_->getRemapStates())
    {
        // Create new remap state for the generation.
        QSharedPointer<FormattedRemapState> grms(new FormattedRemapState);
        grms->state_ = currentState;
        remapStates_->append(grms);

        // Each port referred by the state must be listed.
        foreach(QSharedPointer<RemapPort> rmport, *currentState->getRemapPorts())
        {
            QSharedPointer<QPair<QSharedPointer<Port>,QString> > parsedPort
                (new QPair<QSharedPointer<Port>,QString>);

            // Pick the port name, and the value needed for it to remap state become effective.
            parsedPort->first = component_->getPort(rmport->getPortNameRef());
            parsedPort->second = formatter->formatReferringExpression(rmport->getValue());

            grms->ports_.append(parsedPort);
        }
    }
}