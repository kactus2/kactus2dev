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

#include <QObject>
#include <QDomNode>

class Parameter;

//-----------------------------------------------------------------------------
//! XML reader class for IP-XACT Parameter element.
//-----------------------------------------------------------------------------
class ParameterReader : public QObject
{
    Q_OBJECT

public:

	//! The constructor.
	ParameterReader(QObject* parent = 0);

	//! The destructor.
	~ParameterReader();

    /*!
     *  Creates a parameter from XML description.
     *
     *      @param [in] parameterNode   The XML description of the parameter.
     *
     *      @return The created parameter.
     */
    QSharedPointer<Parameter> createParameterFrom(QDomNode const& parameterNode) const;

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
     */
    void parseVectors(QDomNode const& parameterNode, QSharedPointer<Parameter> parameter) const;
      
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
       
    /*!
     *  Reads the vendor extensions from XML to a parameter.
     *
     *      @param [in] parameterNode   The XML description of the parameter.
     *      @param [in/out] parameter   The parameter to insert the vendor extensions into.
     */
    void parseVendorExtensions(QDomNode const& parameterNode, QSharedPointer<Parameter> parameter) const;

private:

    // Disable copying.
    ParameterReader(ParameterReader const& rhs);
    ParameterReader& operator=(ParameterReader const& rhs);

};

#endif // PARAMETERREADER_H