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

#include <IPXACTmodels/common/GenericVendorExtension.h>
#include <IPXACTmodels/generaldeclarations.h>
#include <IPXACTmodels/XmlUtils.h>
#include <IPXACTmodels/common/VLNV.h>

#include <QString>
#include <QSharedPointer>
#include <QObject>
#include <QList>
#include <QStringList>

//-----------------------------------------------------------------------------
// Function: Document::Document()
//-----------------------------------------------------------------------------
Document::Document():
Extendable(),
    vlnv_(), 
    description_(),
    topComments_(),
    xmlProcessingInstructions_(),
    parameters_(new QList<QSharedPointer<Parameter> >()),
    assertions_(new QList<QSharedPointer<Assertion> >())
{
    vlnv_ = VLNV();
}

//-----------------------------------------------------------------------------
// Function: Document::Document()
//-----------------------------------------------------------------------------
Document::Document(const VLNV &vlnv):
Extendable(),
    vlnv_(), 
    description_(),
    topComments_(),
    xmlProcessingInstructions_(),
    parameters_(new QList<QSharedPointer<Parameter> >()),
    assertions_(new QList<QSharedPointer<Assertion> >())
{
    vlnv_ = vlnv;
}

//-----------------------------------------------------------------------------
// Function: Document::Document()
//-----------------------------------------------------------------------------
Document::Document(Document const& other):
Extendable(other),
    vlnv_(other.vlnv_),
    description_(other.description_),
    topComments_(other.topComments_),
    xmlProcessingInstructions_(other.xmlProcessingInstructions_),
    parameters_(new QList<QSharedPointer<Parameter> >()),
    assertions_(new QList<QSharedPointer<Assertion> >())
{
    copyParameters(other);
    copyAssertions(other);
}

//-----------------------------------------------------------------------------
// Function: Document::operator=()
//-----------------------------------------------------------------------------
Document & Document::operator=( const Document &other )
{
	if (this != &other)
    {
        Extendable::operator=(other);

        if (!vlnv_.isEmpty())
        {
            vlnv_ = other.vlnv_;
        }
		else
        {
			vlnv_ = VLNV();
        }

		description_ = other.description_;
		topComments_ = other.topComments_;

        copyParameters(other);
        copyAssertions(other);

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
// Function: Document::hasParameters()
//-----------------------------------------------------------------------------
bool Document::hasParameters() const
{
    return !parameters_->isEmpty();
}

//-----------------------------------------------------------------------------
// Function: DesignConfiguration::getAssertions()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<Assertion> > > Document::getAssertions() const
{
    return assertions_;
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
// Function: Document::setXmlProcessingInstructions()
//-----------------------------------------------------------------------------
void Document::addXmlProcessingInstructions(QString const& target, QString const& data)
{
    xmlProcessingInstructions_.append(qMakePair(target, data));
}

//-----------------------------------------------------------------------------
// Function: Document::getXmlProcessingInstructions()
//-----------------------------------------------------------------------------
QVector<QPair<QString, QString> > Document::getXmlProcessingInstructions() const
{
    return xmlProcessingInstructions_;
}

//-----------------------------------------------------------------------------
// Function: Document::getDependentDirs()
//-----------------------------------------------------------------------------
QStringList Document::getDependentDirs() const
{
    return QStringList();
}

//-----------------------------------------------------------------------------
// Function: Document::setVersion()
//-----------------------------------------------------------------------------
void Document::setVersion(QString const& versionNumber)
{
    foreach(QSharedPointer<VendorExtension> extension, *getVendorExtensions())
    {
        if (extension->type() == "kactus2:version")
        {
            extension.dynamicCast<Kactus2Value>()->setValue(versionNumber);
            return;
        }
    }

    getVendorExtensions()->append(QSharedPointer<Kactus2Value>(new Kactus2Value("kactus2:version", versionNumber)));
}

//-----------------------------------------------------------------------------
// Function: Document::getVersion()
//-----------------------------------------------------------------------------
QString Document::getVersion() const
{
    foreach(QSharedPointer<VendorExtension> extension, *getVendorExtensions())
    {
        if (extension->type() == "kactus2:version")
        {
            return extension.dynamicCast<Kactus2Value>()->value();
        }
    }

    return QString();
}

//-----------------------------------------------------------------------------
// Function: Document::hasKactusAttributes()
//-----------------------------------------------------------------------------
bool Document::hasKactusAttributes() const
{
    foreach(QSharedPointer<VendorExtension> extension, *getVendorExtensions())
    {
        if (extension->type() == "kactus2:extensions")
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: Document::hasImplementation()
//-----------------------------------------------------------------------------
bool Document::hasImplementation() const
{
    foreach(QSharedPointer<VendorExtension> extension, *getVendorExtensions())
    {
        if (extension->type() == "kactus2:extensions")
        {
            QSharedPointer<KactusAttribute> attributes = extension.dynamicCast<KactusAttribute>();
            return attributes->getImplementation() != KactusAttribute::KTS_IMPLEMENTATION_COUNT;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: Document::setImplementation()
//-----------------------------------------------------------------------------
void Document::setImplementation(KactusAttribute::Implementation implementation)
{
    foreach(QSharedPointer<VendorExtension> extension, *getVendorExtensions())
    {
        if (extension->type() == "kactus2:extensions")
        {
            return extension.dynamicCast<KactusAttribute>()->setImplementation(implementation);
        }
    }

    QSharedPointer<KactusAttribute> attributes(new KactusAttribute());
    attributes->setImplementation(implementation);

    getVendorExtensions()->append(attributes);
}

//-----------------------------------------------------------------------------
// Function: Document::getImplementation()
//-----------------------------------------------------------------------------
KactusAttribute::Implementation Document::getImplementation() const
{
    foreach(QSharedPointer<VendorExtension> extension, *getVendorExtensions())
    {
        if (extension->type() == "kactus2:extensions")
        {
            KactusAttribute::Implementation implementationType =
                extension.dynamicCast<KactusAttribute>()->getImplementation();
            if (implementationType == KactusAttribute::KTS_IMPLEMENTATION_COUNT)
            {
                return KactusAttribute::HW;
            }
            else
            {
                return implementationType;
            }
        }
    }

    return KactusAttribute::HW;
}

//-----------------------------------------------------------------------------
// Function: Document::hasProductHierarchy()
//-----------------------------------------------------------------------------
bool Document::hasProductHierarchy() const
{
    foreach(QSharedPointer<VendorExtension> extension, *getVendorExtensions())
    {
        if (extension->type() == "kactus2:extensions")
        {
            QSharedPointer<KactusAttribute> attributes = extension.dynamicCast<KactusAttribute>();
            return attributes->getHierarchy() != KactusAttribute::KTS_PRODHIER_COUNT;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: Document::setHierarchy()
//-----------------------------------------------------------------------------
void Document::setHierarchy(KactusAttribute::ProductHierarchy productHierarchy)
{
    foreach(QSharedPointer<VendorExtension> extension, *getVendorExtensions())
    {
        if (extension->type() == "kactus2:extensions")
        {
            return extension.dynamicCast<KactusAttribute>()->setHierarchy(productHierarchy);
        }
    }

    QSharedPointer<KactusAttribute> attributes(new KactusAttribute());
    attributes->setHierarchy(productHierarchy);

    getVendorExtensions()->append(attributes);
}

//-----------------------------------------------------------------------------
// Function: Document::getHierarchy()
//-----------------------------------------------------------------------------
KactusAttribute::ProductHierarchy Document::getHierarchy() const
{
    foreach(QSharedPointer<VendorExtension> extension, *getVendorExtensions())
    {
        if (extension->type() == "kactus2:extensions")
        {
            return extension.dynamicCast<KactusAttribute>()->getHierarchy();
        }
    }

    return KactusAttribute::FLAT;
}

//-----------------------------------------------------------------------------
// Function: Document::hasFirmness()
//-----------------------------------------------------------------------------
bool Document::hasFirmness() const
{
    foreach(QSharedPointer<VendorExtension> extension, *getVendorExtensions())
    {
        if (extension->type() == "kactus2:extensions")
        {
            QSharedPointer<KactusAttribute> attributes = extension.dynamicCast<KactusAttribute>();
            return attributes->getFirmness() != KactusAttribute::KTS_REUSE_LEVEL_COUNT;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: Document::getFirmness()
//-----------------------------------------------------------------------------
KactusAttribute::Firmness Document::getFirmness() const
{
    foreach(QSharedPointer<VendorExtension> extension, *getVendorExtensions())
    {
        if (extension->type() == "kactus2:extensions")
        {
            QSharedPointer<KactusAttribute> attributes = extension.dynamicCast<KactusAttribute>();
            return attributes->getFirmness();
        }
    }

    return KactusAttribute::MUTABLE;
}

//-----------------------------------------------------------------------------
// Function: Document::setFirmness()
//-----------------------------------------------------------------------------
void Document::setFirmness(KactusAttribute::Firmness firmness)
{
    foreach(QSharedPointer<VendorExtension> extension, *getVendorExtensions())
    {
        if (extension->type() == "kactus2:extensions")
        {
            return extension.dynamicCast<KactusAttribute>()->setFirmness(firmness);
        }
    }

    QSharedPointer<KactusAttribute> attributes(new KactusAttribute());
    attributes->setFirmness(firmness);

    getVendorExtensions()->append(attributes);
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
