//-----------------------------------------------------------------------------
// File: ModuleParameterReader.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 09.09.2015
//
// Description:
// XML reader for IP-XACT module parameter element.
//-----------------------------------------------------------------------------

#ifndef MODULEPARAMETERREADER_H
#define MODULEPARAMETERREADER_H

#include "ModuleParameter.h"
#include "ParameterReader.h"

#include <IPXACTmodels/common/Document.h>

#include <QDomNode>

//-----------------------------------------------------------------------------
//! XML reader for IP-XACT module parameter element.
//-----------------------------------------------------------------------------
namespace ModuleParameterReader
{

    /*!
     *  Creates a module parameter from XML description.
     *
     *    @param [in] moduleParameterNode   The XML description of the module parameter.
     *    @param [in] docRevision           The IP-XACT standard to comply to.

     *    @return The created module parameter.
     */
    IPXACTMODELS_EXPORT QSharedPointer<ModuleParameter> createModuleParameterFrom(QDomNode const& moduleParameterNode,
        Document::Revision docRevision);

    namespace Details
    {

        /*!
         *  Reads the presence of the module parameter.
         *
         *    @param [in] moduleParameterNode     XML description of the module parameter.
         *    @param [in] moduleParameter         The module parameter whose presence is being read.
         *    @param [in] docRevision             The IP-XACT standard to comply to.
         */
        void parseIsPresent(QDomNode const& moduleParameterNode, QSharedPointer<ModuleParameter> moduleParameter,
            Document::Revision docRevision);
    }
};

#endif // MODULEPARAMETERREADER_H