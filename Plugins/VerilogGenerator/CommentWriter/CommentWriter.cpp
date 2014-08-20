//-----------------------------------------------------------------------------
// File: CommentWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 08.08.2014
//
// Description:
// Class for writing C/Verilog comments.
//-----------------------------------------------------------------------------

#include "CommentWriter.h"

//-----------------------------------------------------------------------------
// Function: CommentWriter::CommentWriter()
//-----------------------------------------------------------------------------
CommentWriter::CommentWriter(QString const& comment): comment_(comment), 
    lineLength_(DEFAULT_LINE_LENGHT), 
    indentSize_(0)
{

}

//-----------------------------------------------------------------------------
// Function: CommentWriter::~CommentWriter()
//-----------------------------------------------------------------------------
CommentWriter::~CommentWriter()
{

}

//-----------------------------------------------------------------------------
// Function: CommentWriter::write()
//-----------------------------------------------------------------------------
void CommentWriter::write(QTextStream& output) const
{
    QString indent = QString(" ").repeated(indentSize_);

    foreach(QString line, comment_.split("\n"))
    {
        while (line.length() > 0)
        {
            int lineEnd = line.lastIndexOf(" ", lineLength_);
            if (lineEnd == -1)
            {
                lineEnd = line.indexOf(" ", lineLength_);
                if (lineEnd == -1)
                {
                    lineEnd = line.length();
                }
            }
            
            output << indent << "// " << line.left(lineEnd) << endl;
            line = line.remove(0, lineEnd + 1);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: CommentWriter::setLineLength()
//-----------------------------------------------------------------------------
void CommentWriter::setLineLength(int maxLength)
{
    lineLength_ = maxLength;        
}

//-----------------------------------------------------------------------------
// Function: CommentWriter::setIndent()
//-----------------------------------------------------------------------------
void CommentWriter::setIndent(int size)
{
    indentSize_ = size;
}