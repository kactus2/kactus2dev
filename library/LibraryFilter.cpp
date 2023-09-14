//-----------------------------------------------------------------------------
// File: LibraryFilter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 10.03.2015
//
// Description:
// Base class for library filters.
//-----------------------------------------------------------------------------

#include "LibraryFilter.h"

#include "LibraryItem.h"

#include <IPXACTmodels/Component/Component.h>

#include <QRegularExpression>

//-----------------------------------------------------------------------------
// Function: LibraryFilter::LibraryFilter()
//-----------------------------------------------------------------------------
LibraryFilter::LibraryFilter(LibraryInterface* libraryAccess, QObject *parent):
QSortFilterProxyModel(parent), 
    firmness_(),
    implementation_(),
    hierarchy_(),
    tags_(),
    vendorValidator_(this),
    libraryValidator_(this),
    nameValidator_(this),
    versionValidator_(this),
    vendorFilter_(),
    libraryFilter_(),
    nameFilter_(),
    versionFilter_(),
    libraryAccess_(libraryAccess)
{
	// set settings for filter
	setSortLocaleAware(true);
	setSortCaseSensitivity(Qt::CaseInsensitive);

	QRegularExpression regExp(".*", QRegularExpression::CaseInsensitiveOption | QRegularExpression::DotMatchesEverythingOption);
	vendorValidator_.setRegularExpression(regExp);
	libraryValidator_.setRegularExpression(regExp);
	nameValidator_.setRegularExpression(regExp);
	versionValidator_.setRegularExpression(regExp);

    Q_ASSERT(libraryAccess_);
}

//-----------------------------------------------------------------------------
// Function: LibraryFilter::onVendorChanged()
//-----------------------------------------------------------------------------
void LibraryFilter::onVendorChanged(QString const& vendorText)
{
    vendorFilter_ = vendorText + ".*";

	// update the reg exp for validator
	QRegularExpression regExp(vendorFilter_, QRegularExpression::CaseInsensitiveOption | QRegularExpression::DotMatchesEverythingOption);
	vendorValidator_.setRegularExpression(regExp);

	invalidateFilter();
}

//-----------------------------------------------------------------------------
// Function: LibraryFilter::onLibraryChanged()
//-----------------------------------------------------------------------------
void LibraryFilter::onLibraryChanged(QString const& libraryText)
{
    libraryFilter_ = libraryText + ".*";

	// update the reg exp for validator
	QRegularExpression regExp(libraryFilter_, QRegularExpression::CaseInsensitiveOption | QRegularExpression::DotMatchesEverythingOption);
	libraryValidator_.setRegularExpression(regExp);

	invalidateFilter();
}

//-----------------------------------------------------------------------------
// Function: LibraryFilter::onNameChanged()
//-----------------------------------------------------------------------------
void LibraryFilter::onNameChanged(QString const& nameText)
{
    nameFilter_ = nameText + ".*";

	// update the reg exp for validator
	QRegularExpression regExp(nameFilter_, QRegularExpression::CaseInsensitiveOption | QRegularExpression::DotMatchesEverythingOption);
	nameValidator_.setRegularExpression(regExp);

	invalidateFilter();
}

//-----------------------------------------------------------------------------
// Function: LibraryFilter::onVersionChanged()
//-----------------------------------------------------------------------------
void LibraryFilter::onVersionChanged(QString const& versionText)
{
    versionFilter_ = versionText + ".*";

	// update the reg exp for validator
	QRegularExpression regExp(versionFilter_, QRegularExpression::CaseInsensitiveOption | QRegularExpression::DotMatchesEverythingOption);
	versionValidator_.setRegularExpression(regExp);

	invalidateFilter();
}

//-----------------------------------------------------------------------------
// Function: LibraryFilter::onFiltersChanged()
//-----------------------------------------------------------------------------
void LibraryFilter::onFiltersChanged(Utils::FilterOptions const& filters)
{
    firmness_ = filters.firmness;
    implementation_ = filters.implementation;
    type_ = filters.type;
    hierarchy_ = filters.hierarchy;
    invalidateFilter();
}

//-----------------------------------------------------------------------------
// Function: LibraryFilter::onFirmnessChanged()
//-----------------------------------------------------------------------------
void LibraryFilter::onFirmnessChanged(Utils::FirmnessOptions const& options)
{
	firmness_ = options;
	invalidateFilter();
}

//-----------------------------------------------------------------------------
// Function: LibraryFilter::onImplementationChanged()
//-----------------------------------------------------------------------------
void LibraryFilter::onImplementationChanged(Utils::ImplementationOptions const& options)
{
	implementation_ = options;
	invalidateFilter();
}

//-----------------------------------------------------------------------------
// Function: LibraryFilter::onTypeChanged()
//-----------------------------------------------------------------------------
void LibraryFilter::onTypeChanged(Utils::TypeOptions const& options)
{
	type_ = options;
	invalidateFilter();
}

//-----------------------------------------------------------------------------
// Function: LibraryFilter::onHierarchyChanged()
//-----------------------------------------------------------------------------
void LibraryFilter::onHierarchyChanged(Utils::HierarchyOptions const& options)
{
	hierarchy_ = options;
	invalidateFilter();
}

//-----------------------------------------------------------------------------
// Function: LibraryFilter::onValidityChanged()
//-----------------------------------------------------------------------------
void LibraryFilter::onValidityChanged(Utils::ValidityOptions const& options)
{
    validity_ = options;
    invalidateFilter();
}

//-----------------------------------------------------------------------------
// Function: LibraryFilter::onTagFilterChanged()
//-----------------------------------------------------------------------------
void LibraryFilter::onTagFilterChanged(QVector<TagData> filteredTags)
{
    tags_ = filteredTags;
    invalidateFilter();
}

//-----------------------------------------------------------------------------
// Function: LibraryFilter::Firmness()
//-----------------------------------------------------------------------------
Utils::FirmnessOptions LibraryFilter::firmness() const
{
    return firmness_;
}

//-----------------------------------------------------------------------------
// Function: LibraryFilter::Implementation()
//-----------------------------------------------------------------------------
Utils::ImplementationOptions LibraryFilter::implementation() const
{
    return implementation_;
}

//-----------------------------------------------------------------------------
// Function: LibraryFilter::Type()
//-----------------------------------------------------------------------------
Utils::TypeOptions LibraryFilter::type() const
{
    return type_;
}

//-----------------------------------------------------------------------------
// Function: LibraryFilter::hasEmptyVLNVfilter()
//-----------------------------------------------------------------------------
bool LibraryFilter::hasEmptyVLNVfilter() const
{
    return vendorFilter_.isEmpty() && libraryFilter_.isEmpty() && nameFilter_.isEmpty() && 
        versionFilter_.isEmpty();
}

//-----------------------------------------------------------------------------
// Function: LibraryFilter::checkFirmness()
//-----------------------------------------------------------------------------
bool LibraryFilter::checkFirmness(QSharedPointer<Component const> component) const
{
	Q_ASSERT(component);

    if (component->getImplementation() == KactusAttribute::SW)
    {
        return true;
    }

    KactusAttribute::Firmness componentFirmness = component->getFirmness();
    if (componentFirmness == KactusAttribute::TEMPLATE)
    {
        return firmness_.templates_;
    }
    else if (componentFirmness == KactusAttribute::MUTABLE)
    {
        return firmness_.mutable_;
    }
    else if (componentFirmness == KactusAttribute::FIXED)
    {
        return firmness_.fixed_;
    }
    else
    {
        return false;
    }
}

//-----------------------------------------------------------------------------
// Function: LibraryFilter::checkImplementation()
//-----------------------------------------------------------------------------
bool LibraryFilter::checkImplementation(QSharedPointer<Component const> component) const
{
    Q_ASSERT(component);

    KactusAttribute::Implementation componentImplementation = component->getImplementation();
    if (componentImplementation == KactusAttribute::HW)
    {
        // if the HW component contains system views then it should be considered also as system
        if (component->hasSystemViews())
        {
            return implementation_.hw_ || implementation_.system_;
        }

        // if the component is pure HW 
        return implementation_.hw_;
    }
    else if (componentImplementation == KactusAttribute::SW)
    {
        return implementation_.sw_;
    }
    else if (componentImplementation == KactusAttribute::SYSTEM)
    {
        return implementation_.system_;
    }
    else
    {
        return false;
    }
}

//-----------------------------------------------------------------------------
// Function: LibraryFilter::checkHierarchy()
//-----------------------------------------------------------------------------
bool LibraryFilter::checkHierarchy(QSharedPointer<Component const> component) const
{
	Q_ASSERT(component);

    if (component->getImplementation() == KactusAttribute::SW)
    {
        return true;
    }

    KactusAttribute::ProductHierarchy componentHierarchy = component->getHierarchy();
    if (componentHierarchy == KactusAttribute::FLAT)
    {
        return hierarchy_.flat_;
    }
    else if (componentHierarchy ==  KactusAttribute::PRODUCT)
    {
        return hierarchy_.product_;
    }
    else if (componentHierarchy == KactusAttribute::BOARD)
    {
        return hierarchy_.board_;
    }
    else if (componentHierarchy == KactusAttribute::CHIP)
    {
        return hierarchy_.chip_;
    }
    else if (componentHierarchy == KactusAttribute::SOC)
    {
        return hierarchy_.soc_;
    }
    else if (componentHierarchy == KactusAttribute::IP)
    {
        return hierarchy_.ip_;
    }
    else 
    {
        return false;
    }
}

//-----------------------------------------------------------------------------
// Function: LibraryFilter::checkValidity()
//-----------------------------------------------------------------------------
bool LibraryFilter::checkValidity(bool isValid) const
{
    return (isValid && validity_.valid_) || (isValid == false && validity_.invalid_);
}

//-----------------------------------------------------------------------------
// Function: LibraryFilter::checkTags()
//-----------------------------------------------------------------------------
bool LibraryFilter::checkTags(QSharedPointer<Document const> document) const
{
    int tagCount = tags_.size();
    if (tagCount == 0)
    {
        return true;
    }

    QVector<TagData> documentTags = document->getTags();
    if (documentTags.size() < tagCount)
    {
        return false;
    }

    int documentTagCounter = 0;
    for (auto const& singleDocumentTag : documentTags)
    {
        if (tags_.contains(singleDocumentTag))
        {
            documentTagCounter++;
            if (documentTagCounter == tagCount)
            {
                return true;
            }
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: LibraryFilter::documentContainsTag()
//-----------------------------------------------------------------------------
bool LibraryFilter::documentContainsTag(QSharedPointer<Document const> document, TagData const& tag) const
{
    for (auto documentTag : document->getTags())
    {
        if (documentTag.name_ == tag.name_ && documentTag.color_ == tag.color_)
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: LibraryFilter::checkVLNVs()
//-----------------------------------------------------------------------------
bool LibraryFilter::checkVLNVs(QVector<VLNV> const& list) const
{
    for (VLNV const& vlnv : list)
    {
        int pos = 0;

        // QT library Forced to use temp variables
        QString vendor = vlnv.getVendor();
        QString library =  vlnv.getLibrary();
        QString name =  vlnv.getName();
        QString version =  vlnv.getVersion();

        // if the vlnv matches the search rules
        if ((vendorValidator_.validate(vendor, pos) == QValidator::Acceptable) &&
            (libraryValidator_.validate(library, pos) == QValidator::Acceptable) &&
            (nameValidator_.validate(name, pos) == QValidator::Acceptable) &&
            (versionValidator_.validate(version, pos) == QValidator::Acceptable)) 
        {
            return true;
        }
    }
    // if none of the vlnvs matched
	return false;
}

//-----------------------------------------------------------------------------
// Function: LibraryFilter::getLibraryInterface()
//-----------------------------------------------------------------------------
LibraryInterface* LibraryFilter::getLibraryInterface() const
{
    return libraryAccess_;
}
