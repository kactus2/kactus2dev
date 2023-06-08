//-----------------------------------------------------------------------------
// File: ParameterReader.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 03.08.2015
//
// Description:
// XML reader class for IP-XACT Parameter element.
//-----------------------------------------------------------------------------

#ifndef PARAMETERREADER_H
#define PARAMETERREADER_H

#include <QDomNode>

#include <IPXACTmodels/common/Document.h>

#include <IPXACTmodels/ipxactmodels_global.h>

class Parameter;

//-----------------------------------------------------------------------------
//! XML reader class for IP-XACT Parameter element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT ParameterReader
{
public:

	//! The constructor.
	ParameterReader();

	//! The destructor.
	virtual ~ParameterReader();

    // Disable copying.
    ParameterReader(ParameterReader const& rhs) = delete;
    ParameterReader& operator=(ParameterReader const& rhs) = delete;

    /*!
     *  Creates a parameter from XML description.
     *
     *      @param [in] parameterNode   The XML description of the parameter.
     *      @param [in] revision    The standard revision of the XML document being read.
     *
     *      @return The created parameter.
     */
    QSharedPointer<Parameter> createParameterFrom(QDomNode const& parameterNode,
        Document::Revision revision = Document::Revision::Std14) const;

protected:

    /*!
     *  Reads the attributes from XML to a parameter.
     *
     *      @param [in] parameterNode   The XML description of the parameter.
     *      @param [in/out] parameter   The parameter to insert the attributes into.
     */
    void parseAttributes(QDomNode const& parameterNode, QSharedPointer<Parameter> parameter) const;
   
    /*!
     *  Reads the name group from XML to a parameter.
     *
     *      @param [in] parameterNode   The XML description of the parameter.
     *      @param [in/out] parameter   The parameter to insert the name group info into.
     */
    void parseNameGroup(QDomNode const& parameterNode, QSharedPointer<Parameter> parameter) const;
       
    /*!
     *  Reads the vectors from XML to a parameter.
     *
     *      @param [in] parameterNode   The XML description of the parameter.
     *      @param [in/out] parameter   The parameter to insert the vectors into.
     *      @param [in] revision        The standard revision of the XML document being read.
     */
    void parseVectors(QDomNode const& parameterNode, QSharedPointer<Parameter> parameter,
        Document::Revision revision = Document::Revision::Std14) const;
      
    /*!
     *  Reads the arrays from XML to a parameter.
     *
     *      @param [in] parameterNode   The XML description of the parameter.
     *      @param [in/out] parameter   The parameter to insert the arrays into.
     */
    void parseArrays(QDomNode const& parameterNode, QSharedPointer<Parameter> parameter) const;
       
    /*!
     *  Reads the parameter value from XML to a parameter.
     *
     *      @param [in] parameterNode   The XML description of the parameter.
     *      @param [in/out] parameter   The parameter to insert the value into.
     */
    void parseValue(QDomNode const& parameterNode, QSharedPointer<Parameter> parameter) const;

};

#endif // PARAMETERREADER_H