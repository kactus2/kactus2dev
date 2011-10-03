//-----------------------------------------------------------------------------
// File: TextEditProvider.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti Määttä
// Date: 28.7.2011
//
// Description:
// Specialized edit provider for QPlainTextEdit.
//-----------------------------------------------------------------------------

#include "TextEditProvider.h"

//-----------------------------------------------------------------------------
// Function: TextEditProvider()
//-----------------------------------------------------------------------------
TextEditProvider::TextEditProvider(QPlainTextEdit& textEdit) :textEdit_(textEdit)
{
    connect(&textEdit_, SIGNAL(redoAvailable(bool)), this, SIGNAL(editStateChanged()));
    connect(&textEdit_, SIGNAL(undoAvailable(bool)), this, SIGNAL(editStateChanged()));
}

//-----------------------------------------------------------------------------
// Function: ~TextEditProvider()
//-----------------------------------------------------------------------------
TextEditProvider::~TextEditProvider()
{
}

//-----------------------------------------------------------------------------
// Function: undo()
//-----------------------------------------------------------------------------
void TextEditProvider::undo()
{
    textEdit_.undo();
    textEdit_.setFocus();
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void TextEditProvider::redo()
{
    textEdit_.redo();
    textEdit_.setFocus();
}

//-----------------------------------------------------------------------------
// Function: canUndo()
//-----------------------------------------------------------------------------
bool TextEditProvider::canUndo() const
{
    return textEdit_.document()->isUndoAvailable();
}

//-----------------------------------------------------------------------------
// Function: canRedo()
//-----------------------------------------------------------------------------
bool TextEditProvider::canRedo() const
{
    return textEdit_.document()->isRedoAvailable();
}
