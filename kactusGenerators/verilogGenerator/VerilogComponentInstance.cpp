//-----------------------------------------------------------------------------
// File: VerilogComponentInstance.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 15.7.2014
//
// Description:
// <Short description of the class/file contents>
//-----------------------------------------------------------------------------

#include "VerilogComponentInstance.h"

//-----------------------------------------------------------------------------
// Function: VerilogComponentInstance::VerilogComponentInstance()
//-----------------------------------------------------------------------------
VerilogComponentInstance::VerilogComponentInstance(QObject* parent, QSharedPointer<Component> component,
    const QString& instanceName, QString const& viewName,
    const QString& description)
    : HDLComponentInstance(parent, component, instanceName, viewName, description),
portMaps_()
{

}

//-----------------------------------------------------------------------------
// Function: VerilogComponentInstance::~VerilogComponentInstance()
//-----------------------------------------------------------------------------
VerilogComponentInstance::~VerilogComponentInstance()
{

}

//-----------------------------------------------------------------------------
// Function: VerilogComponentInstance::write()
//-----------------------------------------------------------------------------
void VerilogComponentInstance::write(QTextStream& stream) const
{
    stream << type() << " ";

    writeParameterAssignments(stream);

    stream << name() << "(";

    writePortAssignments(stream);
    
    stream << endl << ")" << ";";
}

//-----------------------------------------------------------------------------
// Function: VerilogComponentInstance::mapToPort()
//-----------------------------------------------------------------------------
void VerilogComponentInstance::mapToPort(VhdlConnectionEndPoint const& endpoint, const QString& signalName)
{
    
}

//-----------------------------------------------------------------------------
// Function: VerilogComponentInstance::mapToSignal()
//-----------------------------------------------------------------------------
void VerilogComponentInstance::mapToSignal(const QString& portName, int portLeft, int portRight, 
    const QString& portType, const QString& signalName, int signalLeft, int signalRight, const QString& signalType)
{
    portMaps_.insert(Portmap(portName, portLeft, portRight), Portmap(signalName, signalLeft, signalRight));
}

//-----------------------------------------------------------------------------
// Function: VerilogComponentInstance::hasConnection()
//-----------------------------------------------------------------------------
bool VerilogComponentInstance::hasConnection(const QString& portName)
{
    for (QMap<Portmap, Portmap>::const_iterator i = portMaps_.constBegin();	i != portMaps_.constEnd(); ++i)
    {		
        // if the mapping is for port with same name
        if (i.key().name_.compare(portName, Qt::CaseInsensitive) == 0)
        {
            return true;
        }
    }

    // not port mapping for named port was found
    return false;
}

//-----------------------------------------------------------------------------
// Function: VerilogComponentInstance::useDefaultsForOtherPorts()
//-----------------------------------------------------------------------------
void VerilogComponentInstance::useDefaultsForOtherPorts()
{

}

//-----------------------------------------------------------------------------
// Function: VerilogComponentInstance::portType()
//-----------------------------------------------------------------------------
QString VerilogComponentInstance::portType(const QString& portName) const
{
    return QString();
}

//-----------------------------------------------------------------------------
// Function: VerilogComponentInstance::isScalarPort()
//-----------------------------------------------------------------------------
bool VerilogComponentInstance::isScalarPort(const QString& portName) const
{
    return false;
}

//-----------------------------------------------------------------------------
// Function: VerilogComponentInstance::writeParameterAssignments()
//-----------------------------------------------------------------------------
void VerilogComponentInstance::writeParameterAssignments(QTextStream& stream) const
{
    QMap<QString, QString> parameters = getAssignedModelParameters();
    if (!parameters.isEmpty())
    {
        stream << "#(";

        for (QMap<QString, QString>::const_iterator i = parameters.constBegin(); i != parameters.constEnd(); i++)
        {
            stream << "." << i.key() << "(" << i.value() << ")";

            if (i != parameters.constEnd() -1)
            {
                stream << ", ";
            }
        }

        stream << ")" << " ";
    }
}

//-----------------------------------------------------------------------------
// Function: VerilogComponentInstance::writePortAssignments()
//-----------------------------------------------------------------------------
void VerilogComponentInstance::writePortAssignments(QTextStream& stream) const
{
    for (QMap<Portmap, Portmap>::const_iterator i = portMaps_.constBegin(); i != portMaps_.constEnd(); i++)
    {
        stream << endl << "." << i.key().name_ << "(" << i.value().name_ ;

        int portSize = i.key().left_ - i.key().right_ + 1;
        int signalSize = i.value().left_ - i.value().right_ + 1;

        if (portSize != signalSize || i.value().right_ != 0)
        {
            stream << "[" << i.value().left_;

            if (i->left_ != i.value().right_)
            {
                stream << ":" << i.value().right_;
            }

            stream << "]";
        }

        stream << ")";

        if (i != portMaps_.constEnd() - 1)
        {
            stream << ",";
        }
    }
}
