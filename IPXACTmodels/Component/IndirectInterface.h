//-----------------------------------------------------------------------------
// File: IndirectInterface.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 02.08.2017
//
// Description:
// Implementation of the ipxact:indirectInterface element.
//-----------------------------------------------------------------------------

#ifndef INDIRECTINTERFACE_H
#define INDIRECTINTERFACE_H

#include <IPXACTmodels/common/Extendable.h>
#include <IPXACTmodels/common/NameGroup.h>
#include <IPXACTmodels/common/Parameter.h>

#include <QSharedPointer>
#include <QVector>

class TransparentBridge;

//-----------------------------------------------------------------------------
//! Implementation of the ipxact:indirectInterface element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT IndirectInterface : public NameGroup, public Extendable
{
public:

	//! The constructor.
	IndirectInterface(QString const& name = QString());

    //! Copy constructor.
    IndirectInterface(IndirectInterface const& other);

    // Disable assignment.
    IndirectInterface& operator=(IndirectInterface const& rhs) = delete;

	//! The destructor.
	virtual ~IndirectInterface() = default;

	/*!
     *  Get the attributes of the indirect interface.
	 *
	 *      @return The attributes.
 	 */
	QMap<QString, QString> getAttributes() const;

	/*!
     *  Set the attributes for the indirect interface.
	 * 
	 *      @param [in] attributes  The attributes to set.
	 */
	void setAttributes(QMap<QString, QString> const& attributes);

    /*!
     *  Sets the reference for the indirect access address field.
     *
     *      @param [in] addressFieldId      The id for the address field.     
     */
    void setIndirectAddressRef(QString const& addressFieldId);

    /*!
     *  Gets the reference for the indirect access address field.
     *
     *      @return The indirect address field id.
     */
    QString getIndirectAddressRef() const;

    /*!
     *  Sets the reference for the indirect access data field.
     *
     *      @param [in] dataFieldId         The id for the data field.     
     */    
    void setIndirectDataRef(QString const& dataFieldId);
    
    /*!
     *  Gets the reference for the indirect access data field.
     *
     *      @return The indirect data field id.
     */  
    QString getIndirectDataRef() const;
    
    /*!
     *  Sets the indirectly accessible memory map.
     *
     *      @param [in] memoryMapName   The name of the accessible memory map.
     */
    void setMemoryMapRef(QString const& memoryMapName);
    
    /*!
     *  Gets the indirectly accessible memory map.
     *
     *      @return The name of the indirectly accessible memory map.
     */
    QString getMemoryMapRef() const;

   /*!
     *  Gets the transparent bridges in the indirect interface.
     *
     *      @return The transparent bridges.
     */
    QSharedPointer<QList<QSharedPointer<TransparentBridge> > > getTransparentBridges() const;

    /*!
     *  Sets the number of bits in the least addressable unit.
     *
     *      @param [in] bitsInLau   The number of bits to set.
     */
    void setBitsInLau(QString const& bitsInLau);

    /*!
     *  Gets the number of bits in the least addressable unit.
     *
     *      @return   The number of bits in the least addressable unit.
     */
    QString getBitsInLau() const;

    /*!
     *  Sets the endianness (little/big) for the indirect interface.
     *
     *      @param [in] endianness   The endianness to set.
     */
    void setEndianness(QString const& endianness);

    /*!
     *  Gets the endianness (little/big) for the indirect interface.
     *
     *      @return The endianness of the indirect interface.
     */
    QString getEndianness() const;

    /*!
     *  Gets the parameters in the indirect interface.
     *
     *      @return The parameters of the indirect interface.
     */
    QSharedPointer<QList<QSharedPointer<Parameter> > > getParameters() const;

private:

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Contains the attributes for the indirect interface.
    QMap<QString, QString> attributes_;

    //! Reference to the indirect address field.
    QString indirectAddressRef_;

    //! Reference to the indirect data field.
    QString indirectDataRef_;

    //! Reference to the indirectly accessible memory map.
    QString memoryMapRef_;

    //! The indirect transparent bridges.
    QSharedPointer<QList<QSharedPointer<TransparentBridge> > > transparentBridges_;

    //! The number of bits in least addressable unit.
    QString bitsInLau_;

    //! The endianness of the indirect interface.
    QString endianness_;

    //! Parameters of the indirect interface.
    QSharedPointer<QList<QSharedPointer<Parameter> > > parameters_;
};

#endif // INDIRECTINTERFACE_H
