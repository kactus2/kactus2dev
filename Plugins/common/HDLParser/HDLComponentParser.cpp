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

//-----------------------------------------------------------------------------
// Function: HDLComponentParser::HDLComponentParser
//-----------------------------------------------------------------------------
HDLComponentParser::HDLComponentParser(LibraryInterface* library, QSharedPointer<Component> component) :
library_(library),
component_(component)
{
}

//-----------------------------------------------------------------------------
// Function: HDLComponentParser::~HDLComponentParser
//-----------------------------------------------------------------------------
HDLComponentParser::~HDLComponentParser()
{
}

//-----------------------------------------------------------------------------
// Function: HDLComponentParser::parseComponent
//-----------------------------------------------------------------------------
void HDLComponentParser::parseComponent(QSharedPointer<View> activeView)
{
    // Must have an active view.
    activeView_ = activeView;

    if (!activeView_)
    {
        return;
    }

    retval_ = QSharedPointer<GenerationComponent>(new GenerationComponent);
    retval_->component_ = component_;

    // Try to find a component instantiation for the view.
    activeInstantiation_ = retval_->component_->getModel()->
        findComponentInstantiation(activeView_->getComponentInstantiationRef());

    if (activeInstantiation_ && !activeInstantiation_->getModuleName().isEmpty())
    {
        // If there is a named component instantiation, its module shall be used.
        retval_->moduleName_ = activeInstantiation_->getModuleName();
    }
    else
    {
        // Else take module name from the VLNV of the component.
        retval_->moduleName_ = retval_->component_->getVlnv().getName();
    }

    // Initialize the parameter finder.
    QSharedPointer<ComponentParameterFinder> parameterFinder(new ComponentParameterFinder(retval_->component_));
    parameterFinder->setActiveView(activeView);
    formatter_ = QSharedPointer<ExpressionFormatter>(new ExpressionFormatter(parameterFinder));

    // Find and parse the component stuff.
    findParameters();
    parseInterfaces();
    parsePorts();
    parseMemoryMaps();
    parseRemapStates();
}

//-----------------------------------------------------------------------------
// Function: HDLComponentParser::getParsedComponent()
//-----------------------------------------------------------------------------
QSharedPointer<GenerationComponent> HDLComponentParser::getParsedComponent()
{
    return retval_;
}

//-----------------------------------------------------------------------------
// Function: HDLComponentParser::sortModuleParameters()
//-----------------------------------------------------------------------------
void HDLComponentParser::sortParameters(QList<QSharedPointer<Parameter> >& refParameters,
    QList<QSharedPointer<Parameter> >& sortParameters )
{
    // Go through existing ones on the instance.
    for (QList<QSharedPointer<Parameter> >::Iterator parameterAdd =
        refParameters.begin(); parameterAdd != refParameters.end(); ++parameterAdd)
    {
        // The first position for the second pass.
        QList<QSharedPointer<Parameter> >::Iterator minPos =
            sortParameters.begin();

        // The value of the inspected parameter.
        QString valueAdd = (*parameterAdd)->getValue();

        // First pass: Detect if the parameter depends on another parameter.
        for (QList<QSharedPointer<Parameter> >::Iterator parameterCmp =
            sortParameters.begin();
            parameterCmp != sortParameters.end(); ++parameterCmp)
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
            parameterCmp != sortParameters.end(); ++parameterCmp)
        {
            // The value of the the compared parameter.
            QString valueCmp = (*parameterCmp)->getValue();

            // Check if it contains a reference to the inspected parameter.
            if (valueCmp.contains((*parameterAdd)->getValueId()))
            {
                // Take a direct shared pointer to the referring parameter.
                QSharedPointer<Parameter> parameterRef = *parameterCmp;

                // Remove the inspected parameter from the previous place.
                sortParameters.removeOne(*parameterAdd);

                // See where the referring parameter is located now.
                int cmpLoc = sortParameters.indexOf(*parameterCmp);

                // Then the inspected parameter comes before it is referred.
                sortParameters.insert(cmpLoc, *parameterAdd);

                // It will not be inserted twice, so break here.
                append = false;
                break;
            }
        }

        // If there was no match in the second pass, or no second pass at all, append to the end.
        if (append)
        {
            // Remove the inspected parameter from the previous place.
            sortParameters.removeOne(*parameterAdd);
            // Append at the end of the list.
            sortParameters.append(*parameterAdd);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: HDLComponentParser::findParameters()
//-----------------------------------------------------------------------------
void HDLComponentParser::findParameters()
{
    // Cull all the component parameters for the original parameters.
    foreach(QSharedPointer<Parameter> parameter, *retval_->component_->getParameters())
    {
        retval_->originalParameters_.append(parameter);
    }

    // If there is an active component instantiaion, take its module parameters as well.
    if (activeInstantiation_)
    {
        foreach(QSharedPointer<ModuleParameter> parameter, *activeInstantiation_->getModuleParameters())
        {
            retval_->originalParameters_.append(parameter);
        }
    }

    // Create a new list of the parameters.
    QList<QSharedPointer<Parameter> > sortedParameters = QList<QSharedPointer<Parameter> >(retval_->originalParameters_);

    // Sort the parameters.
    sortParameters(retval_->originalParameters_, sortedParameters);

    foreach(QSharedPointer<Parameter> parameterOrig, sortedParameters)
    {
        // Copy the sorted parameters and format them.
        QSharedPointer<Parameter> parameterCpy(new Parameter(*parameterOrig));

        parameterCpy->setValue(formatter_->formatReferringExpression(parameterOrig->getValue()));

        retval_->formattedParameters_.append(parameterCpy);
    }
}

//-----------------------------------------------------------------------------
// Function: HDLComponentParser::parseInterfaces()
//-----------------------------------------------------------------------------
void HDLComponentParser::parseInterfaces()
{
    foreach(QSharedPointer<BusInterface> busInterface, *retval_->component_->getBusInterfaces())
    {
        // Create "our" interface for each IP-XACT interface. Take the relevant values.
        QSharedPointer<GenerationInterface> gInterface(new GenerationInterface);
        gInterface->description_ = busInterface->description();
        gInterface->mode_ = interfaceMode2Str(busInterface->getInterfaceMode());
        gInterface->interface_ = busInterface;

        // Insert to the interface to the list.
        retval_->interfaces_.insert(busInterface->name(), gInterface);

        // Find the correct abstraction type.
        if (busInterface->getAbstractionTypes()->count() < 1)
        {
            continue;
        }

        QSharedPointer<AbstractionType> absType = busInterface->getAbstractionTypes()->first();
        gInterface->absType_ = absType;

        // An abstraction definition is needed. It comes through VLNV reference.
        QSharedPointer<ConfigurableVLNVReference> absRef = absType->getAbstractionRef();

        if (absRef)
        {
            QSharedPointer<AbstractionDefinition> absDef =
                library_->getModel(*absRef).dynamicCast<AbstractionDefinition>();

            if (absDef)
            {
                gInterface->absDef_ = absDef;
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: HDLComponentParser::parsePorts()
//-----------------------------------------------------------------------------
void HDLComponentParser::parsePorts()
{
    foreach (QSharedPointer<Port> cport, *retval_->component_->getPorts())
    {
        // Create generation port.
        QSharedPointer<GenerationPort> gport(new GenerationPort);

        // Needs a reference to the IP-XACT port.
        gport->port_ = cport;

        // Both vector and array bounds may be needed.
        gport->arrayBounds_.first = formatter_->formatReferringExpression(cport->getArrayLeft());
        gport->arrayBounds_.second = formatter_->formatReferringExpression(cport->getArrayRight());

        gport->vectorBounds_.first = formatter_->formatReferringExpression(cport->getLeftBound());
        gport->vectorBounds_.second = formatter_->formatReferringExpression(cport->getRightBound());

        // Add to the list.
        retval_->ports_.insert(cport->name(),gport);
    }
}

//-----------------------------------------------------------------------------
// Function: HDLComponentParser::parseMemoryMaps
//-----------------------------------------------------------------------------
void HDLComponentParser::parseMemoryMaps()
{
    // TODO: replace with a foreach structure
    if (retval_->component_->getMemoryMaps()->size() < 1)
    {
        return;
    }

	QSharedPointer<MemoryMap> memmap = retval_->component_->getMemoryMaps()->first();

	// The AUB is shared between all remap states.
	retval_->aub_ = memmap->getAddressUnitBits();

    QSharedPointer<GenerationRemap> defaultRemap(new GenerationRemap);
    retval_->remaps_.append(defaultRemap);
    defaultRemap->name_ = memmap->name() + "_default";

	foreach (QSharedPointer<MemoryBlockBase> mbb, *memmap->getMemoryBlocks())
	{
		QSharedPointer<AddressBlock> ab = mbb.dynamicCast<AddressBlock>();
		
		// Skip if does not convert to correct type.
		if (!ab)
		{
			continue;
		}

        // Get the total sum of ranges.
		retval_->totalRange_ += ab->getRange();

        parseAddressBlock(ab, defaultRemap);
	}

    // The default map defines the size. We assume that any remaps respect there boundaries.
    retval_->totalRange_ = formatter_->formatReferringExpression(retval_->totalRange_);

	foreach (QSharedPointer<MemoryRemap> remap, *memmap->getMemoryRemaps())
    {
        QSharedPointer<GenerationRemap> rm(new GenerationRemap);
        rm->name_ = remap->name();
        retval_->remaps_.append(rm);

		foreach (QSharedPointer<MemoryBlockBase> mbb, *remap->getMemoryBlocks())
	    {
			QSharedPointer<AddressBlock> ab = mbb.dynamicCast<AddressBlock>();

			// Skip if does not convert to correct type.
			if (!ab)
			{
				continue;
			}

            parseAddressBlock(ab, rm);
		}
	}
}

//-----------------------------------------------------------------------------
// Function: HDLComponentParser::parseAddressBlock
//-----------------------------------------------------------------------------
void HDLComponentParser::parseAddressBlock(QSharedPointer<AddressBlock> ab, QSharedPointer<GenerationRemap> target)
{
    // Create a new address block for generation.
    QSharedPointer<GenerationAddressBlock> gab(new GenerationAddressBlock);

    // Its base address and name are needed.
    gab->baseAddress_ = formatter_->formatReferringExpression(ab->getBaseAddress());
    gab->name_ = ab->name();

    // Append to the list.
    target->blocks_.append(gab);

    foreach (QSharedPointer<RegisterBase> rb, *ab->getRegisterData())
    {
        QSharedPointer<Register> r = rb.dynamicCast<Register>();

        // Skip if does not convert to correct type.
        if (!r)
        {
            continue;
        }

        QSharedPointer<GenerationRegister> gr(new GenerationRegister);

        if (r->getDimension().isEmpty())
        {
            // No dimension is interpreted as one.
            gr->dimension_ = "1";
        }
        else
        {
            // Else take it via formatting.
            gr->dimension_ = formatter_->formatReferringExpression(r->getDimension());
        }

        // Pick also other needed values via formatter, except the name.
        gr->offset_ = formatter_->formatReferringExpression(r->getAddressOffset());
        gr->size_ = formatter_->formatReferringExpression(r->getSize());
        gr->name_ = r->name();

        // Append to the list.
        gab->registers_.append(gr);

        foreach (QSharedPointer<Field> f, *r->getFields())
        {
            QSharedPointer<GenerationField> gf(new GenerationField);

            // Pick also the values needed by a field via formatter, except the name.
            gf->bitOffset_ = formatter_->formatReferringExpression(f->getBitOffset());
            gf->bitWidth_ = formatter_->formatReferringExpression(f->getBitWidth());
            gf->name_ = f->name();

            // Append to the list.
            gr->fields_.append(gf);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: HDLComponentParser::parseRemapStates
//-----------------------------------------------------------------------------
void HDLComponentParser::parseRemapStates()
{
    // TODO: replace with a foreach structure
    if (retval_->component_->getMemoryMaps()->size() < 1)
    {
        return;
    }

    QSharedPointer<MemoryMap> memmap = retval_->component_->getMemoryMaps()->first();

    // Each remap is may have a remap state.
    foreach (QSharedPointer<MemoryRemap> remap, *memmap->getMemoryRemaps())
    {
        QString stateName = remap->getRemapState();

        QSharedPointer<RemapState> state;
        
        // Try to match the remap state reference to a remap state within the component.
        foreach(QSharedPointer<RemapState> currentState, *retval_->component_->getRemapStates())
        {
            if (currentState->name() == stateName)
            {
                state = currentState;
                break;
            }
        }

        // If there is none, skip.
        if (!state)
        {
            continue;
        }

        // Create new remap state for the generation.
        QSharedPointer<GenerationRemapState> grms(new GenerationRemapState);
        grms->stateName_ = stateName;
        retval_->remapStates_.append(grms);

        // Each port referred by the state must be listed.
        foreach(QSharedPointer<RemapPort> rmport, *state->getRemapPorts())
        {
           QSharedPointer<QPair<QSharedPointer<Port>,QString> > parsedPort(new QPair<QSharedPointer<Port>,QString>);
           
           // Pick the port name, and the value needed for it to remap state become effective.
           parsedPort->first = retval_->component_->getPort(rmport->getPortNameRef());
           parsedPort->second = formatter_->formatReferringExpression(rmport->getValue());

           grms->ports_.append(parsedPort);
        }
    }
}