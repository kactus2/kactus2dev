//-----------------------------------------------------------------------------
// File: Kactus2Placeholder.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 19.5.2014
//
// Description:
// Kactus2 vendor extension for single values.
//-----------------------------------------------------------------------------

#include "Kactus2Placeholder.h"

//-----------------------------------------------------------------------------
// Function: Kactus2Flag::Kactus2Flag()
//-----------------------------------------------------------------------------
Kactus2Placeholder::Kactus2Placeholder(QString name):
    name_(name)
{

}

//-----------------------------------------------------------------------------
// Function: Kactus2Flag::~Kactus2Flag()
//-----------------------------------------------------------------------------
Kactus2Placeholder::~Kactus2Placeholder()
{

}

//-----------------------------------------------------------------------------
// Function: Kactus2Extension::write()
//-----------------------------------------------------------------------------
void Kactus2Placeholder::write(QXmlStreamWriter& writer) const
{
    writer.writeEmptyElement(name_);
}
