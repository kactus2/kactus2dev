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

#include <KactusAPI/include/MessageMediator.h>

#include <IPXACTmodels/utilities/Search.h>

#include <IPXACTmodels/Component/RemapState.h>
#include <IPXACTmodels/Component/RemapPort.h>

#include <KactusAPI/include/ComponentParameterFinder.h>
#include <KactusAPI/include/MultipleParameterFinder.h>
#include <KactusAPI/include/ListParameterFinder.h>
#include <KactusAPI/include/ExpressionFormatter.h>

//-----------------------------------------------------------------------------
// Function: MetaComponent::MetaComponent
//-----------------------------------------------------------------------------
MetaComponent::MetaComponent(MessageMediator* messages,
    QSharedPointer<Component> component,
    QSharedPointer<View> activeView) :
    messages_(messages),
    component_(component),
    activeView_(activeView),
    parameters_(new QList<QSharedPointer<Parameter> >()),
    moduleParameters_(new QList<QSharedPointer<Parameter> >()),
    metaParameters_(new QMap<QString,QSharedPointer<Parameter> >()),
    ports_(new QMap<QString,QSharedPointer<MetaPort> >()),
    fileSets_(new QList<QSharedPointer<FileSet> >()),
    moduleName_(),
    activeInstantiation_(),
    remapStates_(new QList<QSharedPointer<FormattedRemapState> >())
{
    // Try to find a component instantiation for the view.
    if (activeView_)
    {
        activeInstantiation_ = Search::findByName(activeView_->getComponentInstantiationRef(),
            *component_->getComponentInstantiations());

        if (activeInstantiation_)
        {
            // If there is a named component instantiation, its module name shall be used.
            moduleName_ = activeInstantiation_->getModuleName();
            parsesFileSets();
        }
    }

    if (moduleName_.isEmpty())
    {
        // If no module name is set, take the name from the VLNV of the component.
        moduleName_ = component_->getVlnv().getName();
    }

    // Must parse the parameters before can use them!
    parseParameters();
}

//-----------------------------------------------------------------------------
// Function:  MetaComponent::formatComponent()
//-----------------------------------------------------------------------------
void MetaComponent::formatComponent()
{
    // Initialize the parameter finders.
    QSharedPointer<MultipleParameterFinder> parameterFinder(new MultipleParameterFinder());
    QSharedPointer<ComponentParameterFinder> componentFinder(new ComponentParameterFinder(component_));
    QSharedPointer<ListParameterFinder> moduleFinder(new ListParameterFinder());
    moduleFinder->setParameterList(getModuleParameters());
    parameterFinder->addFinder(componentFinder);
    parameterFinder->addFinder(moduleFinder);

    //! The formatter for expressions.
    ExpressionFormatter formatter(parameterFinder);

    formatParameters(formatter);
    formatPorts(formatter);
    parseRemapStates(formatter);

    parseMetaParameters();
}

//-----------------------------------------------------------------------------
// Function: MetaComponent::parseMetaParameters()
//-----------------------------------------------------------------------------
void MetaComponent::parseMetaParameters()
{
    for (QSharedPointer<Parameter> original : *getParameters())
    {
        metaParameters_->insert(original->name(), original);
    }

    for (QSharedPointer<Parameter> pOriginal : *getModuleParameters())
    {
        QSharedPointer<ModuleParameter> original = pOriginal.dynamicCast<ModuleParameter>();
        if (original)
        {
            auto i = metaParameters_->find(original->name());
            if (i != metaParameters_->end())
            {
                metaParameters_->erase(i);
            }
            
            metaParameters_->insert(original->name(), original);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MetaComponent::parseParameters()
//-----------------------------------------------------------------------------
void MetaComponent::parseParameters()
{
    // Copy all the component parameters for the original parameters.
    for (QSharedPointer<Parameter> parameterOrig : *component_->getParameters())
    {       
        parameters_->append(QSharedPointer<Parameter>(new Parameter(*parameterOrig)));
    }

    // If there is an active component instantiation, take its module parameters as well.
    if (activeInstantiation_)
    {
        for (QSharedPointer<ModuleParameter> parameterOrig : *activeInstantiation_->getModuleParameters())
        {
            moduleParameters_->append(QSharedPointer<ModuleParameter>(new ModuleParameter(*parameterOrig)));
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MetaComponent::formatParameters()
//-----------------------------------------------------------------------------
void MetaComponent::formatParameters(ExpressionFormatter const& formatter)
{
    sortParameters(parameters_);
    for (QSharedPointer<Parameter> parameter : *parameters_)
    {
        parameter->setValue(formatter.formatReferringExpression(parameter->getValue()));
    }

    sortParameters(moduleParameters_);
    for (QSharedPointer<Parameter> moduleParameter : *moduleParameters_)
    {
        moduleParameter->setValue(formatter.formatReferringExpression(moduleParameter->getValue()));
    }
}

//-----------------------------------------------------------------------------
// Function: MetaComponent::sortParameters()
//-----------------------------------------------------------------------------
void MetaComponent::sortParameters(QSharedPointer<QList<QSharedPointer<Parameter> > > sortParameters)
{
    // Go through existing ones on the instance.
    foreach (QSharedPointer<Parameter> current, *sortParameters)
    {
        // The value of the inspected parameter.
        QString currentValue = current->getValue();
        QString currentId = current->getValueId();

        // The start position for the second pass.
        auto startPosition = std::find_if(sortParameters->begin(), sortParameters->end(),
            [&currentValue](QSharedPointer<Parameter> referenced) { return currentValue.contains(
                referenced->getValueId()); });

        // If none found, next search starts from the beginning.
        if (startPosition == sortParameters->end())
        {
            startPosition = sortParameters->begin();
        }

        auto firstReferencing = std::find_if(startPosition, sortParameters->end(),
            [&currentId](QSharedPointer<Parameter> referencing){ return referencing->getValue().contains(
                currentId); });
        
        // Move to end of the list, unless referencing parameter is found.
        int target = sortParameters->size();
        if (firstReferencing != sortParameters->end())
        {
            target = sortParameters->indexOf(*firstReferencing);

            if (target > sortParameters->indexOf(current))
            {
                --target;
            }
        }

        sortParameters->removeOne(current);
        sortParameters->insert(target, current);
    }
}

//-----------------------------------------------------------------------------
// Function: MetaComponent::parsesFileSets()
//-----------------------------------------------------------------------------
void MetaComponent::parsesFileSets()
{
    for (QString const& fileSetRef : *activeInstantiation_->getFileSetReferences())
    {
        QSharedPointer<FileSet> fileSet = component_->getFileSet(fileSetRef);
        if (fileSet)
        {
            fileSets_->append(fileSet);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MetaComponent::formatPorts()
//-----------------------------------------------------------------------------
void MetaComponent::formatPorts(ExpressionFormatter const& formatter)
{
    for (QSharedPointer<Port> cport : *component_->getPorts())
    {
        QSharedPointer<MetaPort> mPort(new MetaPort);
        mPort->port_ = cport;
        mPort->arrayBounds_.first = "";
        mPort->arrayBounds_.second = "";
        mPort->width_ = "";
        mPort->vectorBounds_.first = formatter.formatReferringExpression(cport->getLeftBound());
        mPort->vectorBounds_.second = formatter.formatReferringExpression(cport->getRightBound());
        mPort->isWire_ = false;
        mPort->isTransactional_ = false;

        if (cport->getWire())
        {
            mPort->isWire_ = true;
            mPort->arrayBounds_.first = formatter.formatReferringExpression(cport->getArrayLeft());
            mPort->arrayBounds_.second = formatter.formatReferringExpression(cport->getArrayRight());
        }
        else if (cport->getTransactional())
        {
            mPort->isTransactional_ = true;
            mPort->width_ = formatter.formatReferringExpression(cport->getTransactional()->getBusWidth());
        }

        ports_->insert(cport->name(), mPort);
    }
}

//-----------------------------------------------------------------------------
// Function: MetaComponent::parseRemapStates
//-----------------------------------------------------------------------------
void MetaComponent::parseRemapStates(ExpressionFormatter const& formatter)
{
    for (QSharedPointer<RemapState> currentState : *component_->getRemapStates())
    {
        QSharedPointer<FormattedRemapState> remapState(new FormattedRemapState);
        remapState->state_ = currentState;
        remapStates_->append(remapState);

        // Each port referred by the state must be listed.
        for (QSharedPointer<RemapPort> remapPort : *currentState->getRemapPorts())
        {
            // Pick the port name, and the value needed for it to remap state become effective.
            QSharedPointer<QPair<QSharedPointer<Port>,QString> > parsedPort
                (new QPair<QSharedPointer<Port>, QString>);
            parsedPort->first = component_->getPort(remapPort->getPortNameRef());
            parsedPort->second = formatter.formatReferringExpression(remapPort->getValue());

            remapState->ports_.append(parsedPort);
        }
    }
}
