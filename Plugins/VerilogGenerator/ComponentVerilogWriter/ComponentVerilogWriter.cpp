//-----------------------------------------------------------------------------
// File: ComponentVerilogWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 01.08.2014
//
// Description:
// Class for writing a component as a Verilog module.
//-----------------------------------------------------------------------------

#include "ComponentVerilogWriter.h"

#include <Plugins/VerilogGenerator/ModelParameterVerilogWriter/ModelParameterVerilogWriter.h>
#include <Plugins/VerilogGenerator/PortVerilogWriter/PortVerilogWriter.h>

#include <Plugins/VerilogGenerator/CommentWriter/CommentWriter.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/Port.h>
#include <IPXACTmodels/common/ModuleParameter.h>
#include <IPXACTmodels/common/VLNV.h>

#include <QSharedPointer>
#include "editors/ComponentEditor/common/ExpressionParser.h"
#include "IPXACTmodels/Component/View.h"
#include "IPXACTmodels/Component/ComponentInstantiation.h"

#include <Plugins/VerilogImport/VerilogSyntax.h>

//-----------------------------------------------------------------------------
// Function: ComponentVerilogWriter::ComponentVerilogWriter
//-----------------------------------------------------------------------------
ComponentVerilogWriter::ComponentVerilogWriter(QSharedPointer<Component> component,
	QSharedPointer<View> activeView, QSharedPointer<const PortSorter> sorter,
	QSharedPointer<ExpressionParser> expressionParser,
	QSharedPointer<ExpressionFormatter> expressionFormatte) :
component_(component),
activeView_(activeView),
sorter_(sorter),
childWriters_(),
parser_(expressionParser),
formatter_(expressionFormatte)
{

}

//-----------------------------------------------------------------------------
// Function: ComponentVerilogWriter::~ComponentVerilogWriter
//-----------------------------------------------------------------------------
ComponentVerilogWriter::~ComponentVerilogWriter()
{
    childWriters_.clear();
}

//-----------------------------------------------------------------------------
// Function: ComponentVerilogWriter::write
//-----------------------------------------------------------------------------
void ComponentVerilogWriter::write(QTextStream& outputStream) const
{
    if (nothingToWrite())
    {
        return;
    }

    writeModuleDeclaration(outputStream);

    writeInternalWiresAndComponentInstances(outputStream);

	if ( implementation_ )
	{
		// If an implementation exists, there must be a warning about overwriting as well.
		outputStream << "// " << VerilogSyntax::TAG_OVERRIDE << endl;

		implementation_->write(outputStream);
	}
		
	writeModuleEnd(outputStream);

	if ( postModule_ )
	{
		postModule_->write(outputStream);
	}
}

//-----------------------------------------------------------------------------
// Function: ComponentVerilogWriter::setImplementation
//-----------------------------------------------------------------------------
void ComponentVerilogWriter::setImplementation( QSharedPointer<TextBodyWriter> implementation )
{
	implementation_ = implementation;
}

//-----------------------------------------------------------------------------
// Function: ComponentVerilogWriter::setPostModule
//-----------------------------------------------------------------------------
void ComponentVerilogWriter::setPostModule( QSharedPointer<TextBodyWriter> postModule )
{
	postModule_ = postModule;
}

//-----------------------------------------------------------------------------
// Function: ComponentVerilogWriter::nothingToWrite
//-----------------------------------------------------------------------------
bool ComponentVerilogWriter::nothingToWrite() const
{
    return component_.isNull();
}

//-----------------------------------------------------------------------------
// Function: ComponentVerilogWriter::writeModuleDeclaration()
//-----------------------------------------------------------------------------
void ComponentVerilogWriter::writeModuleDeclaration(QTextStream& outputStream) const
{
    outputStream << "module " << component_->getVlnv().getName();
    
    writeParameterDeclarations(outputStream);

    writePortDeclarations(outputStream);     
}

//-----------------------------------------------------------------------------
// Function: ComponentVerilogWriter::portNames()
//-----------------------------------------------------------------------------
QString ComponentVerilogWriter::portNames() const
{
    return sorter_->sortedPortNames(component_).join(", ");
}

//-----------------------------------------------------------------------------
// Function: ComponentVerilogWriter::writeParameterDeclarations()
//-----------------------------------------------------------------------------
void ComponentVerilogWriter::writeParameterDeclarations(QTextStream& outputStream) const
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
	QList<QSharedPointer<Parameter> > parametersToWrite(parameters);

	sortParameters(parameters, parametersToWrite);

	if (!parametersToWrite.isEmpty())
	{
		outputStream << " #(" << endl;

		foreach(QSharedPointer<Parameter> parameter, parametersToWrite)
		{
			bool isLastParameter = parameter == parametersToWrite.last();
			writeParameter(outputStream, parameter, isLastParameter);
		}

		outputStream << ") ";
	}
}

//-----------------------------------------------------------------------------
// Function: ComponentVerilogWriter::sortModuleParameters()
//-----------------------------------------------------------------------------
void ComponentVerilogWriter::sortParameters(QList<QSharedPointer<Parameter> >& parameters,
	QList<QSharedPointer<Parameter> >& parametersToWrite) const
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
			QString formatted = this->formatter_->
				formatReferringExpression((*parameterCmp)->getValue());

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

		// If there was no match in the second pass, or no second pass at all, at to then end.
		if (append)
		{
			// Remove the inspected parameter from the previous place.
			parametersToWrite.removeOne(*parameterAdd);
			// Append at the end of the list.
			parametersToWrite.append(*parameterAdd);
		}
	}
}

//-----------------------------------------------------------------------------
// Function: ComponentVerilogWriter::writeParameter()
//-----------------------------------------------------------------------------
void ComponentVerilogWriter::writeParameter(QTextStream& outputStream, QSharedPointer<Parameter> parameter,
    bool isLast) const
{
    outputStream << indentation();
    ModuleParameterVerilogWriter parameterWriter(parameter, formatter_);
    parameterWriter.write(outputStream);

    if (!isLast)
    {
        outputStream << ",";
    }

    CommentWriter descriptionWriter(parameter->description());
    descriptionWriter.setIndent(4);
    descriptionWriter.write(outputStream);

    if (parameter->description().isEmpty())
    {
        outputStream << endl;
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentVerilogWriter::indentation()
//-----------------------------------------------------------------------------
QString ComponentVerilogWriter::indentation() const
{
    return "    ";
}

//-----------------------------------------------------------------------------
// Function: ComponentVerilogWriter::writePortDeclarations()
//-----------------------------------------------------------------------------
void ComponentVerilogWriter::writePortDeclarations(QTextStream& outputStream) const
{
    QString previousInterfaceName = "";

    outputStream << "(";

    QStringList ports = sorter_->sortedPortNames(component_);
    foreach(QString portName, ports)
	{    
        QSharedPointer<QList<QSharedPointer<BusInterface> > > busInterfaces =
            component_->getInterfacesUsedByPort(portName);

        if (busInterfaces->size() == 1)
		{
            writeInterfaceIntroduction(busInterfaces->first()->name(), previousInterfaceName, outputStream);
		}
        else if (!busInterfaces->isEmpty())
        {
            writeInterfaceIntroduction("several", previousInterfaceName, outputStream);
        }
		else
		{
			writeInterfaceIntroduction("none", previousInterfaceName, outputStream);
		}

        bool lastPortToWrite = portName == ports.last();
        writePort(outputStream, component_->getPort(portName), lastPortToWrite);
    }
    
    outputStream << ");" << endl;
}

//-----------------------------------------------------------------------------
// Function: ComponentVerilogWriter::writeInterfaceIntroduction()
//-----------------------------------------------------------------------------
void ComponentVerilogWriter::writeInterfaceIntroduction(QString const& interfaceName, QString& previousInterfaceName,
    QTextStream& outputStream) const
{
    if (previousInterfaceName.compare(interfaceName) != 0)
    {
        outputStream << endl;

        if (interfaceName == "none")
        {
            outputStream << indentation() << "// These ports are not in any interface" << endl;                
        }
        else if (interfaceName == "several")
        {
            outputStream << indentation() << "// There ports are contained in many interfaces" << endl;       
        }
        else
        {
            outputStream << indentation() << "// Interface: " << interfaceName << endl;

            QSharedPointer<const BusInterface> busInterface = component_->getBusInterface(interfaceName);
            CommentWriter descriptionWriter(busInterface->description());
            descriptionWriter.setIndent(4);
            descriptionWriter.write(outputStream);
        }        
        previousInterfaceName = interfaceName;                
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentVerilogWriter::writePort()
//-----------------------------------------------------------------------------
void ComponentVerilogWriter::writePort(QTextStream& outputStream, QSharedPointer<Port> port, bool isLast) const
{
    outputStream << indentation();

    PortVerilogWriter writer(port, formatter_);
    writer.write(outputStream);

    if (!isLast)
    {
        outputStream << ",";
    }

    CommentWriter descriptionWriter(port->description());
    descriptionWriter.setIndent(4);
    descriptionWriter.write(outputStream);

    if (port->description().isEmpty())
    {
        outputStream << endl;
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentVerilogWriter::writeInternalWiresAndComponentInstances()
//-----------------------------------------------------------------------------
void ComponentVerilogWriter::writeInternalWiresAndComponentInstances(QTextStream& outputStream) const
{
    outputStream << endl;

    WriterGroup::write(outputStream);
}

//-----------------------------------------------------------------------------
// Function: ComponentVerilogWriter::writeModuleEnd()
//-----------------------------------------------------------------------------
void ComponentVerilogWriter::writeModuleEnd(QTextStream& outputStream) const
{
    outputStream << "endmodule" << endl;
}
