//-----------------------------------------------------------------------------
// File: NotesStack.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 6.5.2014
//
// Description:
// Stacks NotesEditors for different VLNVs and contexts in a single widget.
//-----------------------------------------------------------------------------

#include "NotesStack.h"
#include "NotesEditor.h"

#include <library\LibraryManager\vlnv.h>

#include <QRegExp>
#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: NotesStack::NotesStack()
//-----------------------------------------------------------------------------
NotesStack::NotesStack(QWidget *parent)
    : QStackedWidget(parent),
    context_(),
    vlnv_(),
    vlnvPaths_(),
    vlnvEditors_()
{
    
}

//-----------------------------------------------------------------------------
// Function: NotesStack::~NotesStack()
//-----------------------------------------------------------------------------
NotesStack::~NotesStack()
{

}


//-----------------------------------------------------------------------------
// Function: NotesStack::onVLNVChanged()
//-----------------------------------------------------------------------------
void NotesStack::onVLNVChanged(VLNV const& vlnv, QString const& path)
{
    vlnv_ = vlnv;
    saveVLNVpath(vlnv, path);

    changeActiveEditor(); 
}

//-----------------------------------------------------------------------------
// Function: NotesStack::onContextChanged()
//-----------------------------------------------------------------------------
void NotesStack::onContextChanged(QString const& context)
{
    context_ = formatContext(context);

    if (vlnv_.isValid())
    {
        changeActiveEditor();
    }    
}

//-----------------------------------------------------------------------------
// Function: NotesStack::onVLNVClosed()
//-----------------------------------------------------------------------------
void NotesStack::onVLNVClosed(VLNV const& vlnv)
{
    closeEditorsForVLNV(vlnv);
}

//-----------------------------------------------------------------------------
// Function: NotesStack::savePathForVLNV()
//-----------------------------------------------------------------------------
void NotesStack::saveVLNVpath(VLNV const& vlnv, QString const& path)
{
    if (!vlnvPaths_.contains(vlnv))
    {
        QString dirPath = formatPathForVlnv(path, vlnv);
        vlnvPaths_.insert(vlnv, dirPath);      
    }
}

//-----------------------------------------------------------------------------
// Function: NotesStack::formatPathForVlnv()
//-----------------------------------------------------------------------------
QString NotesStack::formatPathForVlnv(QString const& vlnvPath, VLNV const& vlnv)
{
    QString path = vlnvPath;
    QRegExp filename = QRegExp("(\\w+\\.?)*$");

    path.remove(filename);
    path.append("notes/");
    path.append(vlnv.toString("/"));

    return path;
}

//-----------------------------------------------------------------------------
// Function: NotesStack::changeActiveEditor()
//-----------------------------------------------------------------------------
void NotesStack::changeActiveEditor()
{
    if (noOpenEditor())
    {
        createEditor();
    }

    showEditor();
}

//-----------------------------------------------------------------------------
// Function: NotesStack::noOpenEditor()
//-----------------------------------------------------------------------------
bool NotesStack::noOpenEditor()
{
    return !vlnvEditors_.value(vlnv_).contains(context_);
}

//-----------------------------------------------------------------------------
// Function: NotesStack::createEditor()
//-----------------------------------------------------------------------------
void NotesStack::createEditor()
{
    NotesEditor* page = new NotesEditor(this);    
    int index = addWidget(page);

    Q_ASSERT(!vlnvEditors_.value(vlnv_).contains(context_));
    
    addEditorForVlnv(index);
}

//-----------------------------------------------------------------------------
// Function: NotesStack::addEditorForVlnv()
//-----------------------------------------------------------------------------
void NotesStack::addEditorForVlnv(int stackIndex)
{

    if (!vlnvEditors_.contains(vlnv_))
    {
        vlnvEditors_.insert(vlnv_, QMap<QString, QWidget*>());
    }

    if (!vlnvEditors_[vlnv_].contains(context_))
    {
        vlnvEditors_[vlnv_].insert(context_, widget(stackIndex));
    }

}

//-----------------------------------------------------------------------------
// Function: NotesStack::showEditor()
//-----------------------------------------------------------------------------
void NotesStack::showEditor()
{
    setCurrentWidget(vlnvEditors_.value(vlnv_).value(context_));
}

//-----------------------------------------------------------------------------
// Function: NotesStack::closeEditorsForVLNV()
//-----------------------------------------------------------------------------
void NotesStack::closeEditorsForVLNV(VLNV const& vlnv)
{
    foreach(QWidget* vlnvEditor, vlnvEditors_.value(vlnv).values())
    {
        removeWidget(vlnvEditor);
        delete vlnvEditor;
    }

    vlnvEditors_.remove(vlnv);

    vlnvPaths_.remove(vlnv);
}

//-----------------------------------------------------------------------------
// Function: NotesStack::formatContext()
//-----------------------------------------------------------------------------
QString NotesStack::formatContext(QString const& context)
{
    QString formatted = context;
    QRegExp path = QRegExp("(\\w*/)*");

    formatted.remove(path);
    
    return formatted;
}
