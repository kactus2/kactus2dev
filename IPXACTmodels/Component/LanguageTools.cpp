//-----------------------------------------------------------------------------
// File: LanguageTools.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 15.6.2023
//
// Description:
// Implementation of the ipxact:languageTools element.
//-----------------------------------------------------------------------------

#include "LanguageTools.h"

//-----------------------------------------------------------------------------
// Function: LanguageTools::LanguageTools()
//-----------------------------------------------------------------------------
LanguageTools::LanguageTools()
{

}

//-----------------------------------------------------------------------------
// Function: LanguageTools::LanguageTools()
//-----------------------------------------------------------------------------
LanguageTools::LanguageTools(const LanguageTools& other) :
    NameGroup(other),
    Extendable(other)
{
    copyFileBuilders(other);
}

//-----------------------------------------------------------------------------
// Function: LanguageTools::operator=()
//-----------------------------------------------------------------------------
LanguageTools& LanguageTools::operator=(const LanguageTools& other)
{
    if (this != &other)
    {
        NameGroup::operator=(other);
        Extendable::operator=(other);

        fileBuilders_->clear();
        copyFileBuilders(other);
    }

    return *this;
}

//-----------------------------------------------------------------------------
// Function: LanguageTools::~LanguageTools()
//-----------------------------------------------------------------------------
LanguageTools::~LanguageTools()
{

}

//-----------------------------------------------------------------------------
// Function: LanguageTools::clone()
//-----------------------------------------------------------------------------
QSharedPointer<LanguageTools> LanguageTools::clone() const
{
    QSharedPointer<LanguageTools> copy(new LanguageTools(*this));
    return copy;
}

//-----------------------------------------------------------------------------
// Function: LanguageTools::getFileBuilders()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<FileBuilder> > > LanguageTools::getFileBuilders() const
{
    return fileBuilders_;
}

//-----------------------------------------------------------------------------
// Function: LanguageTools::setDefaultFileBuilders()
//-----------------------------------------------------------------------------
void LanguageTools::setFileBuilders(
    QSharedPointer<QList<QSharedPointer<FileBuilder> > > newFileBuilders)
{
    fileBuilders_ = newFileBuilders;
}

//-----------------------------------------------------------------------------
// Function: LanguageTools::getLinker()
//-----------------------------------------------------------------------------
QString LanguageTools::getLinker() const
{
    return linker_;
}

//-----------------------------------------------------------------------------
// Function: LanguageTools::setLinker()
//-----------------------------------------------------------------------------
void LanguageTools::setLinker(QString const& linker)
{
    linker_ = linker;
}

//-----------------------------------------------------------------------------
// Function: LanguageTools::getLinkerFlags()
//-----------------------------------------------------------------------------
QString LanguageTools::getLinkerFlags() const
{
    return linkerFlags_;
}

//-----------------------------------------------------------------------------
// Function: LanguageTools::setLinkerFlags()
//-----------------------------------------------------------------------------
void LanguageTools::setLinkerFlags(QString const& flags)
{
    linkerFlags_ = flags;
}

//-----------------------------------------------------------------------------
// Function: LanguageTools::getLinkerCommandFile()
//-----------------------------------------------------------------------------
QSharedPointer<LanguageTools::LanguageTools::LinkerCommandFile> LanguageTools::getLinkerCommandFile() const
{
    return linkerCommandFile_;
}

//-----------------------------------------------------------------------------
// Function: LanguageTools::copyFileBuilders()
//-----------------------------------------------------------------------------
void LanguageTools::copyFileBuilders(const LanguageTools& other) const
{
    for (QSharedPointer<FileBuilder> fileBuilder : *other.fileBuilders_)
    {
        QSharedPointer<FileBuilder> copy = QSharedPointer<FileBuilder>(new FileBuilder(*fileBuilder));
        fileBuilders_->append(copy);
    }
}
