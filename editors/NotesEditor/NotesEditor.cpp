//-----------------------------------------------------------------------------
// File: NotesEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 6.5.2014
//
// Description:
// Editor for writing and viewing notes.
//-----------------------------------------------------------------------------

#include "NotesEditor.h"

#include <library\LibraryManager\vlnv.h>

#include <QRegExp>
#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: NotesEditor::NotesEditor()
//-----------------------------------------------------------------------------
NotesEditor::NotesEditor(QWidget *parent)
    : QWidget(parent),
    splitter_(this),
    editor_(this),
    preview_(this)
{
    preview_.setReadOnly(true);
    setupLayout();
}

//-----------------------------------------------------------------------------
// Function: NotesEditor::~NotesEditor()
//-----------------------------------------------------------------------------
NotesEditor::~NotesEditor()
{

}

//-----------------------------------------------------------------------------
// Function: NotesEditor::setupLayout()
//-----------------------------------------------------------------------------
void NotesEditor::setupLayout()
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(&splitter_);

    splitter_.setOrientation(Qt::Vertical);
    splitter_.addWidget(&editor_);
    splitter_.addWidget(&preview_);
}