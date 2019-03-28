//-----------------------------------------------------------------------------
// File: VendorExtensionsGeneral.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 21.03.2019
//
// Description:
// General enumerations for vendor extensions editor.
//-----------------------------------------------------------------------------

#include <QVector>
#include <QPair>
#include <QString>

#include <QMetaType>

namespace VendorExtensionsGeneral
{
    //! The different user roles for data.
    enum vendorExtensionEditorRoles
    {
        getGenericExtensionRole = Qt::UserRole  //! Gets the generic vendor extension.
    };
}
