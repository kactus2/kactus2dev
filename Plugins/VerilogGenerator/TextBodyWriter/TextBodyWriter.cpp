//-----------------------------------------------------------------------------
// File: TextBodyWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Janne Virtanen
// Date: 09.06.2016
//
// Description:
// Class for writing text body.
//-----------------------------------------------------------------------------

#include "TextBodyWriter.h"

#include <QStringList>

//-----------------------------------------------------------------------------
// Function: TextBodyWriter::TextBodyWriter()
//-----------------------------------------------------------------------------
TextBodyWriter::TextBodyWriter(QString const& text): text_(text)
{

}

//-----------------------------------------------------------------------------
// Function: TextBodyWriter::~TextBodyWriter()
//-----------------------------------------------------------------------------
TextBodyWriter::~TextBodyWriter()
{

}

//-----------------------------------------------------------------------------
// Function: TextBodyWriter::write()
//-----------------------------------------------------------------------------
void TextBodyWriter::write(QTextStream& output) const
{
    output << text_;
}