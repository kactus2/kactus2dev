//-----------------------------------------------------------------------------
// File: parameter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 27.7.2010
//
// Description:
// Equals the spirit:parameter element in IP-Xact specification.
//-----------------------------------------------------------------------------

#ifndef PARAMETER_H_
#define PARAMETER_H_

#include "ipxactmodels_global.h"

#include "NameGroup.h"

#include <QDomNode>
#include <QString>
#include <QMap>
#include <QXmlStreamWriter>
#include <QStringList>

class VendorExtension;

//-----------------------------------------------------------------------------
//! Equals the spirit:parameter element in IP-Xact specification.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT Parameter
{
public:

	/*! The constructor
	 *
	 *      @param [in] parameterNode A reference to the QDomNode to parse the information from.
	 */
	Parameter(QDomNode &parameterNode);

	/*! The default constructor
	 *
	 * Constructs an empty invalid parameter.
	*/
	Parameter();

	//! Copy constructor
	Parameter(const Parameter &other);

	//! Assignment operator
	Parameter &operator=(const Parameter &other);

	//! The destructor
	virtual ~Parameter();

	/*! Write the contents of the class using the writer.
	*
	* Uses the specified writer to write the class contents into file as valid IP-Xact.
	*
	*      @param [in] writer A reference to a QXmlStreamWriter instance that is used to write the document.
	*/
	virtual void write(QXmlStreamWriter& writer);

	/*! Check if the Parameter is in valid state or not.
	*
	*      @return True if the parameter is valid.
	*/
	virtual bool isValid() const;

	/*! Check if the parameter is in a valid state.
	*
	*      @param [in] errorList            The list to add the possible error messages to.
	*      @param [in] parentIdentifier     String from parent to help to identify the location of the error.
	*
	*      @return bool True if the state is valid and writing is possible.
	*/
	virtual bool isValid(QStringList& errorList, QString const& parentIdentifier) const;

	/*! Get the name of the parameter
	 *
	 *      @return QString containing the name
	 */
	QString getName() const;

	/*! Get the display name of the parameter.
	 *
	 *      @return The parameter display name.
	 */
	QString getDisplayName() const;

	/*! Get the description of the parameter.
	 *
	 *      @return QString containing the description.
	 */
	QString getDescription() const;
    
	/*! Get the value of the parameter
	 *
	 *      @return QString containing the value
	 */
	QString getValue() const;

	/*! Set the name for the parameter
	 *
	 *      @param [in] name QString containing the name
	 */
	void setName(QString const& name);
    
	/*! Set the display name for the parameter.
	 *
	 *      @param [in] displayName The display name to set.
	 */
	void setDisplayName(QString const& displayName);
    
	/*! Set the description for the parameter.
	 *
	 *      @param [in] description QString containing the description.
	 */
	void setDescription(QString const& description);

	/*! Set the value for the parameter
	 *
	 *      @param [in] value QString containing the value
	 */
	void setValue(QString const& value);
    
    /*!
     *  Gets the choice referenced by the parameter value.
     *
     *      @return The choice name referenced by the value or an empty string, if no choice is referenced.
     */
    QString getChoiceRef() const;

    /*!
     *  Sets the choice to reference by the parameter value.
     *
     *      @param [in] choiceRef   The choice name to reference.
     */
    void setChoiceRef(QString const& choiceRef);

    /*!
     *  Gets the format for the parameter value.
     *
     *      @return The format for the parameter value.
     */
    QString getValueFormat() const;
    
    /*!
     *  Sets the format for the parameter value.
     *
     *      @param [in] format   The format for the parameter value.
     */
    void setValueFormat(QString const& format);

    /*!
     *  Gets the length of the bit string for storing the parameter value.
     *
     *      @return The length of the bit string.
     */
    QString getBitStringLength() const;

    /*!
     *  Sets the length of the bit string for storing the parameter value.
     *
     *      @param [in] length   The bit string length for the parameter value.
     */
    void setBitStringLength(QString const& length);

    /*!
     *  Gets the minimum value for the parameter value.
     *
     *      @return The minimum value.
     */
    QString getMinimumValue() const;

    /*!
     *  Sets the minimum value for the parameter value
     *
     *      @param [in] minimum   The minimum value.
     */
    void setMinimumValue(QString const& minimum);
    
    /*!
     *  Gets the maximum value for the parameter value.
     *
     *      @return The maximum value.
     */
    QString getMaximumValue() const;

    /*!
     *  Sets the maximum value for the parameter value
     *
     *      @param [in] maximum   The maximum value.
     */
    void setMaximumValue(QString const& maximum);

    /*!
     *  Gets the resolve type for the parameter value.
     *
     *      @return The resolve type for the value.
     */
    QString getValueResolve() const;

    /*!
     *  Sets the resolve type for the parameter value
     *
     *      @param [in] resolve   The resolve type.
     */
    void setValueResolve(QString const& resolve);

    /*!
     *  Checks if the parameter has an attribute with the given name.
     *
     *      @param [in] attributeName   The name of the attribute to find.
     *
     *      @return True, if an attribute with the given name exists, otherwise false.
     */
    bool hasAttribute(QString const& attributeName) const;

    /*!
     *  Gets a value of an attribute.
     *
     *      @param [in] attributeName       The name of the attribute to get.
     *
     *      @return The attribute value.
     */
    QString getAttribute(QString const& attributeName) const;    

    /*!
     *  Sets an attribute.
     *
     *      @param [in] attributeName       The name of the attribute to set.
     *      @param [in] attributeValue      The value of the attribute.
     */
    void setAttribute(QString const& attributeName, QString const& attributeValue);
 
    /*!
     *  Sets an attribute for the value.
     *
     *      @param [in] attributeName       The name of the attribute to set.
     *      @param [in] attributeValue      The value of the attribute.
     */
    void setValueAttribute(QString const& attributeName, QString const& attributeValue);

protected:
        
    /*!
     *  Gets the identifier for the IP-Xact element represented by the parameter e.g. spirit:parameter.
     *
     *      @return The name of IP-Xact element.
     */
    virtual QString elementIdentifier() const;
       
    /*!
     *  Gets the general name of the IP-Xact element represented by the parameter e.g. parameter.
     *
     *      @return The name of IP-Xact element.
     */
    virtual QString elementName() const;

private:
    
    /*!
     *  Checks if the format attribute of the value is set.
     *
     *      @return True, if the format is set, otherwise false.
     */
    bool formatSet() const;

    /*!
     *  Checks if the format attribute of the value is valid.
     *
     *      @return True, if the format is valid, otherwise false.
     */
    bool isValidFormat() const;

    /*!
     *  Checks if the parameter value is valid in the specified format (if any).
     *
     *      @return True, if the value is valid for the format, otherwise false.
     */
    bool isValidValueForFormat() const;
    
    /*!
     *  Checks if the parameter boundary value (minimum/maximum) is valid in the specified format.
     *
     *      @return True, if the boundary value is valid for the format, otherwise false.
     */
    bool isValidBoundaryForFormat(QString const& boundary) const;
    
    /*!
     *  Checks if the parameter value should be compared to the minimum value.
     *
     *      @return True, if the value should be compared, otherwise false.
     */
    bool shouldCompareValueToMinimum() const;

    bool shouldCompareValueToMaximum() const;

    /*!
     *  Checks if the parameter value is less than the minimum value.
     *
     *      @return True, if the value is less than the minimum value, otherwise false.
     */
    bool valueIsLessThanMinimum() const;

    bool valueIsGreaterThanMaximum() const;

    /*!
     *   Gets the value of a given string in the format specified for the value.
     *
     *      @param [in] value   The string whose value to get.
     *
     *      @return The value of the string.
     */
    qreal valueOf(QString const& value) const;
    
    /*!
     *   Gets the value of a given string when the string is interpret as a long integer.
     *
     *      @param [in] value   The string whose value to get.
     *
     *      @return The value of the string.
     */
    qreal longValueOf(QString const& value) const;
    
    /*!
     *   Gets the value of a given string when the string is interpret as a floating point number.
     *
     *      @param [in] value   The string whose value to get.
     *
     *      @return The value of the string.
     */
    qreal floatValueOf(QString const& value) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Contains the name, display name and description of parameter.
    NameGroup nameGroup_;

	/*!  The actual value of the parameter
	 * MANDATORY
	 */
	QString value_;

    /*!
	 * OPTIONAL
	 * Contains the attributes for the parameter
	 */
	QMap<QString, QString> attributes_;

	//! The attributes for the value
	QMap<QString, QString> valueAttributes_;

    /*!
	 * OPTIONAL spirit: vendorExtensions
	 * Parameter vendor extensions.
	 */
    QList<QSharedPointer<VendorExtension> > vendorExtensions_;
};

#endif /* PARAMETER_H_ */
