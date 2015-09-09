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

class Parameter;
class VendorExtension;
//-----------------------------------------------------------------------------
//! Writer class for IP-XACT Parameter element.
//-----------------------------------------------------------------------------
class ParameterWriter : public QObject
{
    Q_OBJECT
public:

	//! The constructor.
	ParameterWriter(QObject* parent = 0);

	//! The destructor.
	~ParameterWriter();

    /*!
     *  Writes the given parameter into xml.
     *
     *      @param [in] writer      The xml writer to use.
     *      @param [in] parameter   The parameter to write.
     */
    void writeParameter(QXmlStreamWriter& writer, QSharedPointer<Parameter> parameter) const;

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
     */
    void writeNameGroup(QXmlStreamWriter& writer, QSharedPointer<Parameter> parameter) const;
    
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
           
    /*!
     *  Writes the vendor extensions of a given parameter into xml.
     *
     *      @param [in] writer      The xml writer to use.
     *      @param [in] parameter   The parameter whose vendor extensions to write.
     */
    void writeVendorExtensions(QXmlStreamWriter& writer, QSharedPointer<Parameter> parameter) const;

private:

    // Disable copying.
    ParameterWriter(ParameterWriter const& rhs);
    ParameterWriter& operator=(ParameterWriter const& rhs);
};

#endif // PARAMETERWRITER_H
