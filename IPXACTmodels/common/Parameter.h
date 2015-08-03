//-----------------------------------------------------------------------------
// File: Parameter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 03.08.2015
//
// Description:
// Implementation for ipxact:parameter element.
//-----------------------------------------------------------------------------

#ifndef PARAMETER_H
#define PARAMETER_H

#include <IPXACTmodels/ipxactmodels_global.h>
#include <IPXACTmodels/NameGroup.h>

#include <IPXACTmodels/common/Array.h>
#include <IPXACTmodels/common/Vector.h>

#include <QDomNode>
#include <QString>
#include <QMap>
#include <QXmlStreamWriter>
#include <QStringList>
#include <QSharedPointer>

class VendorExtension;

//-----------------------------------------------------------------------------
//! Implementation for ipxact:parameter element.
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
	//virtual void write(QXmlStreamWriter& writer);

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
     *  Gets the IP-Xact format type.
     */
    QString getType() const;

    /*!
     *  Sets the IP-Xact format type.
     *
     *      @param [in] type   The format type.
     */
    void setType(QString const& type);

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
     *  Gets the unique id for the parameter value.
     *
     *      @return The unique id for the value.
     */
    QString getValueId() const;

    /*!
     *  Sets the unique id for referencing the parameter value
     *
     *      @param [in] id   The id to set.
     */
     void setValueId(QString const& id);

     /*!
      *  Get the amount of times the parameter has been referenced.
      *
      *     @return     The amount of times the parameter has been referenced.
      */
     int getUsageCount() const;

     /*!
      *  Increase the amount of references to this parameter.
      */
     void increaseUsageCount();

     /*!
      *  Decrease the amount of references to this parameter.
      */
     void decreaseUsageCount();

    /*!
     *  Checks if the parameter has an attribute with the given name.
     *
     *      @param [in] attributeName   The name of the attribute to find.
     *
     *      @return True, if an attribute with the given name exists, otherwise false.
     */
    bool hasAttribute(QString const& attributeName) const;

    /*!
     *  Gets the value of an attribute.
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
     *  Gets the value of an attribute of the paraemter value.
     *
     *      @param [in] attributeName       The name of the attribute to get.
     *
     *      @return The value attribute value.
     */
    QString getValueAttribute(QString const& attributeName) const;

    /*!
     *  Sets an attribute for the value.
     *
     *      @param [in] attributeName       The name of the attribute to set.
     *      @param [in] attributeValue      The value of the attribute.
     */
    void setValueAttribute(QString const& attributeName, QString const& attributeValue);
           
    /*!
     *  Gets the general name of the IP-Xact element represented by the parameter e.g. parameter.
     *
     *      @return The name of IP-Xact element.
     */
    virtual QString elementName() const;

    QStringList getAttributeNames() const;

    QStringList getValueAttributes() const;

    QSharedPointer<QList<QSharedPointer<Vector> > > getVectors() const;

    void setVectorLeft(QString const& leftExpression);

    void setVectorRight(QString const& rightExpression);

    QSharedPointer<QList<QSharedPointer<Array> > > getArrays() const;

    void setArrayLeft(QString const& leftExpression);
        
    QString getArrayLeft() const;

    void setArrayRight(QString const& rightExpression);
        
    QString getArrayRight() const;

    QSharedPointer<QList<QSharedPointer<VendorExtension> > > getVendorExtensions() const;

protected:
        
    /*!
     *  Gets the identifier for the IP-Xact element represented by the parameter e.g. spirit:parameter.
     *
     *      @return The name of IP-Xact element.
     */
    virtual QString elementIdentifier() const;

private:
 
    /*!
     *  Creates an UUID for the parameter.
     */
    void createUuid();

    /*!
     *  Parses the vendor extensions from a DOM node.
     *
     *      @param [in] vendorExtensionNode     The DOM node containing all the vendor extensions.
     */
    void parseVendorExtensions(QDomNode const& vendorExtensionNode);

    /*!
     *  Copies vendor extensions from another parameter.
     *
     *      @param [in] other   The parameter to copy extensions from.
     */
    void copyVendorExtensions(Parameter const& other);

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

    //! Vector for containing the bit width of the parameter.
    QSharedPointer<Vector> bitWidthVector_;


    QSharedPointer<QList<QSharedPointer<Vector> > > vectors_;

    QSharedPointer<QList<QSharedPointer<Array> > > arrays_;

    /*!
	 * OPTIONAL spirit: vendorExtensions
	 * Parameter vendor extensions.
	 */
    QSharedPointer<QList<QSharedPointer<VendorExtension> > > vendorExtensions_;
};

#endif /* PARAMETER_H */
