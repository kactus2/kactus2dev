//-----------------------------------------------------------------------------
// File: PortAlignment.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 27.01.2016
//
// Description:
// Implementation of port alignment class.
//-----------------------------------------------------------------------------

#ifndef PORTALIGNMENT_H
#define PORTALIGNMENT_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QString>

//-----------------------------------------------------------------------------
//! Implementation of port alignment class.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT PortAlignment
{
public:

    /*!
     *  The constructor.
     *
     *      @param [in] leftAlignment   The higher alignment of the port.
     *      @param [in] right           The lower alignment of the port.
     */
    PortAlignment(int leftAlignment = 0, int rightAlignment = 0);

    //! The copy constructor.
    PortAlignment(PortAlignment const& other);

	/*!
	 *  The destructor.
	*/
    ~PortAlignment();

    /*!
     *  Get the higher (left) alignment of the port.
     *
     *      @return The higher (left) alignment of the port.
     */
    int getLeftAlignment() const;

    /*!
     *  Set the higher (left) alignment of the port.
     *
     *      @param [in] newLeftAlignment    The new higher (left) alignment.
     */
    void setLeftAlignment(int newLeftAlignment);

    /*!
     *  Get the lower (right) alignment of the port.
     *
     *      @return The lower (right) alignment of the port.
     */
    int getRightAlignment() const;

    /*!
     *  Set the lower (right) alignment of the port.
     *
     *      @param [in] newRightAlignment   The new lower (right) alignment.
     */
    void setRightAlignment(int newRightAlignment);

    /*!
     *  Check if the port alignment is valid.
     *
     *      @return True, if the alignment is valid, false otherwise.
     */
    bool isValid() const;

    /*!
     *  Set the validity of the port alignment.
     *
     *      @param [in] validity    The new validity of the port alignment.
     */
    void setAlignmentValidity(bool validity);

private:

	// Disable assignment.
    PortAlignment& operator=(PortAlignment const& rhs);
    
	//-----------------------------------------------------------------------------
	// Data.
	//-----------------------------------------------------------------------------

    //! The higher (left) bound of the alignment.
    int leftAlignment_;

    //! The lower (right) bound of the alignment.
    int rightAlignment_;

    //! The validity of the alignment.
    bool validAlignment_;
};
#endif // PORTALIGNMENT_H
