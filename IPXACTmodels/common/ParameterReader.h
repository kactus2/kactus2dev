//-----------------------------------------------------------------------------
// File: ParameterReader.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 03.08.2015
//
// Description:
// XML reader for IP-XACT Parameter element.
//-----------------------------------------------------------------------------

#ifndef PARAMETERREADER_H
#define PARAMETERREADER_H

#include <QDomNode>

#include <IPXACTmodels/common/Document.h>

#include <IPXACTmodels/ipxactmodels_global.h>

class Parameter;

//-----------------------------------------------------------------------------
//! XML reader for IP-XACT Parameter element.
//-----------------------------------------------------------------------------
namespace ParameterReader
{
    /*!
     *  Creates a parameter from XML description.
     *
     *      @param [in] parameterNode   The XML description of the parameter.
     *      @param [in] revision        The standard revision of the XML document being read.
     *
     *      @return The created parameter.
     */
    IPXACTMODELS_EXPORT QSharedPointer<Parameter> createParameterFrom(QDomNode const& parameterNode,
        Document::Revision revision = Document::Revision::Std14);
    
    namespace Details
    {

        /*!
         *  Reads the attributes from XML to a parameter.
         *
         *      @param [in] parameterNode   The XML description of the parameter.
         *      @param [in/out] parameter   The parameter to insert the attributes into.
         */
        void parseAttributes(QDomNode const& parameterNode, QSharedPointer<Parameter> parameter);

        /*!
         *  Reads the vectors from XML to a parameter.
         *
         *      @param [in] parameterNode   The XML description of the parameter.
         *      @param [in/out] parameter   The parameter to insert the vectors into.
         *      @param [in] revision        The standard revision of the XML document being read.
         */
        void parseVectors(QDomNode const& parameterNode, QSharedPointer<Parameter> parameter,
            Document::Revision revision = Document::Revision::Std14);

        /*!
         *  Reads the arrays from XML to a parameter.
         *
         *      @param [in] parameterNode   The XML description of the parameter.
         *      @param [in/out] parameter   The parameter to insert the arrays into.
         *      @param [in] revision        The standard revision of the XML document being read.
         */
        void parseArrays(QDomNode const& parameterNode, QSharedPointer<Parameter> parameter,
            Document::Revision revision = Document::Revision::Std14);

        /*!
         *  Reads the parameter value from XML to a parameter.
         *
         *      @param [in] parameterNode   The XML description of the parameter.
         *      @param [in/out] parameter   The parameter to insert the value into.
         */
        void parseValue(QDomNode const& parameterNode, QSharedPointer<Parameter> parameter);
    }
};

#endif // PARAMETERREADER_H