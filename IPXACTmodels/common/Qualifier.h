//-----------------------------------------------------------------------------
// File: Qualifier.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 11.08.2015
//
// Description:
// Implementation for ipxact:qualifier group.
//-----------------------------------------------------------------------------

#ifndef QUALIFIER_H
#define QUALIFIER_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QString>
#include <QMap>

//-----------------------------------------------------------------------------
//! Implementation for ipxact:qualifier group.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT Qualifier 
{
public:

    enum Type
    {
        Address,
        Data,
        Clock,
        Reset,
        Valid,
        Interrupt,
        ClockEnable,
        PowerEnable,
        Opcode,
        Protection,
        FlowControl,
        User,
        Request,
        Response,
        Any
    };

    enum Attribute
    {
        ResetLevel = 0,
        ClockEnableLevel,
        PowerEnableLevel,
        PowerDomainReference,
        FlowType,
        UserFlowType,
        UserDefined,
        COUNT
    };

    //! Map of qualifier type and string pairs.
    static const QMap<Qualifier::Type, QString> QUALIFIER_TYPE_STRING;

    Qualifier();

    Qualifier(Qualifier const& other);

    Qualifier& operator=(Qualifier const& other);

    ~Qualifier() = default;

    /*!
     *	Checks if the qualifier has been set.
     *		
     * 		@return True, if any qualifier type is set, otherwise false.
     */
    bool isSet() const;

    /*!
     *	Clears any values set to the qualifier.
     */
    void clear();

    /*!
     *	Checks if the qualifier has a specific type set.
     *  
     *      @param [in] type	The type to check.
     *		
     * 		@return True, if the type is set, otherwise false.
     */
    bool hasType(Type type) const;

    /*!
     *	Sets the qualifier type.
     *  
     *      @param [in] type	The type to set.
     */
    void setType(Type type);

    void removeType(Type type);

    /*!
     *	Gets the qualifier types that have been set.
     *  
     *		
     * 		@return A list of set types.
     */
    QSharedPointer<QList<Type> > getTypes() const;
    
    /*!
     *	Get a selected attribute.
     *  
     *      @param [in] attributeName	The attribute name.
     *		
     * 		@return The attribute as a string.
     */
    QString getAttribute(Attribute attribute) const;

    /*!
     *	Sets a given qualifier attribute.
     *  
     *      @param [in] attributeName	The attribute name to set.
     *      @param [in] attributeValue	The attribute value to set.
     */
    void setAttribute(Attribute attribute, QString const& attributeValue);

    bool operator==(Qualifier const& other);

    bool operator!=(Qualifier const& other);

    /*!
     *	Gets a qualifier type as string.
     *  
     *      @param [in] type	Type to convert to string.
     *		
     * 		@return Type as string.
     */
    static QString typeToString(Type type);

    /*!
     *	Converts a qualifier type from string to type.
     *
     *      @param [in] typeString	Type as a string to convert to type.
     *
     * 		@return Qualifier type.
     */
    static Type stringToType(QString const& typeString);

    /*!
     *	Converts a qualifier attribute name string to attribute.
     *  
     *      @param [in] attributeName	The attribute as string.
     *		
     * 		@return The converted attribute.
     */
    static Attribute stringToAttributeName(QString const& attributeName);

private:
    //! The list of types assigned to this qualifier.
    QSharedPointer<QList<Type> > types_ = QSharedPointer<QList<Type> >(new QList<Type>());

    //! The list of attributes set for the qualifier.
    std::array<QString, Attribute::COUNT> attributes_;
};

#endif // QUALIFIER_H
