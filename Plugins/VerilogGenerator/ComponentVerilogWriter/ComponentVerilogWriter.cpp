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

namespace
{
    const QString INDENT = "    ";
}

//-----------------------------------------------------------------------------
// Function: ComponentVerilogWriter::ComponentVerilogWriter
//-----------------------------------------------------------------------------
ComponentVerilogWriter::ComponentVerilogWriter(QSharedPointer<const Component> component,
    QSharedPointer<const PortSorter> sorter) :
component_(component), sorter_(sorter), instanceWriters_()
{

}

//-----------------------------------------------------------------------------
// Function: ComponentVerilogWriter::~ComponentVerilogWriter
//-----------------------------------------------------------------------------
ComponentVerilogWriter::~ComponentVerilogWriter()
{
    foreach(Writer* instanceWriter, instanceWriters_)
    {
        delete instanceWriter;
    }
    instanceWriters_.clear();
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

    writeParameterDeclarations(outputStream);

    writePortDeclarations(outputStream);

    writeComponentInstances(outputStream);

    writeModuleEnd(outputStream);
}

//-----------------------------------------------------------------------------
// Function: ComponentVerilogWriter::add()
//-----------------------------------------------------------------------------
void ComponentVerilogWriter::add(Writer* writer)
{
    instanceWriters_.append(writer);
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
    outputStream << "module " << component_->getVlnv()->getName() << "(" << portNames() << ");" << endl;
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
    foreach(QSharedPointer<ModelParameter> parameter, component_->getModelParameters())
    {
        outputStream << INDENT;
        ModelParameterVerilogWriter writer(parameter);
        writer.write(outputStream);
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentVerilogWriter::writePortDeclarations()
//-----------------------------------------------------------------------------
void ComponentVerilogWriter::writePortDeclarations(QTextStream& outputStream) const
{
    QString previousInterfaceName = "";

    foreach(QString portName, sorter_->sortedPortNames(component_))
    {    
        writeInterfaceIntroduction(component_->getInterfaceNameForPort(portName), previousInterfaceName, 
            outputStream);

        outputStream << INDENT;

        PortVerilogWriter writer(component_->getPort(portName));
        writer.write(outputStream);
    }

    outputStream << endl;
}

//-----------------------------------------------------------------------------
// Function: ComponentVerilogWriter::writeInterfaceIntroduction()
//-----------------------------------------------------------------------------
void ComponentVerilogWriter::writeInterfaceIntroduction(QString const& interfaceName, QString& previousInterfaceName,
    QTextStream& outputStream) const
{
    bool isFirstInterface = previousInterfaceName.isEmpty();
  
    if (previousInterfaceName.compare(interfaceName) != 0)
    {
        if (!isFirstInterface)
        {
            outputStream << endl;
        }

        if (interfaceName == "none")
        {
            outputStream << INDENT << "// These ports are not in any interface" << endl;                
        }
        else if (interfaceName == "several")
        {
            outputStream << INDENT << "// There ports are contained in many interfaces" << endl;       
        }
        else
        {
            outputStream << INDENT << "// Interface: " << interfaceName << endl;

            QSharedPointer<const BusInterface> busInterface = component_->getBusInterface(interfaceName);
            CommentWriter descriptionWriter(busInterface->getDescription());
            descriptionWriter.setIndent(4);
            descriptionWriter.write(outputStream);
        }        
        previousInterfaceName = interfaceName;                
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentVerilogWriter::writeComponentInstances()
//-----------------------------------------------------------------------------
void ComponentVerilogWriter::writeComponentInstances(QTextStream& outputStream) const
{
    foreach(Writer* writer, instanceWriters_)
    {
        writer->write(outputStream);
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentVerilogWriter::writeModuleEnd()
//-----------------------------------------------------------------------------
void ComponentVerilogWriter::writeModuleEnd(QTextStream& outputStream) const
{
    outputStream << "endmodule" << endl;
}
