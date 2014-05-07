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
    path_(),
    vlnv_(),
    editors_(),
    editorsForVlnv_(),
    vlnvPaths_()
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
    context_ = context;

    if (vlnv_.isValid())
    {
        changeActiveEditor();
    }    
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
    adjustPath();

    if (noOpenEditor())
    {
        createEditor();
    }

    showEditor();
}

//-----------------------------------------------------------------------------
// Function: NotesStack::adjustPath()
//-----------------------------------------------------------------------------
void NotesStack::adjustPath()
{
    path_ = vlnvPaths_.value(vlnv_) + "/" + context_;
}

//-----------------------------------------------------------------------------
// Function: NotesStack::noOpenEditor()
//-----------------------------------------------------------------------------
bool NotesStack::noOpenEditor()
{
    return !editors_.contains(path_);
}

//-----------------------------------------------------------------------------
// Function: NotesStack::createEditor()
//-----------------------------------------------------------------------------
void NotesStack::createEditor()
{
    NotesEditor* page = new NotesEditor(this);
    int index = addWidget(page);

    Q_ASSERT(!editors_.contains(path_));
    editors_.append(path_);  
    
    addEditorForVlnv(index);
}

//-----------------------------------------------------------------------------
// Function: NotesStack::addEditorForVlnv()
//-----------------------------------------------------------------------------
void NotesStack::addEditorForVlnv(int stackIndex)
{
    if (!editorsForVlnv_.contains(vlnv_))
    {
        editorsForVlnv_.insert(vlnv_, QList<int>());
    }

    editorsForVlnv_[vlnv_].append(stackIndex);
}

//-----------------------------------------------------------------------------
// Function: NotesStack::showEditor()
//-----------------------------------------------------------------------------
void NotesStack::showEditor()
{
    setCurrentIndex(editors_.indexOf(path_));
}
