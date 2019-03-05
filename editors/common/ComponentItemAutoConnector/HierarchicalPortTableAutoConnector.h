//-----------------------------------------------------------------------------
// File: HierarchicalPortTableAutoConnector.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 01.03.2019
//
// Description:
// Automatically connects ports of a component item and a top level component.
//-----------------------------------------------------------------------------

#ifndef HIERARCHICALPORTTABLEAUTOCONNECTOR_H
#define HIERARCHICALPORTTABLEAUTOCONNECTOR_H

#include <IPXACTmodels/common/DirectionTypes.h>

#include <editors/common/ComponentItemAutoConnector/PortTableAutoConnector.h>

//-----------------------------------------------------------------------------
//! Automatically connects ports of two components.
//-----------------------------------------------------------------------------
class HierarchicalPortTableAutoConnector : public PortTableAutoConnector
{
public:

    /*!
     *  The constructor.
     */
    HierarchicalPortTableAutoConnector() = default;

    /*!
     *  Destructor.
     */
    virtual ~HierarchicalPortTableAutoConnector() = default;

    // No copying. No assignments.
    HierarchicalPortTableAutoConnector(HierarchicalPortTableAutoConnector const& rhs) = delete;
    HierarchicalPortTableAutoConnector& operator=(HierarchicalPortTableAutoConnector const& rhs) = delete;

private:

    /*!
     *  Get connectible port directions for the selected port direction.
     *
     *      @param [in] portDirection   The selected port direction.
     *
     *      @return List of connectible port directions.
     */
    virtual QVector<DirectionTypes::Direction> getConnectableDirections(DirectionTypes::Direction portDirection)
        const;
};

//-----------------------------------------------------------------------------

#endif // HIERARCHICALPORTTABLEAUTOCONNECTOR_H
