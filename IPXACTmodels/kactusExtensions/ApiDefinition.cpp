//-----------------------------------------------------------------------------
// File: ApiDefinition.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 3.4.2012
//
// Description:
// Class which encapsulates the handling of custom API definition object.
//-----------------------------------------------------------------------------

#include "ApiDefinition.h"
#include "ApiFunction.h"

#include <IPXACTmodels/common/GenericVendorExtension.h>

//-----------------------------------------------------------------------------
// Function: ApiDefinition::ApiDefinition()
//-----------------------------------------------------------------------------
ApiDefinition::ApiDefinition(VLNV const& vlnv) : Document(vlnv),
                                                 language_(),
                                                 comDefRef_(),
                                                 dataTypes_(new QStringList),
												 functions_(new QList<QSharedPointer<ApiFunction> >)
{

}

//-----------------------------------------------------------------------------
// Function: ApiDefinition::ApiDefinition()
//-----------------------------------------------------------------------------
ApiDefinition::ApiDefinition(ApiDefinition const& rhs) : Document(rhs),
                                                         language_(rhs.language_),
                                                         comDefRef_(rhs.comDefRef_),
                                                         dataTypes_(new QStringList(*rhs.dataTypes_)),
                                                         functions_(new QList<QSharedPointer<ApiFunction> >)
{
    foreach (QSharedPointer<ApiFunction> func, *rhs.functions_)
    {
        functions_->append(QSharedPointer<ApiFunction>(new ApiFunction(*func.data())));
    }
}

//-----------------------------------------------------------------------------
// Function: ApiDefinition::~ApiDefinition()
//-----------------------------------------------------------------------------
ApiDefinition::~ApiDefinition()
{

}

//-----------------------------------------------------------------------------
// Function: ApiDefinition::clone()
//-----------------------------------------------------------------------------
QSharedPointer<Document> ApiDefinition::clone() const
{
    return QSharedPointer<Document>(new ApiDefinition(*this));
}

//-----------------------------------------------------------------------------
// Function: ApiDefinition::getDependentFiles()
//-----------------------------------------------------------------------------
QStringList ApiDefinition::getDependentFiles() const
{
    return QStringList();
}

//-----------------------------------------------------------------------------
// Function: ApiDefinition::getDependentVLNVs()
//-----------------------------------------------------------------------------
QList<VLNV> ApiDefinition::getDependentVLNVs() const
{
    QList<VLNV> vlnvs;

    if (comDefRef_.isValid())
    {
        vlnvs.append(comDefRef_);
    }

    return vlnvs;
}

//-----------------------------------------------------------------------------
// Function: ApiDefinition::setLanguage()
//-----------------------------------------------------------------------------
void ApiDefinition::setLanguage(QString const& language)
{
    language_ = language;
}

//-----------------------------------------------------------------------------
// Function: ApiDefinition::addDataType()
//-----------------------------------------------------------------------------
void ApiDefinition::addDataType(QString const& type)
{
    if (!dataTypes_->contains(type))
    {
        dataTypes_->append(type);
    }
}

//-----------------------------------------------------------------------------
// Function: ApiDefinition::removeDataType()
//-----------------------------------------------------------------------------
void ApiDefinition::removeDataType(QString const& type)
{
    dataTypes_->removeOne(type);
}

//-----------------------------------------------------------------------------
// Function: ApiDefinition::setDataTypes()
//-----------------------------------------------------------------------------
void ApiDefinition::setDataTypes(QStringList const& types)
{
    dataTypes_ = QSharedPointer<QStringList>( new QStringList( types ) );
}

//-----------------------------------------------------------------------------
// Function: ApiDefinition::removeFunctions()
//-----------------------------------------------------------------------------
void ApiDefinition::removeFunctions()
{
    functions_->clear();
}

//-----------------------------------------------------------------------------
// Function: ApiDefinition::getLanguage()
//-----------------------------------------------------------------------------
QString const& ApiDefinition::getLanguage() const
{
    return language_;
}

//-----------------------------------------------------------------------------
// Function: ApiDefinition::getDataTypes()
//-----------------------------------------------------------------------------
QSharedPointer<QStringList> ApiDefinition::getDataTypes() const
{
    return dataTypes_;
}

//-----------------------------------------------------------------------------
// Function: Document::getFunctions()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<ApiFunction> > > ApiDefinition::getFunctions() const
{
	return functions_;
}

//-----------------------------------------------------------------------------
// Function: ApiDefinition::setComDefinitionRef()
//-----------------------------------------------------------------------------
void ApiDefinition::setComDefinitionRef(VLNV const& vlnv)
{
    comDefRef_ = vlnv;
}

//-----------------------------------------------------------------------------
// Function: ApiDefinition::getComDefinitionRef()
//-----------------------------------------------------------------------------
VLNV const& ApiDefinition::getComDefinitionRef() const
{
    return comDefRef_;
}
