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
     *  Checks if the parameter has an attribute with the given name.
     *
     *      @param [in] attributeName   The name of the attribute to find.
     *
     *      @return True, if an attribute with the given name exists, otherwise false.
     */
    bool hasAttribute(QString const& attributeName) const;

	/*! Get the attributes for the parameter
	 *
	 *      @return QMap containing pointers to the attributes
	 */
	 QMap<QString, QString> const& getAttributes() const;

	/*! Get the attributes for the parameter
	 *
	 *      @return QMap containing pointers to the attributes
	 */
	virtual QMap<QString, QString> const& getValueAttributes() const;

protected:
        
    /*!
     *  Gets the name of the IP-Xact element represented by the parameter e.g. parameter or modelParameter.
     *  The default implementation returns spirit:parameter.
     *
     *      @return The name of IP-Xact element.
     */
    virtual QString elementName() const;

    /*!
     *  Sets an attribute.
     *
     *      @param [in] attributeName       The name of the attribute to set.
     *      @param [in] attributeValue      The value of the attribute.
     */
    void setAttribute(QString const& attributeName, QString const& attributeValue);

private:

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
