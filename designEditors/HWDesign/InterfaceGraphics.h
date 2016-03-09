//-----------------------------------------------------------------------------
// File: InterfaceGraphics.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 04.09.2015
//
// Description:
// <Short description of the class/file contents>
//-----------------------------------------------------------------------------

#ifndef INTERFACEGRAPHICS_H
#define INTERFACEGRAPHICS_H

#include <IPXACTmodels/common/DirectionTypes.h>

#include <IPXACTmodels/Component/BusInterface.h>

namespace InterfaceGraphics
{

    //-----------------------------------------------------------------------------
    // Function: getInterfaceDirection()
    //-----------------------------------------------------------------------------
    static DirectionTypes::Direction getInterfaceDirection(QSharedPointer<BusInterface> busInterface_, 
        QSharedPointer<Component> component) 
    {
        DirectionTypes::Direction dir = DirectionTypes::DIRECTION_INVALID;

        foreach (QString const& portName, busInterface_->getPhysicalPortNames())
        {
            QSharedPointer<Port> port = component->getPort(portName);

            if (port != 0)
            {
                if (dir == DirectionTypes::DIRECTION_INVALID)
                {
                    dir = port->getDirection();
                }
                else if (dir != port->getDirection())
                {
                    return DirectionTypes::INOUT;
                }
            }
        }

        return DirectionTypes::DIRECTION_INVALID;
    }

}

#endif
