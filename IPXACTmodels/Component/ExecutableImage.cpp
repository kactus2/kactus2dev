//-----------------------------------------------------------------------------
// File: ExecutableImage.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 15.6.2023
//
// Description:
// Implementation for ipxact:executableImage element.
//-----------------------------------------------------------------------------

#include "ExecutableImage.h"

//-----------------------------------------------------------------------------
// Function: ExecutableImage::ExecutableImage()
//-----------------------------------------------------------------------------
ExecutableImage::ExecutableImage(QString const& name) :
    NameGroup(name),
    Extendable()
{

}

//-----------------------------------------------------------------------------
// Function: ExecutableImage::ExecutableImage()
//-----------------------------------------------------------------------------
ExecutableImage::ExecutableImage( const ExecutableImage& other ):
	NameGroup(other),
    Extendable(other)
{
    copyParameters(other);
    copyLanguageTools(other);
}


//-----------------------------------------------------------------------------
// Function: ExecutableImage::~ExecutableImage()
//-----------------------------------------------------------------------------
ExecutableImage::~ExecutableImage()
{

}

//-----------------------------------------------------------------------------
// Function: ExecutableImage::operator=()
//-----------------------------------------------------------------------------
ExecutableImage& ExecutableImage::operator=( const ExecutableImage& other )
{
	if (this != &other)
    {
		NameGroup::operator=(other);
        Extendable::operator=(other);

        parameters_->clear();
        copyParameters(other);

        copyLanguageTools(other);

	}
	return *this;
}

//-----------------------------------------------------------------------------
// Function: ExecutableImage::getImageId()
//-----------------------------------------------------------------------------
QString ExecutableImage::getImageId() const
{
    return imageId_;
}

//-----------------------------------------------------------------------------
// Function: ExecutableImage::setImageId()
//-----------------------------------------------------------------------------
void ExecutableImage::setImageId(QString const& id)
{
    imageId_ = id;
}

//-----------------------------------------------------------------------------
// Function: ExecutableImage::getImageType()
//-----------------------------------------------------------------------------
QString ExecutableImage::getImageType() const
{
    return imageType_;
}

//-----------------------------------------------------------------------------
// Function: ExecutableImage::setImageType()
//-----------------------------------------------------------------------------
void ExecutableImage::setImageType(QString const& type)
{
    imageType_ = type;
}

//-----------------------------------------------------------------------------
// Function: ExecutableImage::getParameters()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<Parameter> > > ExecutableImage::getParameters() const
{
    return parameters_;
}

//-----------------------------------------------------------------------------
// Function: ExecutableImage::setParameters()
//-----------------------------------------------------------------------------
void ExecutableImage::setParameters(QSharedPointer<QList<QSharedPointer<Parameter> > > parameters)
{
    parameters_ = parameters;
}

//-----------------------------------------------------------------------------
// Function: ExecutableImage::getLanguageTools()
//-----------------------------------------------------------------------------
QSharedPointer<LanguageTools> ExecutableImage::getLanguageTools() const
{
    return languageTools_;
}

//-----------------------------------------------------------------------------
// Function: ExecutableImage::setLanguageTools()
//-----------------------------------------------------------------------------
void ExecutableImage::setLanguageTools(QSharedPointer<LanguageTools> languageTools)
{
    languageTools_ = languageTools;
}

//-----------------------------------------------------------------------------
// Function: ExecutableImage::copyParameters()
//-----------------------------------------------------------------------------
void ExecutableImage::copyParameters(ExecutableImage const& other)
{
    for (auto const& parameter : *other.parameters_)
    {
        parameters_->append(QSharedPointer<Parameter>(new Parameter(*parameter)));
    }
}

//-----------------------------------------------------------------------------
// Function: ExecutableImage::copyLanguageTools()
//-----------------------------------------------------------------------------
void ExecutableImage::copyLanguageTools(ExecutableImage const& other)
{
    if (other.languageTools_ == nullptr)
    {
        languageTools_.clear();
    }
    else
    {
        languageTools_ = other.languageTools_->clone();
    }
}
