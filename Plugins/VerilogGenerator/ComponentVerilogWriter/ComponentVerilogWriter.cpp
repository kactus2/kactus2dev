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


#include <IPXACTmodels/component.h>
#include <IPXACTmodels/businterface.h>
#include <IPXACTmodels/port.h>
#include <IPXACTmodels/modelparameter.h>
#include <IPXACTmodels/vlnv.h>

#include <QSharedPointer>

//-----------------------------------------------------------------------------
// Function: ComponentVerilogWriter::ComponentVerilogWriter
//-----------------------------------------------------------------------------
ComponentVerilogWriter::ComponentVerilogWriter(QSharedPointer<const Component> component,
    QSharedPointer<const PortSorter> sorter) :
component_(component), sorter_(sorter), childWriters_()
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

    writeModuleEnd(outputStream);
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
void ComponentVerilogWriter::writeModuleDeclaration( QTextStream& outputStream ) const
{
    outputStream << "module " << component_->getVlnv()->getName();
    
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
// Function: VerilogComponentVerilogWriterGenerator::writeParameterDeclarations()
//-----------------------------------------------------------------------------
void ComponentVerilogWriter::writeParameterDeclarations(QTextStream& outputStream) const
{
    if (component_->hasModelParameters())
    {
        outputStream << " #(" << endl;

        QList<QSharedPointer<ModelParameter> > modelParameters = component_->getModelParameters();
        foreach(QSharedPointer<ModelParameter> parameter, modelParameters)
        {
            bool isLastParameter = parameter == modelParameters.last();
            writeParameter(outputStream, parameter, isLastParameter);
        }

        outputStream << ") ";
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentVerilogWriter::writeParameter()
//-----------------------------------------------------------------------------
void ComponentVerilogWriter::writeParameter(QTextStream& outputStream, QSharedPointer<ModelParameter> parameter,
    bool isLast) const
{
    outputStream << indentation();
    ModelParameterVerilogWriter parameterWriter(parameter);
    parameterWriter.write(outputStream);

    if (!isLast)
    {
        outputStream << ",";
    }

    CommentWriter descriptionWriter(parameter->getDescription());
    descriptionWriter.setIndent(4);
    descriptionWriter.write(outputStream);

    if (parameter->getDescription().isEmpty())
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
        writeInterfaceIntroduction(component_->getInterfaceNameForPort(portName), previousInterfaceName, 
            outputStream);

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
            CommentWriter descriptionWriter(busInterface->getDescription());
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

    PortVerilogWriter writer(port);
    writer.write(outputStream);

    if (!isLast)
    {
        outputStream << ",";
    }

    CommentWriter descriptionWriter(port->getDescription());
    descriptionWriter.setIndent(4);
    descriptionWriter.write(outputStream);

    if (port->getDescription().isEmpty())
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
