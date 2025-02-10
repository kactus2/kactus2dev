//-----------------------------------------------------------------------------
// File: PacketValidator.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 06.06.2023
//
// Description:
// Validator for ipxact:packet.
//-----------------------------------------------------------------------------

#ifndef PACKETVALIDATOR_H
#define PACKETVALIDATOR_H

#include <IPXACTmodels/ipxactmodels_global.h>
#include <IPXACTmodels/AbstractionDefinition/Packet.h>

#include <KactusAPI/include/ExpressionParser.h>

#include <QString>
#include <QStringList>
#include <QSharedPointer>

//-----------------------------------------------------------------------------
//! Validator for ipxact:packet.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT PacketValidator
{
public:

    PacketValidator(QSharedPointer<ExpressionParser> expressionParser);

    ~PacketValidator() = default;

    // Disable copying.
    PacketValidator(PacketValidator const& other) = delete;
    PacketValidator& operator=(PacketValidator const& other) = delete;

    /*!
     *  Validates the given Packet.
     *
     *      @param [in] packet  The packet to validate.
     *
     *      @return True, if the Packet is valid IP-XACT, otherwise false.
     */
    bool validate(QSharedPointer<Packet> packet) const;

    /*!
     *  Finds possible errors in a Packet and creates a list of them.
     *
     *      @param [in] errors      List of found errors.
     *      @param [in] packet      The AbstractionDefinition whose errors to find.
     *      @param [in] context     Context to help locate the error.
     */
    void findErrorsIn(QStringList& errors, QSharedPointer<Packet> packet, QString const& context) const;

private:

    /*!
     *	Checks if packet field is valid.
     *  
     *      @param [in] packetField     The packet field to check.
     *		
     * 		@return True, if field is valid, otherwise false.
     */
    bool isValidPacketField(QSharedPointer<PacketField> packetField) const;

    /*!
     *	Finds possible errors in a packet field and creates a list of them.
     *  
     *      @param [in] errors	        List of found errors.
     *      @param [in] context	        Context to help locate the error.
     *      @param [in] packetField	    The packet field to check.
     */
    void findErrorsInPacketField(QStringList& errors, QString const& context, QSharedPointer<PacketField> packetField) const;

    /*!
     *	Checks the validity of a packet or packet field name.
     *  
     *      @param [in] name	The name to check.
     *		
     * 		@return True, if name is valid, otherwise false.
     */
    bool isValidName(QString const& name) const;

    /*!
     *	Parses the field width expression.
     *  
     *      @param [in] width	        The width value to parse.
     *      @param [in] widthIsValid	Indicates if the parsed width is valid.
     *		
     * 		@return Resolved width expression in bits.
     */
    QString parseFieldWidth(QString const& width, bool* widthIsValid) const;

    /*!
     *	Checks if the packet field contains a valid value.
     *  
     *      @param [in] packetField	    The packet field to check.
     *      @param [in] fieldWidth	    The width of the field in bits.
     *		
     * 		@return True, if the value is valid, otherwise false.
     */
    bool hasValidFieldValue(QSharedPointer<PacketField> packetField, QString const& fieldWidth) const;

    /*!
     *	Finds possible errors in a packet field value.
     *  
     *      @param [in] errors	                List of found errors.
     *      @param [in] context	                Context to help locate the error.
     *      @param [in] packetField	            The packet field to check.
     *      @param [in] resolvedFieldWidth      The width of the packet field in bits.
     */
    void findErrorsInPacketFieldValue(QStringList& errors, QString const& context,
        QSharedPointer<PacketField> packetField, QString const& resolvedFieldWidth) const;

    /*!
     *	Parses a constant field value.
     *  
     *      @param [in] token	Description
     *		
     * 		@return The resolved value expression in binary.
     */
    QString parseConstantFieldValue(QString token) const;

    /*!
     *  Get the base for a given number.
     *
     *      @param [in] constantNumber  The selected number.
     *
     *      @return The base for the selected number. Either 2, 8, 10 or 16.
     */
    int baseOf(QString const& constantNumber) const;

    //! The Expression parser used to validate expressions in packet field elements.
    QSharedPointer<ExpressionParser> expressionParser_;

};

#endif // PACKETVALIDATOR_H
