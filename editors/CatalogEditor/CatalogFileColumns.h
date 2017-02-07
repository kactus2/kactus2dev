//-----------------------------------------------------------------------------
// File: CatalogFileColumns.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 02.02.2017
//
// Description:
// Common declarations for ipxactFiles in catalog editor.
//-----------------------------------------------------------------------------

#include <IPXACTmodels/common/VLNV.h>

namespace CatalogFileColumns
{
    //! The editable columns in the ipxactFiles of the catalog editor.
    enum columns
    {
        TREE_NAVIGATION = 0,
        VENDOR,
        LIBRARY,
        NAME,
        VERSION,
        PATH,
        COLUMN_COUNT
    };

    //! Types in the different categories.
    enum categories{
        CATALOGS = 0,
        BUS_DEFINITIONS,
        ABSTRACTION_DEFINITIONS,
        COMPONENTS,
        ABSTRACTORS,
        DESIGNS,
        DESIGN_CONFIGURATIONS,
        GENERATOR_CHAINS,
        UNKNOWN,
        CATEGORY_COUNT
    };   

    //! Types in the different categories.
    const VLNV::IPXactType CATEGORY_TYPES[CatalogFileColumns::CATEGORY_COUNT] =
    {
        VLNV::CATALOG,
        VLNV::BUSDEFINITION,
        VLNV::ABSTRACTIONDEFINITION,
        VLNV::COMPONENT,
        VLNV::ABSTRACTOR,
        VLNV::DESIGN,
        VLNV::DESIGNCONFIGURATION,
        VLNV::GENERATORCHAIN,
        VLNV::INVALID
    };

}
