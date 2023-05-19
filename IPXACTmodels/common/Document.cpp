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
#include <IPXACTmodels/common/VLNV.h>

#include <IPXACTmodels/generaldeclarations.h>

#include <IPXACTmodels/kactusExtensions/Kactus2Group.h>

#include <QString>
#include <QSharedPointer>
#include <QObject>
#include <QList>
#include <QStringList>

//-----------------------------------------------------------------------------
// Function: Document::Document()
//-----------------------------------------------------------------------------
Document::Document(Revision revision):
Extendable(),
    
    revision_(revision)
{
    addDefaultNameSpaces(revision);
    setSchemaLocation(revision);
}

//-----------------------------------------------------------------------------
// Function: Document::Document()
//-----------------------------------------------------------------------------
Document::Document(const VLNV &vlnv, Revision revision):
Extendable(),
    revision_(revision)
{
    documentNameGroup_.vlnv = vlnv;
    addDefaultNameSpaces(revision);
    setSchemaLocation(revision);
}

//-----------------------------------------------------------------------------
// Function: Document::Document()
//-----------------------------------------------------------------------------
Document::Document(Document const& other):
Extendable(other),
    documentNameGroup_(other.documentNameGroup_),
    revision_(other.revision_),
    topComments_(other.topComments_),
    xmlProcessingInstructions_(other.xmlProcessingInstructions_),
    xmlNameSpaces_(other.xmlNameSpaces_),
    xmlSchemaLocation_(other.xmlSchemaLocation_)
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

        if (!documentNameGroup_.vlnv.isEmpty())
        {
            documentNameGroup_.vlnv = other.documentNameGroup_.vlnv;
        }
		else
        {
            documentNameGroup_.vlnv = VLNV();
        }

        documentNameGroup_.description = other.documentNameGroup_.description;
		topComments_ = other.topComments_;

        xmlProcessingInstructions_ = other.xmlProcessingInstructions_;
        xmlNameSpaces_ = other.xmlNameSpaces_;
        xmlSchemaLocation_ = other.xmlSchemaLocation_;

        copyParameters(other);
        copyAssertions(other);

	}
	return *this;
}


//-----------------------------------------------------------------------------
// Function: Document::getVlnv()
//-----------------------------------------------------------------------------
VLNV Document::getVlnv() const
{
	return documentNameGroup_.vlnv;
}

//-----------------------------------------------------------------------------
// Function: Document::setVlnv()
//-----------------------------------------------------------------------------
void Document::setVlnv(VLNV const& vlnv)
{
    documentNameGroup_.vlnv = vlnv;
}

//-----------------------------------------------------------------------------
// Function: Document::getDisplayName()
//-----------------------------------------------------------------------------
QString Document::getDisplayName() const
{
    return documentNameGroup_.displayName;
}

//-----------------------------------------------------------------------------
// Function: Document::setDisplayName()
//-----------------------------------------------------------------------------
void Document::setDisplayName(const QString& displayName)
{
    documentNameGroup_.displayName = displayName;
}

//-----------------------------------------------------------------------------
// Function: Document::getShortDescription()
//-----------------------------------------------------------------------------
QString Document::getShortDescription() const
{
    return documentNameGroup_.shortDescription;
}

//-----------------------------------------------------------------------------
// Function: Document::setShortDescription()
//-----------------------------------------------------------------------------
void Document::setShortDescription(const QString& shortDescription)
{
    documentNameGroup_.shortDescription = shortDescription;
}

//-----------------------------------------------------------------------------
// Function: Document::getDescription()
//-----------------------------------------------------------------------------
QString Document::getDescription() const
{
    return documentNameGroup_.description;
}

//-----------------------------------------------------------------------------
// Function: Document::setDescription()
//-----------------------------------------------------------------------------
void Document::setDescription(QString const& description)
{
    documentNameGroup_.description = description;
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
    topComments_ = comment.split(QStringLiteral("\n"));
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
// Function: Document::addXmlNameSpace()
//-----------------------------------------------------------------------------
void Document::addXmlNameSpace(QString const& nameSpace, QString const& uri)
{
    // Go through all known xml name spaces.
    // Discard the change if it already exists.
    if (std::any_of(xmlNameSpaces_.cbegin(), xmlNameSpaces_.cend(), 
        [nameSpace](auto& xmlNameSpace) {return xmlNameSpace.first == nameSpace;}))
    {
        return;
    }

    // Otherwise append to the list.
    xmlNameSpaces_.append(qMakePair(nameSpace, uri));
}

//-----------------------------------------------------------------------------
// Function: Document::getXmlNameSpaces()
//-----------------------------------------------------------------------------
QVector<QPair<QString, QString> > Document::getXmlNameSpaces() const
{
    return xmlNameSpaces_;
}

//-----------------------------------------------------------------------------
// Function: Document::setSchemaLocation()
//-----------------------------------------------------------------------------
void Document::setSchemaLocation(Document::Revision revision)
{
    if (revision == Revision::Std14)
    {
        xmlSchemaLocation_ = QStringLiteral("http://www.accellera.org/XMLSchema/IPXACT/1685-2014 http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd");
    }
    else if (revision == Revision::Std22)
    {
        xmlSchemaLocation_ = QStringLiteral("http://www.accellera.org/XMLSchema/IPXACT/1685-2022 http://www.accellera.org/XMLSchema/IPXACT/1685-2022/index.xsd");
    }
    else
    {
        xmlSchemaLocation_ = QStringLiteral("");
    }
}

//-----------------------------------------------------------------------------
// Function: Document::getSchemaLocation()
//-----------------------------------------------------------------------------
QString Document::getSchemaLocation() const
{
    return xmlSchemaLocation_;
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
    auto extension = findVendorExtension(QStringLiteral("kactus2:version"));

    if (extension != nullptr)
    {
        extension.dynamicCast<Kactus2Value>()->setValue(versionNumber);
        return;
    }

    getVendorExtensions()->append(QSharedPointer<Kactus2Value>(new Kactus2Value(QStringLiteral("kactus2:version"),
        versionNumber)));
}

//-----------------------------------------------------------------------------
// Function: Document::getVersion()
//-----------------------------------------------------------------------------
QString Document::getVersion() const
{
    auto extension = findVendorExtension(QStringLiteral("kactus2:version"));

    if (extension != nullptr)
    {
        return extension.dynamicCast<Kactus2Value>()->value();
    }

    return QString();
}

//-----------------------------------------------------------------------------
// Function: Document::hasKactusAttributes()
//-----------------------------------------------------------------------------
bool Document::hasKactusAttributes() const
{
    return std::any_of(getVendorExtensions()->cbegin(), getVendorExtensions()->cend(), 
        [](auto& extension) {return extension->type() == QLatin1String("kactus2:extensions"); });
}

//-----------------------------------------------------------------------------
// Function: Document::hasImplementation()
//-----------------------------------------------------------------------------
bool Document::hasImplementation() const
{
    auto extension = findVendorExtension(QStringLiteral("kactus2:extensions"));

    if (extension != nullptr)
    {
        QSharedPointer<KactusAttribute> attributes = extension.dynamicCast<KactusAttribute>();
        return attributes->getImplementation() != KactusAttribute::KTS_IMPLEMENTATION_COUNT;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: Document::setImplementation()
//-----------------------------------------------------------------------------
void Document::setImplementation(KactusAttribute::Implementation implementation)
{
    auto extension = findVendorExtension(QStringLiteral("kactus2:extensions"));

    if (extension != nullptr)
    {
        return extension.dynamicCast<KactusAttribute>()->setImplementation(implementation);
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
    auto extension = findVendorExtension(QStringLiteral("kactus2:extensions"));

    auto implementationType = KactusAttribute::HW;

    if (extension != nullptr)
    {
        auto readType = extension.dynamicCast<KactusAttribute>()->getImplementation();

        if (readType != KactusAttribute::KTS_IMPLEMENTATION_COUNT)
        {
            implementationType = readType;
        }
    }

    return implementationType;
}

//-----------------------------------------------------------------------------
// Function: Document::hasProductHierarchy()
//-----------------------------------------------------------------------------
bool Document::hasProductHierarchy() const
{
    auto extension = findVendorExtension(QStringLiteral("kactus2:extensions"));

    if (extension != nullptr)
    {
        QSharedPointer<KactusAttribute> attributes = extension.dynamicCast<KactusAttribute>();
        return attributes->getHierarchy() != KactusAttribute::KTS_PRODHIER_COUNT;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: Document::setHierarchy()
//-----------------------------------------------------------------------------
void Document::setHierarchy(KactusAttribute::ProductHierarchy productHierarchy)
{
    auto extension = findVendorExtension(QStringLiteral("kactus2:extensions"));

    if (extension != nullptr)
    {
        return extension.dynamicCast<KactusAttribute>()->setHierarchy(productHierarchy);
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
    auto extension = findVendorExtension(QStringLiteral("kactus2:extensions"));

    if (extension != nullptr)
    {
        return extension.dynamicCast<KactusAttribute>()->getHierarchy();
    }

    return KactusAttribute::FLAT;
}

//-----------------------------------------------------------------------------
// Function: Document::hasFirmness()
//-----------------------------------------------------------------------------
bool Document::hasFirmness() const
{
    auto extension = findVendorExtension(QStringLiteral("kactus2:extensions"));

    if (extension != nullptr)
    {
        QSharedPointer<KactusAttribute> attributes = extension.dynamicCast<KactusAttribute>();
        return attributes->getFirmness() != KactusAttribute::KTS_REUSE_LEVEL_COUNT;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: Document::getFirmness()
//-----------------------------------------------------------------------------
KactusAttribute::Firmness Document::getFirmness() const
{
    auto extension = findVendorExtension(QStringLiteral("kactus2:extensions"));

    if (extension != nullptr)
    {
        QSharedPointer<KactusAttribute> attributes = extension.dynamicCast<KactusAttribute>();
        return attributes->getFirmness();
    }

    return KactusAttribute::MUTABLE;
}

//-----------------------------------------------------------------------------
// Function: Document::setFirmness()
//-----------------------------------------------------------------------------
void Document::setFirmness(KactusAttribute::Firmness firmness)
{
    auto extension = findVendorExtension(QStringLiteral("kactus2:extensions"));

    if (extension != nullptr)
    {
        return extension.dynamicCast<KactusAttribute>()->setFirmness(firmness);
    }

    QSharedPointer<KactusAttribute> attributes(new KactusAttribute());
    attributes->setFirmness(firmness);

    getVendorExtensions()->append(attributes);
}

//-----------------------------------------------------------------------------
// Function: Document::getLicense()
//-----------------------------------------------------------------------------
QString Document::getLicense() const
{
    auto extension = findVendorExtension(QStringLiteral("kactus2:license"));

    if (extension != nullptr)
    {
        QSharedPointer<Kactus2Value> licenseExtension = extension.dynamicCast<Kactus2Value>();
        return licenseExtension->value();
    }

    return QString();
}

//-----------------------------------------------------------------------------
// Function: Document::setLicense()
//-----------------------------------------------------------------------------
void Document::setLicense(QString const& license)
{
    auto extension = findVendorExtension(QStringLiteral("kactus2:license"));

    if (extension != nullptr)
    {
        QSharedPointer<Kactus2Value> licenseExtension = extension.dynamicCast<Kactus2Value>();
        licenseExtension->setValue(license);
    }
    else if (license.isEmpty() == false)
    {
        QSharedPointer<Kactus2Value> licenseValue (new Kactus2Value(QStringLiteral("kactus2:license"), license));
        getVendorExtensions()->append(licenseValue);
    }
    else
    {
        getVendorExtensions()->removeAll(extension);
    }
}

//-----------------------------------------------------------------------------
// Function: Document::addDefaultNameSpaces()
//-----------------------------------------------------------------------------
void Document::addDefaultNameSpaces(Revision revision)
{
    xmlNameSpaces_.append(qMakePair(QStringLiteral("xsi"), QStringLiteral("http://www.w3.org/2001/XMLSchema-instance")));

    if (revision == Revision::Std14)
    {
        xmlNameSpaces_.append(qMakePair(QStringLiteral("ipxact"), QStringLiteral("http://www.accellera.org/XMLSchema/IPXACT/1685-2014")));
    }
    else if (revision == Revision::Std22)
    {

        xmlNameSpaces_.append(qMakePair(QStringLiteral("ipxact"), QStringLiteral("http://www.accellera.org/XMLSchema/IPXACT/1685-2022")));
    }

    xmlNameSpaces_.append(qMakePair(QStringLiteral("kactus2"), QStringLiteral("http://kactus2.cs.tut.fi")));
}

//-----------------------------------------------------------------------------
// Function: Document::copyParameters()
//-----------------------------------------------------------------------------
void Document::copyParameters(Document const& other)
{
    parameters_->reserve(other.parameters_->count());
    for (QSharedPointer<Parameter> parameter : *other.parameters_)
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
    assertions_->reserve(other.assertions_->count());
    for (QSharedPointer<Assertion> assertion : *other.assertions_)
    {
        if (assertion)
        {
            QSharedPointer<Assertion> copy = QSharedPointer<Assertion>(new Assertion(*assertion.data()));
            assertions_->append(copy);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: Document::getTags()
//-----------------------------------------------------------------------------
QVector<TagData> Document::getTags() const
{
    QSharedPointer<Kactus2Group> tagGroup = getTagGroup();
    if (tagGroup == nullptr)
    {
        return QVector<TagData>();
    }

    QVector<TagData> documentTags;
    for (auto singleTag : tagGroup->getByType(QLatin1String("kactus2:tag")))
    {
        QSharedPointer<Kactus2Group> tagValue = singleTag.dynamicCast<Kactus2Group>();
        if (tagValue)
        {
            QString newTagName;
            QString newTagColor;

            QList<QSharedPointer<VendorExtension> > tagNamesExtension =
                tagValue->getByType(QLatin1String("kactus2:name"));
            if (tagNamesExtension.size() == 1)
            {
                QSharedPointer<Kactus2Value> tagName =
                    tagNamesExtension.first().dynamicCast<Kactus2Value>();
                if (tagName)
                {
                    newTagName = tagName->value();
                }
            }

            QList<QSharedPointer<VendorExtension> > tagColorsExtension =
                tagValue->getByType(QLatin1String("kactus2:color"));
            if (tagColorsExtension.size() == 1)
            {
                QSharedPointer<Kactus2Value> tagColor =
                    tagColorsExtension.first().dynamicCast<Kactus2Value>();
                if (tagColor)
                {
                    newTagColor = tagColor->value();
                }
            }

            if (!newTagName.isEmpty() && !newTagColor.isEmpty())
            {
                documentTags.append(TagData({ newTagName, newTagColor }));
            }
        }
    }

    return documentTags;
}

//-----------------------------------------------------------------------------
// Function: Document::getTagGroup()
//-----------------------------------------------------------------------------
QSharedPointer<Kactus2Group> Document::getTagGroup() const
{
    auto extension = findVendorExtension(QStringLiteral("kactus2:tags"));

    if (extension != nullptr)
    {
        QSharedPointer<Kactus2Group> tagGroup = extension.dynamicCast<Kactus2Group>();
        if (tagGroup)
        {
            return tagGroup;
        }
    }

    return QSharedPointer<Kactus2Group>();
}

//-----------------------------------------------------------------------------
// Function: Document::setTags()
//-----------------------------------------------------------------------------
void Document::setTags(QVector<TagData> newTags) const
{
    QSharedPointer<Kactus2Group> tagGroup = getTagGroup();

    if (tagGroup)
    {
        if (newTags.isEmpty())
        {
            getVendorExtensions()->removeAll(tagGroup);
        }
        else
        {
            removeNonExistingTags(tagGroup, newTags);
        }
    }
    else if (!tagGroup && !newTags.isEmpty())
    {
        tagGroup = QSharedPointer<Kactus2Group>(new Kactus2Group(QLatin1String("kactus2:tags")));
        getVendorExtensions()->append(tagGroup);
    }

    for (auto const& tag : newTags)
    {
        QSharedPointer<Kactus2Group> existingTag = getTagByName(tag.name_, tagGroup);
        if (existingTag)
        {
            QList<QSharedPointer<VendorExtension> > colorExtension =
                existingTag->getByType(QLatin1String("kactus2:color"));
            if (colorExtension.size() == 1)
            {
                QSharedPointer<Kactus2Value> tagColorExtension =
                    colorExtension.first().dynamicCast<Kactus2Value>();
                if (tagColorExtension)
                {
                    tagColorExtension->setValue(tag.color_);
                }
            }
        }
        else
        {
            QSharedPointer<Kactus2Value> newTagName(new Kactus2Value(QLatin1String("kactus2:name"), tag.name_));
            QSharedPointer<Kactus2Value> newTagColor(new Kactus2Value(QLatin1String("kactus2:color"), tag.color_));

            QSharedPointer<Kactus2Group> newTagContainer(new Kactus2Group(QLatin1String("kactus2:tag")));
            newTagContainer->addToGroup(newTagName);
            newTagContainer->addToGroup(newTagColor);

            tagGroup->addToGroup(newTagContainer);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: Document::getRevision()
//-----------------------------------------------------------------------------
Document::Revision Document::getRevision() const
{
    return revision_;
}

//-----------------------------------------------------------------------------
// Function: Document::getTagByName()
//-----------------------------------------------------------------------------
QSharedPointer<Kactus2Group> Document::getTagByName(QString const& name, QSharedPointer<Kactus2Group> tagGroup) const
{
    for (auto tagExtension : tagGroup->getByType(QLatin1String("kactus2:tag")))
    {
        QSharedPointer<Kactus2Group> tag = tagExtension.dynamicCast<Kactus2Group>();
        if (tag)
        {
            QList<QSharedPointer<VendorExtension> > tagNameContainer =
                tag->getByType(QLatin1String("kactus2:name"));
            if (tagNameContainer.size() == 1)
            {
                QSharedPointer<Kactus2Value> tagName = tagNameContainer.first().dynamicCast<Kactus2Value>();
                if (tagName && tagName->value() == name)
                {
                    return tag;
                }
            }
        }
    }

    return QSharedPointer<Kactus2Group>();
}

//-----------------------------------------------------------------------------
// Function: Document::removeNonExistingTags()
//-----------------------------------------------------------------------------
void Document::removeNonExistingTags(QSharedPointer<Kactus2Group> tagContainer, QVector<TagData> const& newTags) const
{
    for (auto tagExtension : tagContainer->getByType(QLatin1String("kactus2:tag")))
    {
        QString tagName = getTagName(tagExtension);
        if (std::none_of(newTags.cbegin(), newTags.cend(), 
            [tagName](auto& comparisonTag) {return comparisonTag.name_ == tagName; }))  
        {
            tagContainer->removeFromGroup(tagExtension);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: Document::getTagName()
//-----------------------------------------------------------------------------
QString Document::getTagName(QSharedPointer<VendorExtension> tagExtension) const
{
    QSharedPointer<Kactus2Group> tag = tagExtension.dynamicCast<Kactus2Group>();
    if (tag)
    {
        QList<QSharedPointer<VendorExtension> > tagNameContainer = tag->getByType(QLatin1String("kactus2:name"));
        if (tagNameContainer.size() == 1)
        {
            QSharedPointer<Kactus2Value> tagName = tagNameContainer.first().dynamicCast<Kactus2Value>();
            if (tagName)
            {
                return tagName->value();
            }
        }
    }

    return QString();
}
