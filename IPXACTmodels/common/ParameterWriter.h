//-----------------------------------------------------------------------------
// File: ParameterWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 31.07.2015
//
// Description:
// Writer class for IP-XACT Parameter element.
//-----------------------------------------------------------------------------

#ifndef PARAMETERWRITER_H
#define PARAMETERWRITER_H

#include <QObject>
#include <QSharedPointer>
#include <QXmlStreamWriter>

#include <IPXACTmodels/ipxactmodels_global.h>
#include <IPXACTmodels/common/Document.h>

class Parameter;
class VendorExtension;

//-----------------------------------------------------------------------------
//! Writer class for IP-XACT Parameter element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT ParameterWriter
{
public:

	//! The constructor.
	ParameterWriter();

	//! The destructor.
	virtual ~ParameterWriter();

    // Disable copying.
    ParameterWriter(ParameterWriter const& rhs) = delete;
    ParameterWriter& operator=(ParameterWriter const& rhs) = delete;

    /*!
     *  Writes the given parameter into xml.
     *
     *      @param [in] writer      The xml writer to use.
     *      @param [in] parameter   The parameter to write.
     *      @param [in] docRevision The document std revision.
     */
    void writeParameter(QXmlStreamWriter& writer, QSharedPointer<Parameter> parameter, 
        Document::Revision docRevision) const;

protected:

    /*!
     *  Writes the attributes of a given parameter into xml.
     *
     *      @param [in] writer      The xml writer to use.
     *      @param [in] parameter   The parameter whose attributes to write.
     */
    void writeAttributes(QXmlStreamWriter& writer, QSharedPointer<Parameter> parameter) const;

    /*!
     *  Writes the name group of a given parameter into xml.
     *
     *      @param [in] writer      The xml writer to use.
     *      @param [in] parameter   The parameter whose name group to write.
     *      @param [in] docRevision The document std revision.
     */
    void writeNameGroup(QXmlStreamWriter& writer, QSharedPointer<Parameter> parameter, 
        Document::Revision docRevision) const;

    /*!
     *  Writes the vectors of a given parameter into xml.
     *
     *      @param [in] writer      The xml writer to use.
     *      @param [in] parameter   The parameter whose vectors to write.
     */
    void writeVectors(QXmlStreamWriter& writer, QSharedPointer<Parameter> parameter) const;
        
    /*!
     *  Writes the arrays of a given parameter into xml.
     *
     *      @param [in] writer      The xml writer to use.
     *      @param [in] parameter   The parameter whose arrays to write.
     */
    void writeArrays(QXmlStreamWriter& writer, QSharedPointer<Parameter> parameter) const;
        
    /*!
     *  Writes the value of a given parameter into xml.
     *
     *      @param [in] writer      The xml writer to use.
     *      @param [in] parameter   The parameter whose value to write.
     */
    void writeValue(QXmlStreamWriter& writer, QSharedPointer<Parameter> parameter) const;
};

#endif // PARAMETERWRITER_H
