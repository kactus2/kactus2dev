//-----------------------------------------------------------------------------
// File: LibraryMock.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 06.08.2014
//
// Description:
// <Short description of the class/file contents>
//-----------------------------------------------------------------------------

#include "LibraryMock.h"

#include <IPXACTmodels/librarycomponent.h>
#include <IPXACTmodels/vlnv.h>

//-----------------------------------------------------------------------------
// Function: LibraryMock::LibraryMock()
//-----------------------------------------------------------------------------
LibraryMock::LibraryMock(QObject* parent):
    QObject(parent),
        components_(),
        paths_()
{

}

//-----------------------------------------------------------------------------
// Function: LibraryMock::()
//-----------------------------------------------------------------------------
LibraryMock::~LibraryMock()
{

}

//-----------------------------------------------------------------------------
// Function: LibraryMock::addComponent()
//-----------------------------------------------------------------------------
void LibraryMock::addComponent(QSharedPointer<LibraryComponent> component)
{
    components_.insert(*component->getVlnv(), component);
}

//-----------------------------------------------------------------------------
// Function: LibraryMock::clear()
//-----------------------------------------------------------------------------
void LibraryMock::clear()
{
    components_.clear();
}

//-----------------------------------------------------------------------------
// Function: LibraryMock::getModel()
//-----------------------------------------------------------------------------
QSharedPointer<LibraryComponent> LibraryMock::getModel(const VLNV& vlnv)
{
    return components_.value(vlnv, QSharedPointer<LibraryComponent>(0));
}

//-----------------------------------------------------------------------------
// Function: LibraryMock::getModelReadOnly()
//-----------------------------------------------------------------------------
QSharedPointer<LibraryComponent const> LibraryMock::getModelReadOnly(const VLNV& vlnv)
{
    return components_.value(vlnv, QSharedPointer<LibraryComponent>(0));
}

//-----------------------------------------------------------------------------
// Function: LibraryMock::contains()
//-----------------------------------------------------------------------------
bool LibraryMock::contains(const VLNV& vlnv)
{
    return components_.contains(vlnv);
}

//-----------------------------------------------------------------------------
// Function: LibraryMock::getPath()
//-----------------------------------------------------------------------------
const QString LibraryMock::getPath(const VLNV& vlnv) const
{
    return paths_.value(vlnv);
}

//-----------------------------------------------------------------------------
// Function: LibraryMock::getDirectoryPath()
//-----------------------------------------------------------------------------
QString LibraryMock::getDirectoryPath(const VLNV& /*vlnv*/) const
{
    return QString();
}

//-----------------------------------------------------------------------------
// Function: LibraryMock::writeModelToFile()
//-----------------------------------------------------------------------------
bool LibraryMock::writeModelToFile(const QString path, QSharedPointer<LibraryComponent> model, bool /*printErrors*/)
{
    paths_.insert(*model->getVlnv(), path);
    return true;
}

//-----------------------------------------------------------------------------
// Function: LibraryMock::writeModelToFile()
//-----------------------------------------------------------------------------
bool LibraryMock::writeModelToFile(QSharedPointer<LibraryComponent> /*model*/, bool /*printErrors*/)
{
    return false;
}

//-----------------------------------------------------------------------------
// Function: LibraryMock::searchForIPXactFiles()
//-----------------------------------------------------------------------------
void LibraryMock::searchForIPXactFiles()
{
    
}

//-----------------------------------------------------------------------------
// Function: LibraryMock::getNeededVLNVs()
//-----------------------------------------------------------------------------
void LibraryMock::getNeededVLNVs(const VLNV& /*vlnv*/, QList<VLNV>& /*list*/)
{
    
}

//-----------------------------------------------------------------------------
// Function: LibraryMock::getDependencyFiles()
//-----------------------------------------------------------------------------
void LibraryMock::getDependencyFiles(const VLNV& /*vlnv*/, QStringList& /*list*/)
{

}

//-----------------------------------------------------------------------------
// Function: LibraryMock::getHierarchicalDependencyFiles()
//-----------------------------------------------------------------------------
void LibraryMock::getHierarchicalDependencyFiles(const VLNV& /*vlnv*/, QStringList& /*list*/)
{
    
}

//-----------------------------------------------------------------------------
// Function: LibraryMock::getTreeRoot()
//-----------------------------------------------------------------------------
LibraryItem const* LibraryMock::getTreeRoot() const
{
    return 0;
}

//-----------------------------------------------------------------------------
// Function: LibraryMock::getDocumentType()
//-----------------------------------------------------------------------------
VLNV::IPXactType LibraryMock::getDocumentType(const VLNV& vlnv)
{
    if ( components_.keys().contains(vlnv) )
    {
        return vlnv.getType();
    }
    else
    {
        return VLNV::INVALID;
    }
}

//-----------------------------------------------------------------------------
// Function: LibraryMock::referenceCount()
//-----------------------------------------------------------------------------
int LibraryMock::referenceCount(const VLNV& /*vlnv*/) const
{
    return 0;
}

//-----------------------------------------------------------------------------
// Function: LibraryMock::getOwners()
//-----------------------------------------------------------------------------
int LibraryMock::getOwners(QList<VLNV>& /*list*/, const VLNV& /*vlnvToSearch*/) const
{
    return 0;
}

//-----------------------------------------------------------------------------
// Function: LibraryMock::getChildren()
//-----------------------------------------------------------------------------
int LibraryMock::getChildren(QList<VLNV>& /*list*/, const VLNV& /*vlnvToSearch*/) const
{
    return 0;
}

//-----------------------------------------------------------------------------
// Function: LibraryMock::getDesignVLNV()
//-----------------------------------------------------------------------------
VLNV LibraryMock::getDesignVLNV(const VLNV& /*hierarchyRef*/)
{
    return VLNV();
}

//-----------------------------------------------------------------------------
// Function: LibraryMock::getDesign()
//-----------------------------------------------------------------------------
QSharedPointer<Design> LibraryMock::getDesign(const VLNV& hierarchyRef)
{
    return components_.value(hierarchyRef, QSharedPointer<LibraryComponent>(0)).dynamicCast<Design>();
}

//-----------------------------------------------------------------------------
// Function: LibraryMock::isValid()
//-----------------------------------------------------------------------------
bool LibraryMock::isValid(const VLNV& /*vlnv*/)
{
    return false;
}

//-----------------------------------------------------------------------------
// Function: LibraryMock::onCheckLibraryIntegrity()
//-----------------------------------------------------------------------------
void LibraryMock::onCheckLibraryIntegrity()
{
    Q_ASSERT(false);
}

//-----------------------------------------------------------------------------
// Function: LibraryMock::onSelectionChanged()
//-----------------------------------------------------------------------------
void LibraryMock::onSelectionChanged(const VLNV& /*vlnv*/)
{
    Q_ASSERT(false);
}

//-----------------------------------------------------------------------------
// Function: LibraryMock::onClearSelection()
//-----------------------------------------------------------------------------
void LibraryMock::onClearSelection()
{
    Q_ASSERT(false);
}

//-----------------------------------------------------------------------------
// Function: LibraryMock::onEditItem()
//-----------------------------------------------------------------------------
void LibraryMock::onEditItem(const VLNV& /*vlnv*/)
{
    Q_ASSERT(false);
}

//-----------------------------------------------------------------------------
// Function: LibraryMock::onOpenDesign()
//-----------------------------------------------------------------------------
void LibraryMock::onOpenDesign(const VLNV& /*vlnv*/)
{
    Q_ASSERT(false);
}

//-----------------------------------------------------------------------------
// Function: LibraryMock::onCreateNewItem()
//-----------------------------------------------------------------------------
void LibraryMock::onCreateNewItem(const VLNV& /*vlnv*/)
{
    Q_ASSERT(false);
}

//-----------------------------------------------------------------------------
// Function: LibraryMock::onCreateDesign()
//-----------------------------------------------------------------------------
void LibraryMock::onCreateDesign(const VLNV& /*vlnv*/)
{
    Q_ASSERT(false);
}

//-----------------------------------------------------------------------------
// Function: LibraryMock::removeObject()
//-----------------------------------------------------------------------------
void LibraryMock::removeObject(const VLNV& /*vlnv*/)
{
    Q_ASSERT(false);
}

//-----------------------------------------------------------------------------
// Function: LibraryMock::removeObjects()
//-----------------------------------------------------------------------------
void LibraryMock::removeObjects(const QList<VLNV>& /*vlnvList*/)
{
    Q_ASSERT(false);
}

//-----------------------------------------------------------------------------
// Function: LibraryMock::beginSave()
//-----------------------------------------------------------------------------
void LibraryMock::beginSave()
{
    Q_ASSERT(false);
}

//-----------------------------------------------------------------------------
// Function: LibraryMock::endSave()
//-----------------------------------------------------------------------------
void LibraryMock::endSave()
{
   Q_ASSERT(false);
}
