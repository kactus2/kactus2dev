//-----------------------------------------------------------------------------
// File: Document.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 05.08.2015
//
// Description:
// Base class for IP-XACT documents e.g. component and design.
//-----------------------------------------------------------------------------

#include "Document.h"

#include <IPXACTmodels/GenericVendorExtension.h>
#include <IPXACTmodels/generaldeclarations.h>
#include <IPXACTmodels/XmlUtils.h>
#include <IPXACTmodels/vlnv.h>

#include <QString>
#include <QSharedPointer>
#include <QObject>
#include <QList>
#include <QStringList>

//-----------------------------------------------------------------------------
// Function: Document::Document()
//-----------------------------------------------------------------------------
Document::Document():
vlnv_(), 
description_(),
kactus2Attributes_(),
topComments_(),
parameters_(new QList<QSharedPointer<Parameter> >()),
assertions_(new QList<QSharedPointer<Assertion> >()),
vendorExtensions_(new QList<QSharedPointer<VendorExtension> >())
{
	vlnv_ = VLNV();
}

//-----------------------------------------------------------------------------
// Function: Document::Document()
//-----------------------------------------------------------------------------
Document::Document(const VLNV &vlnv):
vlnv_(), 
description_(),
kactus2Attributes_(),
topComments_(),
parameters_(new QList<QSharedPointer<Parameter> >()),
assertions_(new QList<QSharedPointer<Assertion> >()),
vendorExtensions_(new QList<QSharedPointer<VendorExtension> >())
{
    vlnv_ = vlnv;
}

//-----------------------------------------------------------------------------
// Function: Document::Document()
//-----------------------------------------------------------------------------
Document::Document(Document const& other):
vlnv_(other.vlnv_),
description_(other.description_),
kactus2Attributes_(other.kactus2Attributes_),
topComments_(other.topComments_),
parameters_(new QList<QSharedPointer<Parameter> >()),
assertions_(new QList<QSharedPointer<Assertion> >()),
vendorExtensions_(new QList<QSharedPointer<VendorExtension> >())
{
    copyParameters(other);
    copyAssertions(other);
    copyVendorExtensions(other);
}

//-----------------------------------------------------------------------------
// Function: Document::operator=()
//-----------------------------------------------------------------------------
Document & Document::operator=( const Document &other )
{
	if (this != &other)
    {
        if (!vlnv_.isEmpty())
        {
            vlnv_ = other.vlnv_;
        }
		else
        {
			vlnv_ = VLNV();
        }

		description_ = other.description_;
		kactus2Attributes_ = other.kactus2Attributes_;
		topComments_ = other.topComments_;

        copyParameters(other);
        copyAssertions(other);
        copyVendorExtensions(other);
	}
	return *this;
}

//-----------------------------------------------------------------------------
// Function: Document::~Document()
//-----------------------------------------------------------------------------
Document::~Document()
{

}

//-----------------------------------------------------------------------------
// Function: Document::getVlnv()
//-----------------------------------------------------------------------------
VLNV Document::getVlnv() const
{
	return vlnv_;
}

//-----------------------------------------------------------------------------
// Function: Document::setVlnv()
//-----------------------------------------------------------------------------
void Document::setVlnv(VLNV const& vlnv)
{
	vlnv_ = vlnv;
}

//-----------------------------------------------------------------------------
// Function: Document::getDescription()
//-----------------------------------------------------------------------------
QString Document::getDescription() const
{
    return description_;
}

//-----------------------------------------------------------------------------
// Function: Document::setDescription()
//-----------------------------------------------------------------------------
void Document::setDescription(QString const& description)
{
	description_ = description;
}

//-----------------------------------------------------------------------------
// Function: Document::getParameters()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<Parameter> > > Document::getParameters() const
{
    return parameters_;
}

//-----------------------------------------------------------------------------
// Function: DesignConfiguration::getAssertions()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<Assertion> > > Document::getAssertions() const
{
    return assertions_;
}

//-----------------------------------------------------------------------------
// Function: LibraryComponent::getVendorExtensions()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<VendorExtension> > > Document::getVendorExtensions() const
{
    return vendorExtensions_;
}

//-----------------------------------------------------------------------------
// Function: Document::setTopComments()
//-----------------------------------------------------------------------------
void Document::setTopComments(QString const& comment)
{
    QStringList comments = comment.split("\n");
    topComments_ = comments;
}

//-----------------------------------------------------------------------------
// Function: Document::setTopComments()
//-----------------------------------------------------------------------------
void Document::setTopComments( const QStringList& comments )
{
    topComments_ = comments;
}

//-----------------------------------------------------------------------------
// Function: Document::getTopComments()
//-----------------------------------------------------------------------------
QStringList Document::getTopComments() const
{
    return topComments_;
}

//-----------------------------------------------------------------------------
// Function: Document::getDependentDirs()
//-----------------------------------------------------------------------------
QStringList Document::getDependentDirs() const
{
    return QStringList();
}

//-----------------------------------------------------------------------------
// Function: librarycomponent::setVersion()
//-----------------------------------------------------------------------------
void Document::setVersion(QString versionNumber)
{
    foreach(QSharedPointer<VendorExtension> extension, *vendorExtensions_)
    {
        if (extension->type() == "kactus2:version")
        {
            extension.dynamicCast<Kactus2Value>()->setValue(versionNumber);
            return;
        }
    }

    vendorExtensions_->append(QSharedPointer<Kactus2Value>(new Kactus2Value("kactus2:version", versionNumber)));
}

//-----------------------------------------------------------------------------
// Function: Document::hasKactusAttributes()
//-----------------------------------------------------------------------------
bool Document::hasKactusAttributes() const
{
    return !kactus2Attributes_.isEmpty();
}

//-----------------------------------------------------------------------------
// Function: Document::hasImplementation()
//-----------------------------------------------------------------------------
bool Document::hasImplementation() const
{
    return kactus2Attributes_.contains(QString("kts_implementation"));
}

//-----------------------------------------------------------------------------
// Function: Document::setImplementation()
//-----------------------------------------------------------------------------
void Document::setImplementation(KactusAttribute::Implementation implementation)
{
    kactus2Attributes_.insert("kts_implementation", KactusAttribute::valueToString(implementation));
}

//-----------------------------------------------------------------------------
// Function: Document::getImplementation()
//-----------------------------------------------------------------------------
KactusAttribute::Implementation Document::getImplementation() const
{
    KactusAttribute::Implementation implementation = KactusAttribute::HW;

    if (!hasImplementation())
    {
        return implementation;
    }
    else
    {
        KactusAttribute::stringToValue(kactus2Attributes_.value(QString("kts_implementation")), implementation);
    }

    return implementation;
}

//-----------------------------------------------------------------------------
// Function: Document::hasProductHierarchy()
//-----------------------------------------------------------------------------
bool Document::hasProductHierarchy() const
{
    return kactus2Attributes_.contains(QString("kts_productHier"));
}

//-----------------------------------------------------------------------------
// Function: Document::setHierarchy()
//-----------------------------------------------------------------------------
void Document::setHierarchy(KactusAttribute::ProductHierarchy productHierarchy)
{
    kactus2Attributes_.insert("kts_productHier", KactusAttribute::valueToString(productHierarchy));
}

//-----------------------------------------------------------------------------
// Function: Document::getHierarchy()
//-----------------------------------------------------------------------------
KactusAttribute::ProductHierarchy Document::getHierarchy() const
{
    KactusAttribute::ProductHierarchy hierarchy = KactusAttribute::FLAT;

    // if attribute is not found
    if (!hasProductHierarchy())
    {
        return hierarchy;
    }
    else
    {
        KactusAttribute::stringToValue(kactus2Attributes_.value(QString("kts_productHier")), hierarchy);
    }

    return hierarchy;
}

//-----------------------------------------------------------------------------
// Function: Document::hasFirmness()
//-----------------------------------------------------------------------------
bool Document::hasFirmness() const
{
    return kactus2Attributes_.contains("kts_firmness");
}

//-----------------------------------------------------------------------------
// Function: Document::getFirmness()
//-----------------------------------------------------------------------------
KactusAttribute::Firmness Document::getFirmness() const
{
    KactusAttribute::Firmness firmness = KactusAttribute::MUTABLE;
    if (!kactus2Attributes_.contains(QString("kts_firmness")))
    {
        return firmness;
    }
    else
    {
        KactusAttribute::stringToValue(kactus2Attributes_.value(QString("kts_firmness")), firmness);
    }

    return firmness;
}

//-----------------------------------------------------------------------------
// Function: Document::setFirmness()
//-----------------------------------------------------------------------------
void Document::setFirmness(KactusAttribute::Firmness firmness)
{
    kactus2Attributes_.insert("kts_firmness", KactusAttribute::valueToString(firmness));
}

//-----------------------------------------------------------------------------
// Function: Document::copyParameters()
//-----------------------------------------------------------------------------
void Document::copyParameters(Document const& other)
{
    foreach (QSharedPointer<Parameter> parameter, *other.parameters_)
    {
        QSharedPointer<Parameter> copy = QSharedPointer<Parameter>(new Parameter(*parameter.data()));
        parameters_->append(copy);
    }
}

//-----------------------------------------------------------------------------
// Function: Document::copyAssertions()
//-----------------------------------------------------------------------------
void Document::copyAssertions(Document const& other)
{
    foreach (QSharedPointer<Assertion> assertion, *other.assertions_)
    {
        if (assertion)
        {
            QSharedPointer<Assertion> copy = QSharedPointer<Assertion>(new Assertion(*assertion.data()));
            assertions_->append(copy);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: librarycomponent::copyVendorExtensions()
//-----------------------------------------------------------------------------
void Document::copyVendorExtensions(const Document & other)
{
    foreach (QSharedPointer<VendorExtension> extension, *other.vendorExtensions_)
    {
        vendorExtensions_->append(QSharedPointer<VendorExtension>(extension->clone()));
    }
}
