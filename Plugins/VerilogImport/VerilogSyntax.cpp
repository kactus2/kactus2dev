//-----------------------------------------------------------------------------
// File: VerilogSyntax.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Janne Virtanen
// Date: 20.10.2016
//
// Description:
// Common elements of the Verilog language.
//-----------------------------------------------------------------------------

#include "VerilogSyntax.h"

#include <QTextStream>

//-----------------------------------------------------------------------------
// Function: VerilogSyntax::cullStrayComments()
//-----------------------------------------------------------------------------
QString VerilogSyntax::cullStrayComments(QString const& input)
{
    // Make a copy, remove all multiline comments as well as lines that are purely comments.
    QString inspectWithoutComments = input;
    inspectWithoutComments.remove(VerilogSyntax::MULTILINE_COMMENT);
    inspectWithoutComments.remove(VerilogSyntax::COMMENTLINE);

    return inspectWithoutComments;
}

//-----------------------------------------------------------------------------
// Function: VerilogSyntax::findModuleHeader()
//-----------------------------------------------------------------------------
QPair<int,int> VerilogSyntax::findModuleDeclaration(QString const& input)
{
    // The return value.
    QPair<int,int> retval;

    // Find the beginning of the module header.
    int moduleDeclarationBeginIndex = input.indexOf(VerilogSyntax::MODULE_KEY_WORD);

    // Its shall be the first return value.
    retval.first = moduleDeclarationBeginIndex;

    // Make a copy of the input.
    QString inspect = input;

    // The ending position of the module header.
    int moduleDeclarationEndIndex = -1;
    
    // We decide it is easier to read in stream format.
    QTextStream sourceStream(&inspect);
    // The last stream position.
    int streamPos = -1;

    // The alternative syntaxes for comment starts.
    QRegularExpression comment("(////)|(//*)");

    while (!sourceStream.atEnd())
    {
        // Remember the position before this line.
        streamPos = sourceStream.pos();

        // Read the next line.
        const QString currentLine = sourceStream.readLine();

        // Find position of any comment start in the line, as well as declaration ending.
        int commentPosition = currentLine.indexOf(comment);
        int possibleEnding = currentLine.indexOf(");");

        // If the declaration ending is actually before the comment, it is now valid.
        if (commentPosition != -1 && commentPosition < possibleEnding)
        {
            continue;
        }

        // If the ending is detected, we are done looping.
        if (possibleEnding != -1)
        {
            moduleDeclarationEndIndex = possibleEnding;
            break;
        }
    }

    // No ending for module declaration detected: Set return value as -1.
    if (moduleDeclarationEndIndex == -1)
    {
        retval.second = -1;
        return retval;
    }

    // The position for the modue declaration ending = position on the line + stream position before the line.
    moduleDeclarationEndIndex = moduleDeclarationEndIndex + streamPos;
    // Set it as a return value.
    retval.second = moduleDeclarationEndIndex -  moduleDeclarationBeginIndex;

    return retval;
}