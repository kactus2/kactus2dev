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
        DirectionTypes::Direction direction = DirectionTypes::DIRECTION_INVALID;

        if (busInterface_->getAbstractionTypes() && !busInterface_->getAbstractionTypes()->isEmpty())
        {
            if (busInterface_->getAbstractionTypes()->size() != 1)
            {
                direction = DirectionTypes::INOUT;
            }

            else
            {
                QSharedPointer<AbstractionType> abstraction = busInterface_->getAbstractionTypes()->first();
                foreach (QString portName, abstraction->getPhysicalPortNames())
                {
                    QSharedPointer<Port> port = component->getPort(portName);
                    if (port != 0)
                    {
                        if (direction == DirectionTypes::DIRECTION_INVALID)
                        {
                            direction = port->getDirection();
                        }
                        else if (direction != port->getDirection())
                        {
                            return DirectionTypes::INOUT;
                        }
                    }
                }
            }
        }

        return direction;
    }

}

#endif
