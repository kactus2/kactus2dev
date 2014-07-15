/* 
 *  	Created on: 26.10.2011
 *      Author: Antti Kamppi
 * 		filename: vhdlcomponentdeclaration.cpp
 *		Project: Kactus 2
 */

#include "vhdlcomponentdeclaration.h"

#include "vhdlgeneral.h"
#include "vhdlgeneric.h"
#include "vhdlport.h"

#include <kactusGenerators/HDLGenerator/HDLComponentInstance.h>
#include <kactusGenerators/HDLGenerator/HDLUtils.h>

#include <IPXACTmodels/component.h>
#include <IPXACTmodels/modelparameter.h>
#include <IPXACTmodels/port.h>
#include <IPXACTmodels/generaldeclarations.h>

#include <QSharedPointer>

//-----------------------------------------------------------------------------
// Function: VhdlComponentDeclaration::VhdlComponentDeclaration()
//-----------------------------------------------------------------------------
VhdlComponentDeclaration::VhdlComponentDeclaration(QSharedPointer<Component> component, QObject* parent):
HDLComponentDeclaration(component, parent),
generics_(),
ports_()
{
	// parse the generics for the component declaration
	QList<QSharedPointer<ModelParameter> > modelParams = component->getModelParameters();
	foreach (QSharedPointer<ModelParameter> modelParam, modelParams)
    {
		QSharedPointer<VhdlGeneric> generic(new VhdlGeneric(modelParam.data(), this));
		generics_.insert(generic->name(), generic);
	}

	// parse the ports for the component declaration
	QList<QSharedPointer<Port> > ports = component->getPorts();
	foreach (QSharedPointer<Port> port, ports)
    {	
		// do not add ports with invalid direction or phantom direction
		if (port->getDirection() == General::DIRECTION_INVALID ||
			port->getDirection() == General::DIRECTION_PHANTOM) {
			continue;
		}

		// create the actual port
		QSharedPointer<VhdlPort> vhdlPort(new VhdlPort(port.data(), this));

		// create the sorter instance
		VhdlPortSorter sorter(component->getInterfaceNameForPort(vhdlPort->name()), vhdlPort->name(), 
            port->getDirection());

		// this port can not be created yet
		Q_ASSERT(!ports_.contains(sorter));

		ports_.insert(sorter, vhdlPort);
	}
}

//-----------------------------------------------------------------------------
// Function: VhdlComponentDeclaration::~VhdlComponentDeclaration()
//-----------------------------------------------------------------------------
VhdlComponentDeclaration::~VhdlComponentDeclaration()
{

}

//-----------------------------------------------------------------------------
// Function: vhdlcomponentdeclaration::write()
//-----------------------------------------------------------------------------
void VhdlComponentDeclaration::write( QTextStream& stream ) const
{
	// if component contains a description
	if (!description().isEmpty())
    {
		HDLUtils::writeDescription(description(), stream, "--", QString("  "));
	}
	stream << "  " << "-- IP-XACT VLNV: " << componentModel()->getVlnv()->toString() << endl;
	stream << "  " << "component " << type() << endl;

    writeGenerics(stream);

    writePorts(stream);

	stream << "  end component;" << endl;
}

//-----------------------------------------------------------------------------
// Function: vhdlcomponentdeclaration::checkPortConnections()
//-----------------------------------------------------------------------------
void VhdlComponentDeclaration::checkPortConnections()
{
	// check each vhdl port
	foreach (QSharedPointer<HDLPort> port, ports_)
    {		
		// check each instance of this type
		foreach (QSharedPointer<HDLComponentInstance> instantation, instances())
        {			
			// if the named port is connected in this instance
			if (instantation->hasConnection(port->name())) {
				
				// port is needed so it can not be commented and search can be 
				// stopped for this port
				port->setConnected(true);
				break;
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Function: vhdlcomponentdeclaration::portType()
//-----------------------------------------------------------------------------
QString VhdlComponentDeclaration::portType(QString const& portName) const
{
	// used to search for the correct port
	VhdlPortSorter sorter(componentModel()->getInterfaceNameForPort(portName),
		portName, componentModel()->getPortDirection(portName));
	
	// if the named port is found
	if (ports_.contains(sorter))
    {
		return ports_.value(sorter)->type();
	}
	else
    {
		return QString("undefined port");
	}
}

//-----------------------------------------------------------------------------
// Function: VhdlComponentDeclaration::writeGenerics()
//-----------------------------------------------------------------------------
void VhdlComponentDeclaration::writeGenerics(QTextStream& stream) const
{
    if (generics_.isEmpty())
    {
        return;
    }

    // Write the generic declarations.
    stream << "    "  << "generic (" << endl;
    for (QMap<QString, QSharedPointer<VhdlGeneric> >::const_iterator i = generics_.begin(); i != generics_.end(); 
        ++i)
    {
        stream << "      " ;
        i.value()->write(stream);

        // if this is not the last generic to write
        if (i + 1 != generics_.end())
        {
            stream << ";";
        }

        if (!i.value()->description().isEmpty())
        {
            stream << " ";
            HDLUtils::writeDescription(i.value()->description(), stream, "--");
        }
        else
        {
            stream << endl;
        }
    }
    stream << "    " << ");" << endl;
}

//-----------------------------------------------------------------------------
// Function: VhdlComponentDeclaration::writePorts()
//-----------------------------------------------------------------------------
void VhdlComponentDeclaration::writePorts(QTextStream& stream) const
{
    if (ports_.isEmpty() && !HDLPort::hasConnectedPorts(ports_.values()))
    {
        return;
    }

    // write the port declarations
    stream << "  " << "  " << "port (" << endl;
    QString previousInterface;
    for (QMap<VhdlPortSorter, QSharedPointer<HDLPort> >::const_iterator i = ports_.begin(); i != ports_.end(); ++i)
    {
        // if the port is first in the interface, then introduce the interface
        if (i.key().interface() != previousInterface)
        {
            stream << endl << "      -- ";

            const QString interfaceName = i.key().interface();

            if (interfaceName == QString("none"))
            {
                stream << "These ports are not in any interface" << endl;
            }
            else if (interfaceName == QString("several"))
            {
                stream << "There ports are contained in many interfaces" << endl;
            }
            else
            {	
                stream << "Interface: " << interfaceName << endl;

                const QString description = componentModel()->getInterfaceDescription(interfaceName);
                if (!description.isEmpty())
                {
                    HDLUtils::writeDescription(description, stream, "--", QString("      "));
                }
            }
            previousInterface = interfaceName;
        }

        stream << "  " << "  " << "  "; //3 indents
        i.value()->write(stream); // print the actual port definition


        // if this is not the last port to write
        if (i + 1 != ports_.end())
        {
            stream << ";";
        }

        if (!i.value()->description().isEmpty())
        {
            stream << " ";
            HDLUtils::writeDescription(i.value()->description(), stream, "--");
        }
        else
        {
            stream << endl;
        }
    }
    stream << "    );" << endl;
}
