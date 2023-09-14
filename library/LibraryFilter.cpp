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

//-----------------------------------------------------------------------------
// Function: LibraryFilter::LibraryFilter()
//-----------------------------------------------------------------------------
LibraryFilter::LibraryFilter(LibraryInterface* libraryAccess, QObject *parent):
QSortFilterProxyModel(parent), 
    firmness_(),
    implementation_(),
    hierarchy_(),
    tags_(),
    libraryAccess_(libraryAccess)
{
	// set settings for filter
	setSortLocaleAware(true);
	setSortCaseSensitivity(Qt::CaseInsensitive);

    Q_ASSERT(libraryAccess_);
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
// Function: LibraryFilter::getLibraryInterface()
//-----------------------------------------------------------------------------
LibraryInterface* LibraryFilter::getLibraryInterface() const
{
    return libraryAccess_;
}
