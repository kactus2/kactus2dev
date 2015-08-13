//-----------------------------------------------------------------------------
// File: CellSpecification.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 13.08.2015
//
// Description:
// Implementation for ipxact:cellSpecification.
//-----------------------------------------------------------------------------

#include "CellSpecification.h"

//-----------------------------------------------------------------------------
// Function: CellSpecification::CellSpecification()
//-----------------------------------------------------------------------------
CellSpecification::CellSpecification() : cellStrength_(UNKNOWN), function_(), cellClass_(NO_CLASS)
{

}

//-----------------------------------------------------------------------------
// Function: CellSpecification::CellSpecification()
//-----------------------------------------------------------------------------
CellSpecification::CellSpecification(CellSpecification const& other) :
cellStrength_(other.cellStrength_),
    function_(other.function_),
    cellClass_(other.cellClass_)
{

}

//-----------------------------------------------------------------------------
// Function: CellSpecification::~CellSpecification()
//-----------------------------------------------------------------------------
CellSpecification::~CellSpecification()
{

}

//-----------------------------------------------------------------------------
// Function: CellSpecification::setCellStrength()
//-----------------------------------------------------------------------------
void CellSpecification::setCellStrength(CellStrengthType strength)
{
    cellStrength_ = strength;
}

//-----------------------------------------------------------------------------
// Function: CellSpecification::getCellStrength()
//-----------------------------------------------------------------------------
CellSpecification::CellStrengthType CellSpecification::getCellStrength() const
{
    return cellStrength_;
}

//-----------------------------------------------------------------------------
// Function: CellSpecification::setCellFunction()
//-----------------------------------------------------------------------------
void CellSpecification::setCellFunction(QString const& function)
{
    cellClass_ = NO_CLASS;
    function_ = function;
}

//-----------------------------------------------------------------------------
// Function: CellSpecification::getCellFunction()
//-----------------------------------------------------------------------------
QString CellSpecification::getCellFunction() const
{
    return function_;
}

//-----------------------------------------------------------------------------
// Function: CellSpecification::setCellClass()
//-----------------------------------------------------------------------------
void CellSpecification::setCellClass(CellClassType cellClass)
{
    function_.clear();
    cellClass_ = cellClass;
}

//-----------------------------------------------------------------------------
// Function: CellSpecification::getCellClass()
//-----------------------------------------------------------------------------
CellSpecification::CellClassType CellSpecification::getCellClass() const
{
    return cellClass_;
}
