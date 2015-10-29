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

#include <IPXACTmodels/common/Extendable.h>
#include <IPXACTmodels/common/NameGroup.h>

#include "Array.h"
#include "Vector.h"

#include <QString>
#include <QMap>
#include <QStringList>
#include <QSharedPointer>

class VendorExtension;

//-----------------------------------------------------------------------------
//! Implementation for ipxact:parameter element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT Parameter : public NameGroup, public Extendable
{
public:

	/*! The default constructor
	 *
	 * Constructs an empty invalid parameter.
	*/
	Parameter();

	//! Copy constructor
	Parameter(const Parameter &other);

    //! The destructor
    virtual ~Parameter();

	//! Assignment operator
	Parameter &operator=(const Parameter &other);
    
    virtual QString elementName() const;

	/*! Get the value of the parameter
	 *
	 *      @return QString containing the value
	 */
	QString getValue() const;

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
     *  Gets the names of the parameter attributes.
     *
     *      @return The parameter attribute names.
     */
    QStringList getAttributeNames() const;

    /*!
     *  Gets the names of the parameter value attributes.
     *
     *      @return The parameter value attribute names.
     */
    QStringList getValueAttributeNames() const;

    /*!
     *  Gets the vectors for the parameter.
     *
     *      @return The parameter vectors.
     */
    QSharedPointer<QList<QSharedPointer<Vector> > > getVectors() const;
        
    /*!
     *  Gets the left index of the parameter vector.
     *
     *      @return The left index of the vector.
     */
    QString getVectorLeft() const;

    /*!
     *  Gets the right index of the parameter vector.
     *
     *      @return The right index of the vector.
     */
    QString getVectorRight() const;

    /*!
     *  Sets the left index of the parameter vector.
     *
     *      @param [in] leftExpression   The left index to set.
     */
    void setVectorLeft(QString const& leftExpression);
    
    /*!
     *  Sets the right index of the parameter vector.
     *
     *      @param [in] leftExpression   The left index to set.
     */
    void setVectorRight(QString const& rightExpression);
    
    /*!
     *  Gets the arrays for the parameter.
     *
     *      @return The parameter arrays.
     */
    QSharedPointer<QList<QSharedPointer<Array> > > getArrays() const;
    
    /*!
     *  Gets the left index of the parameter array.
     *
     *      @return The left index of the array.
     */
    QString getArrayLeft() const;

    /*!
     *  Gets the right index of the parameter array.
     *
     *      @return The right index of the array.
     */
    QString getArrayRight() const;

    /*!
     *  Sets the left index of the parameter array.
     *
     *      @param [in] leftExpression   The left index to set.
     */
    void setArrayLeft(QString const& leftExpression);
        
    /*!
     *  Sets the right index of the parameter array.
     *
     *      @param [in] leftExpression   The left index to set.
     */
    void setArrayRight(QString const& rightExpression);

private:
 
    /*!
     *  Creates an UUID for the parameter.
     */
    void createUuid();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The parameter value.
	QString value_;

    //! Parameter attributes.
	QMap<QString, QString> attributes_;

	//! The attributes for the value.
	QMap<QString, QString> valueAttributes_;

    //! Vector for containing the bit width of the parameter.
    QSharedPointer<Vector> bitWidthVector_;

    //! Vectors for defining parameter bit width.
    QSharedPointer<QList<QSharedPointer<Vector> > > vectors_;

    //! Arrays for defining parameter as an array of values.
    QSharedPointer<QList<QSharedPointer<Array> > > arrays_;

};

#endif /* PARAMETER_H */
