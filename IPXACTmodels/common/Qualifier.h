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

    //! Map of qualifier type and string pairs.
    static const QMap<Qualifier::Type, QString> QUALIFIER_TYPE_STRING;

    Qualifier();

    Qualifier(Qualifier const& other);

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

    /*!
     *	Gets the qualifier types that have been set.
     *  
     *		
     * 		@return A list of set types.
     */
    QSharedPointer<QList<Type> > getTypes() const;

    /*!
     *	Sets the level attribute of the reset qualifier.
     *  
     *      @param [in] level	The level to set.
     */
    void setResetLevel(QString const& level);

    /*!
     *	Gets the level attribute of the reset qualifier.
     *
     *      @return The reset level.
     */
    QString getResetLevel() const;

    /*!
     *	Sets the level attribute of the clock enable qualifier.
     *
     *      @param [in] level	The level to set.
     */
    void setClockEnableLevel(QString const& level);

    /*!
     *	Gets the level attribute of the clock enable qualifier.
     *
     *      @return The reset level.
     */
    QString getClockEnableLevel() const;

    /*!
     *	Sets the level attribute of the power enable qualifier.
     *
     *      @param [in] level	The level to set.
     */
    void setPowerEnableLevel(QString const& level);

    /*!
     *	Gets the level attribute of the power enable qualifier.
     *
     *      @return The reset level.
     */
    QString getPowerEnableLevel() const;

    /*!
     *	Sets the powerDomainRef attribute of the power enable qualifier.
     *
     *      @param [in] reference	The power domain reference to set.
     */
    void setPowerDomainRef(QString const& reference);

    /*!
     *	Gets the powerDomainRef attribute of the power enable qualifier.
     *
     *      @return The power domain reference.
     */
    QString getPowerDomainRef() const;

    /*!
     *	Sets the flowType attribute of the flow control qualifier.
     *
     *      @param [in] flowType    The flow type to set.
     */
    void setFlowType(QString const& flowType);

    /*!
     *	Gets the flowType attribute of the flow control qualifier.
     *
     *      @return The flow type.
     */
    QString getFlowType() const;

    /*!
     *	Sets the user attribute of the flow control qualifier.
     *
     *      @param [in] userFlowType    The user defined flow type to set.
     */
    void setUserFlowType(QString const& userFlowType);

    /*!
     *	Gets the user attribute of the flow control qualifier.
     *
     *      @return The user defined flow type.
     */
    QString getUserFlowType() const;

    /*!
     *	Sets the user attribute of the user qualifier.
     *
     *      @param [in] userDefined     The user defined qualifier.
     */
    void setUserDefined(QString const& userDefined);

    /*!
     *	Gets the user attribute of the user qualifier.
     *
     *      @return The user defined qualifier.
     */
    QString getUserDefined() const;
    
    /*!
     *	Get a selected attribute by qualifier type, e.g. attribute 'level' for qualifier type Reset.
     *  
     *      @param [in] attributeName	The attribute name.
     *      @param [in] qualifierType	The qualifier type.
     *		
     * 		@return The attribute as a string.
     */
    QString getAttribute(QString const& attributeName, Qualifier::Type qualifierType) const;

    /*!
     *	Sets a given attribute for a specific qualifier type.
     *  
     *      @param [in] attributeName	The attribute name to set.
     *      @param [in] attributeValue	The attribute value to set.
     *      @param [in] qualifierType	The qualifier type to set the attribute to.
     */
    void setAttribute(QString const& attributeName, QString const& attributeValue, Qualifier::Type qualifierType);

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

private:
    //! The list of types assigned to this qualifier.
    QSharedPointer<QList<Type> > types_ = QSharedPointer<QList<Type> >(new QList<Type>());

    //! The level attribute for qualifier Reset.
    QString resetLevel_;

    //! The level attribute for qualifier Clock Enable.
    QString clockEnableLevel_;
    
    //! The level attribute for qualifier Power Enable.
    QString powerEnableLevel_;

    //! The powerDomainRef attribute for qualifier Power Enable.
    QString powerDomainRef_;

    //! The flowType attribute for qualifier Flow Control.
    QString flowType_;

    //! The user attribute for qualifier Flow Control.
    QString userFlowType_;

    //! The user attribute for qualifier User.
    QString userDefined_;
};

#endif // QUALIFIER_H
