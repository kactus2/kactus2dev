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

#include "libraryitem.h"
#include "libraryinterface.h"

#include <IPXACTmodels/librarycomponent.h>
#include <IPXACTmodels/busdefinition.h>
#include <IPXACTmodels/design.h>
#include <IPXACTmodels/designconfiguration.h>

#include <QRegExp>

//-----------------------------------------------------------------------------
// Function: LibraryFilter::LibraryFilter()
//-----------------------------------------------------------------------------
LibraryFilter::LibraryFilter(QObject *parent):
QSortFilterProxyModel(parent), 
firmness_(),
implementation_(),
hierarchy_(),
vendorValidator_(this),
libraryValidator_(this),
nameValidator_(this),
versionValidator_(this),
vendorFilter_(),
libraryFilter_(),
nameFilter_(),
versionFilter_() 
{
	// set settings for filter
	setSortLocaleAware(true);
	setSortCaseSensitivity(Qt::CaseInsensitive);

	QRegExp regExp("*", Qt::CaseInsensitive, QRegExp::Wildcard);
	vendorValidator_.setRegExp(regExp);
	libraryValidator_.setRegExp(regExp);
	nameValidator_.setRegExp(regExp);
	versionValidator_.setRegExp(regExp);
}

//-----------------------------------------------------------------------------
// Function: LibraryFilter::~LibraryFilter()
//-----------------------------------------------------------------------------
LibraryFilter::~LibraryFilter()
{
}

//-----------------------------------------------------------------------------
// Function: LibraryFilter::onVendorChanged()
//-----------------------------------------------------------------------------
void LibraryFilter::onVendorChanged(QString const& vendorText)
{
    vendorFilter_ = vendorText;

	// update the reg exp for validator
	QRegExp regExp(vendorText, Qt::CaseInsensitive, QRegExp::Wildcard);
	vendorValidator_.setRegExp(regExp);

	invalidateFilter();
}

//-----------------------------------------------------------------------------
// Function: LibraryFilter::onLibraryChanged()
//-----------------------------------------------------------------------------
void LibraryFilter::onLibraryChanged(QString const& libraryText)
{
    libraryFilter_ = libraryText;

	// update the reg exp for validator
	QRegExp regExp(libraryText, Qt::CaseInsensitive, QRegExp::Wildcard);
	libraryValidator_.setRegExp(regExp);

	invalidateFilter();
}

//-----------------------------------------------------------------------------
// Function: LibraryFilter::onNameChanged()
//-----------------------------------------------------------------------------
void LibraryFilter::onNameChanged(QString const& nameText)
{
    nameFilter_ = nameText;

	// update the reg exp for validator
	QRegExp regExp(nameText, Qt::CaseInsensitive, QRegExp::Wildcard);
	nameValidator_.setRegExp(regExp);

	invalidateFilter();
}

//-----------------------------------------------------------------------------
// Function: LibraryFilter::onVersionChanged()
//-----------------------------------------------------------------------------
void LibraryFilter::onVersionChanged(QString const& versionText)
{
    versionFilter_ = versionText;

	// update the reg exp for validator
	QRegExp regExp(versionText, Qt::CaseInsensitive, QRegExp::Wildcard);
	versionValidator_.setRegExp(regExp);

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
bool LibraryFilter::checkFirmness( QSharedPointer<Component const> component ) const
{
	Q_ASSERT(component);

    if (component->getComponentImplementation() == KactusAttribute::SW)
    {
        return true;
    }

    KactusAttribute::Firmness componentFirmness = component->getComponentFirmness();
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
    else if (componentFirmness == KactusAttribute::DEFINITIONS)
    {
        return firmness_.definitions_;
    }
    else
    {
        return false;
    }
}

//-----------------------------------------------------------------------------
// Function: LibraryFilter::checkImplementation()
//-----------------------------------------------------------------------------
bool LibraryFilter::checkImplementation( QSharedPointer<Component const> component ) const
{
    Q_ASSERT(component);

    KactusAttribute::Implementation componentImplementation = component->getComponentImplementation();
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

    if (component->getComponentImplementation() == KactusAttribute::SW)
    {
        return true;
    }

    KactusAttribute::ProductHierarchy componentHierarchy = component->getComponentHierarchy();
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
// Function: LibraryFilter::checkVLNVs()
//-----------------------------------------------------------------------------
bool LibraryFilter::checkVLNVs(QList<VLNV> const& list) const
{
    foreach (VLNV vlnv, list)
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
