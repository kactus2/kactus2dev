//-----------------------------------------------------------------------------
// File: modelparameter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 2.8.2010
//
// Description:
// Equals the spirit:modelParameter element in IP-Xact specification.
//-----------------------------------------------------------------------------

#ifndef MODELPARAMETER_H_
#define MODELPARAMETER_H_

#include <IPXACTmodels/NameGroup.h>
#include <IPXACTmodels/parameter.h>

#include "ipxactmodels_global.h"

#include <QDomNode>
#include <QString>
#include <QMap>
#include <QXmlStreamWriter>
#include <QStringList>

class VendorExtension;

//-----------------------------------------------------------------------------
// Equals the spirit:modelParameter element in IP-Xact specification.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT ModelParameter : public Parameter
{
public:

	/*! The constructor
	 *
	 *      @param [in] modelParameterNode A reference to a QDomNode to parse the information from.
	 */
	ModelParameter(QDomNode &modelParameterNode);

	/*! The default constructor
	 *
	 * Constructs an empty model parameter that is not valid
	*/
	ModelParameter();

	//! The copy constructor
	ModelParameter(const ModelParameter &other);

	//! Assignment operator
	ModelParameter &operator=(const ModelParameter &other);

	//! The destructor
	~ModelParameter();

   /*! Checks if the model parameter is valid IP-Xact.
	*
	*      @return True if the model parameter is in valid state.
	*/
    virtual bool isValid() const;

	/*! Check if the model parameter is in a valid state.
	*
	*      @param [in] errorList            The list to add the possible error messages to.
	*      @param [in] parentIdentifier     String from parent to help to identify the location of the error.
	*
	*      @return True if the state is valid and writing is possible.
	*/
	virtual bool isValid(QStringList& errorList, QString const& parentIdentifier) const;
    
	/*! Get the data type of the model parameter
	 *
	 *      @return QString containing the data type
	*/
	QString getDataType() const;

	/*! Set the data type of the model parameter
	 *
	 *      @param [in] dataType QString containing the data type
	 *
	*/
	void setDataType(QString const& dataType);

	/*! Get the usage type of the model parameter
	 *
	 *
	 *      @return QString containing the usage type
	*/
	QString getUsageType() const;

	/*! Set the usage type of the model parameter
	 *
	 *      @param [in] usageType QString containing the usage type
	 *
	*/
	void setUsageType(QString const& usageType);           
    
protected:
                        
    /*!
     *  Gets the identifier for the IP-Xact element represented by the model parameter.
     *
     *      @return The name of IP-Xact element.
     */
    virtual QString elementIdentifier() const;

    /*!
     *  Gets the general name of the IP-Xact element represented by the model parameter.
     *
     *      @return The name of IP-Xact element.
     */
    virtual QString elementName() const;

};


#endif /* MODELPARAMETER_H_ */
