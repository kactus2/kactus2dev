//-----------------------------------------------------------------------------
// File: cpu.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 06.08.2010
//
// Description:
// Implementation of ipxact:cpu element.
//-----------------------------------------------------------------------------

#ifndef CPU_H
#define CPU_H

#include <IPXACTmodels/common/NameGroup.h>
#include <IPXACTmodels/common/Extendable.h>

#include <IPXACTmodels/Component/Region.h>
#include <IPXACTmodels/Component/ExecutableImage.h>

#include <QString>
#include <QStringList>
#include <QSharedPointer>

class Choice;
class Parameter;
class VendorExtension;

//-----------------------------------------------------------------------------
//! Implementation of ipxact:cpu element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT Cpu : public NameGroup, public Extendable
{

public:

    //! Cpu address space reference.
    struct IPXACTMODELS_EXPORT AddressSpaceRef
    {
        //! The address space reference.
        QString addressSpaceRef_;

        //! The presence.
        QString isPresent_;

        /*!
         *  The constructor.
         *
         *      @param [in] addressSpaceReference   The address space reference.
         */
        AddressSpaceRef(QString const& addressSpaceReference = QString());

        /*!
         *  Get the address space reference.
         *
         *      @return Address space reference.
         */
        QString getAddressSpaceRef() const;

        /*!
         *  Set the address space reference.
         *
         *      @param [in] newAddressSpaceRef  The new address space reference.
         */
        void setAddressSpaceRef(QString const& newAddressSpaceRef);

        /*!
         *  Get the is present.
         *
         *      @return Is present value.
         */
        QString getIsPresent() const;

        /*!
         *  Set the is present value.
         *
         *      @param [in] newIsPresent    The new is present value.
         */
        void setIsPresent(QString const& newIsPresent);
    };

    /*!
     *  Default constructor.
     *
     *      @param [in] name    Name of the cpu.
     */
    Cpu(QString const& name = QString());

	//! Copy constructor.
	Cpu(const Cpu &other);

	//! Assignment operator.
	Cpu &operator=(const Cpu &other);

	//! The destructor.
	~Cpu();

    /*!
     *  Gets the presence.
     *
     *      @return The presence value.
     */
    QString getIsPresent() const;

    /*!
     *  Set the presence.
     *
     *      @param [in] newIsPresent    The new presence value.
     */
    void setIsPresent(QString const& newIsPresent);

	/*!
     *  Get list of the address space references for this cpu.
	 *
	 *      @return A list containing the addressSpaceRefs.
	 */
    QStringList getAddressSpaceRefs() const;

    /*!
     *  Get the address space references.
     *
     *      @return List of address space references.
     */
    QSharedPointer<QList<QSharedPointer<Cpu::AddressSpaceRef> > > getAddressSpaceReferences() const;
          
    /*!
     *  Set the address space references.
     *
     *      @param [in] addrSpaceNames    The address space references to set.
     */  
    void setAddressSpaceRefs(QStringList addrSpaceNames);

    /*!
     *  Set the address space references.
     *
     *      @param [in] newAddressSpaceRefs     List containing the new address space references.
     */
    void setAddressSpaceReferences(QSharedPointer<QList<QSharedPointer<AddressSpaceRef> > > newAddressSpaceRefs);

    QString getRange() const;

    void setRange(QString const& range);

    QString getWidth() const;

    void setWidth(QString const& width);

    QString getMemoryMapReference() const;

    void setMemoryMapReference(QString const& memoryMapRef);

    QSharedPointer<QList<QSharedPointer<Region> > > getRegions() const;

    /*!
     *  Get the address unit bits
     *
     *      @return The address unit bits.
     */
    QString getAddressUnitBits() const;


    QSharedPointer<QList<QSharedPointer<ExecutableImage> > > getExecutableImages();

    void setAddressUnitBits(QString const& addressUnitBits);

	/*!
     *  Get the parameters of the cpu.
	 *
	 *      @return List containing pointers to the parameters for the cpu.
	 */
	QSharedPointer<QList<QSharedPointer<Parameter> > >  getParameters();

private:

    /*!
     *  Copy the parameters.
     *
     *      @param [in] other   The cpu being copied.
     */
    void copyParameters(const Cpu& other);

    /*!
     *  Copy the address space references.
     *
     *      @param [in] other   The cpu being copied.
     */
    void copyAddressSpaceRefs(const Cpu& other);

    void copyRegions(Cpu const& other);

    void copyExecutableImages(Cpu const& other);


    //! Presence of the CPU.
	QString isPresent_;

    //! References a name of an address space within component.
    QSharedPointer<QList<QSharedPointer<AddressSpaceRef> > > addressSpaceRefs_ =
        QSharedPointer<QList<QSharedPointer<AddressSpaceRef> > >(new QList<QSharedPointer<AddressSpaceRef> >);

    QString range_;

    QString width_;


    QSharedPointer<QList<QSharedPointer<Region> > > regions_ =
        QSharedPointer<QList<QSharedPointer<Region> > >(new QList<QSharedPointer<Region> >);

    QString addressUnitBits_;

    QSharedPointer<QList<QSharedPointer<ExecutableImage> > > executableImages_ =
        QSharedPointer<QList<QSharedPointer<ExecutableImage> > >(new QList<QSharedPointer<ExecutableImage> >);

    QString memoryMapReference_;

	//! Specifies any cpu-type parameters.
    QSharedPointer<QList<QSharedPointer<Parameter> > > parameters_ =
        QSharedPointer<QList<QSharedPointer<Parameter> > >(new QList<QSharedPointer<Parameter> >);

};

#endif // CPU_H
