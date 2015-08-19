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

#include <QObject>

//-----------------------------------------------------------------------------
//! Implementation for ipxact:qualifier group.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT Qualifier 
{
public:

    //! Possible qualifier types.
    enum Type
    {
        Data = 0,
        Address,
        Clock,
        Reset,
        Data_Address,
        Any
    };

	//! The constructor.
	Qualifier();

    //! The copy constructor.
    Qualifier(Qualifier const& rhs);

	//! The destructor.
	virtual ~Qualifier();

    //! The assign operator.
    Qualifier& operator=(Qualifier const& rhs);

    /*!
     *  Sets the qualifier.
     *
     *      @param [in] qualifierType   The qualifier to set.
     */
    void setType(Qualifier::Type qualifierType);

    /*!
     *  Checks if the qualifier is set.
     *
     *      @return True, if the qualifier is set a value, otherwise false.
     */
    bool isSet();

    /*!
     *  Checks if the qualifier is set as address.
     *
     *      @return True, if the qualifier is set as address, otherwise false.
     */
    bool isAddress() const;
    
    /*!
     *  Checks if the qualifier is set as data.
     *
     *      @return True, if the qualifier is set as data, otherwise false.
     */
    bool isData() const;
    
    /*!
     *  Checks if the qualifier is set as clock.
     *
     *      @return True, if the qualifier is set as clock, otherwise false.
     */
    bool isClock() const;
    
    /*!
     *  Checks if the qualifier is set as reset.
     *
     *      @return True, if the qualifier is set as reset, otherwise false.
     */
    bool isReset() const;

private:

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Flag for address qualifier.
    bool isAddress_;

    //! Flag for data qualifier.
    bool isData_;

    //! Flag for clock qualifier.
    bool isClock_;

    //! Flag for reset qualifier.
    bool isReset_;
};

#endif // QUALIFIER_H
