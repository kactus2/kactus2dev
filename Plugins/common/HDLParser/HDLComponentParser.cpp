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

#include <Plugins/VerilogGenerator/ModelParameterVerilogWriter/ModelParameterVerilogWriter.h>
#include <Plugins/VerilogGenerator/PortVerilogWriter/PortVerilogWriter.h>

#include <Plugins/VerilogGenerator/CommentWriter/CommentWriter.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/Port.h>
#include <IPXACTmodels/common/ModuleParameter.h>
#include <IPXACTmodels/common/VLNV.h>

#include <IPXACTmodels/Component/MemoryMap.h>
#include <IPXACTmodels/Component/MemoryRemap.h>
#include <IPXACTmodels/Component/MemoryBlockBase.h>
#include <IPXACTmodels/Component/AddressBlock.h>
#include <IPXACTmodels/Component/Register.h>
#include <IPXACTmodels/Component/Field.h>

#include <QSharedPointer>
#include "editors/ComponentEditor/common/ExpressionParser.h"
#include "IPXACTmodels/Component/View.h"
#include "IPXACTmodels/Component/ComponentInstantiation.h"

//-----------------------------------------------------------------------------
// Function: HDLComponentParser::HDLComponentParser
//-----------------------------------------------------------------------------
HDLComponentParser::HDLComponentParser(QSharedPointer<Component> component,
	QSharedPointer<View> activeView, QSharedPointer<const PortSorter> sorter,
	QSharedPointer<ExpressionFormatter> expressionFormatter) :
component_(component),
activeView_(activeView),
sorter_(sorter),
formatter_(expressionFormatter)
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
QSharedPointer<GenerationComponent> HDLComponentParser::parseComponent() const
{
    QSharedPointer<GenerationComponent> retval(new GenerationComponent);
    retval->component = component_;
    parseRegisters(retval);
    parseParameterDeclarations(retval);
    retval->sortedPortNames = sorter_->sortedPortNames(component_);

    return retval;
}

//-----------------------------------------------------------------------------
// Function: HDLComponentParser::parseRegisters
//-----------------------------------------------------------------------------
void HDLComponentParser::parseRegisters(QSharedPointer<GenerationComponent> target) const
{
    // TODO: replace with a foreach structure
    if (component_->getMemoryMaps()->size() < 1)
    {
        return;
    }

	QSharedPointer<MemoryMap> memmap = component_->getMemoryMaps()->first();

	// The AUB is shared between all remap states.
	QString aub = memmap->getAddressUnitBits();

	// The default map defines the size. We assume that any remaps respect there boundaries.
	QString totalRange;

	// List of the address blocks to go through.
	QList<QSharedPointer<AddressBlock> > blocks;

	// Get the total sum of ranges.
	foreach (QSharedPointer<MemoryBlockBase> mbb, *memmap->getMemoryBlocks())
	{
		QSharedPointer<AddressBlock> ab = mbb.dynamicCast<AddressBlock>();
		
		// Skip if does not convert to correct type.
		if (!ab)
		{
			continue;
		}

		totalRange += ab->getRange();

		blocks.append(ab);
	}

	foreach (QSharedPointer<MemoryRemap> remap, *memmap->getMemoryRemaps())
	{
		foreach (QSharedPointer<MemoryBlockBase> mbb, *remap->getMemoryBlocks())
	    {
			QSharedPointer<AddressBlock> ab = mbb.dynamicCast<AddressBlock>();

			// Skip if does not convert to correct type.
			if (!ab)
			{
				continue;
			}

			blocks.append(ab);
		}
	}

	foreach (QSharedPointer<AddressBlock> ab, blocks)
    {
        QSharedPointer<GenerationAddressBlock> gab(new GenerationAddressBlock);

        gab->baseAddress_ = ab->getBaseAddress();

        target->blocks.append(gab);

        foreach (QSharedPointer<RegisterBase> rb, *ab->getRegisterData())
        {
            QSharedPointer<Register> r = rb.dynamicCast<Register>();

            // Skip if does not convert to correct type.
            if (!r)
            {
                continue;
            }

		    QSharedPointer<GenerationRegister> gr(new GenerationRegister);

            gr->dimension_ = r->getDimension();
            gr->offset_ = r->getAddressOffset();
            gr->size_ = r->getSize();

            gab->registers.append(gr);

            foreach (QSharedPointer<Field> f, *r->getFields())
            {
                QSharedPointer<GenerationField> gf(new GenerationField);

                gf->bitOffset_ = f->getBitOffset();
                gf->bitWidth_ = f->getBitWidth();
            }
        }
	}
}

//-----------------------------------------------------------------------------
// Function: HDLComponentParser::parseParameterDeclarations()
//-----------------------------------------------------------------------------
void HDLComponentParser::parseParameterDeclarations(QSharedPointer<GenerationComponent> target) const
{
	if (!activeView_)
	{
		return;
	}

	QSharedPointer<ComponentInstantiation> currentInsta =
		component_->getModel()->findComponentInstantiation(activeView_->getComponentInstantiationRef());

	// Take copy the parameters of the component instantiation as well as the component.
	QList<QSharedPointer<Parameter> > parameters = QList<QSharedPointer<Parameter> >(*component_->getParameters());

	if (currentInsta)
	{
		foreach(QSharedPointer<ModuleParameter> parameter, *currentInsta->getModuleParameters())
		{
			parameters.append(parameter);
		}
	}

	// Create a new list of module parameters.
	target->parameters = QList<QSharedPointer<Parameter> >(parameters);

	sortParameters(parameters, target->parameters);
}

//-----------------------------------------------------------------------------
// Function: HDLComponentParser::sortModuleParameters()
//-----------------------------------------------------------------------------
void HDLComponentParser::sortParameters(QList<QSharedPointer<Parameter> >& parameters,
    QList<QSharedPointer<Parameter> >& parametersToWrite ) const
{
	// Go through existing ones on the instance.
	for (QList<QSharedPointer<Parameter> >::Iterator parameterAdd =
		parameters.begin(); parameterAdd != parameters.end(); ++parameterAdd)
	{
		// The first position for the second pass.
		QList<QSharedPointer<Parameter> >::Iterator minPos =
			parametersToWrite.begin();

		// Resolve the value of the inspected parameter.
		QString addFormatted = formatter_->
			formatReferringExpression((*parameterAdd)->getValue());

		// First pass: Detect if the parameter depends on another parameter.
		for (QList<QSharedPointer<Parameter> >::Iterator parameterCmp =
			parametersToWrite.begin();
			parameterCmp != parametersToWrite.end(); ++parameterCmp)
		{
			if (addFormatted.contains((*parameterCmp)->name()))
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
			parameterCmp != parametersToWrite.end(); ++parameterCmp)
		{
			// Resolve the value of the the compared parameter.
			QString formatted = formatter_->formatReferringExpression((*parameterCmp)->getValue());

			// Check if it contains a reference to the inspected parameter.
			if (formatted.contains((*parameterAdd)->name()))
			{
				// Remove the inspected parameter from the previous place.
				parametersToWrite.removeOne(*parameterAdd);

				// Then the inspected parameter comes before it is referred.
				parametersToWrite.insert(parameterCmp, *parameterAdd);

				// It will not be inserted twice, so break here.
				append = false;
				break;
			}
		}

		// If there was no match in the second pass, or no second pass at all, append to the end.
		if (append)
		{
			// Remove the inspected parameter from the previous place.
			parametersToWrite.removeOne(*parameterAdd);
			// Append at the end of the list.
			parametersToWrite.append(*parameterAdd);
		}
	}
}