//-----------------------------------------------------------------------------
// File: AddressSubsection.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Joni-Matti M‰‰tt‰
// Date: 22.9.2012
//
// Description:
// Declares the address subsection class.
//-----------------------------------------------------------------------------

#ifndef ADDRESSSUBSECTION_H
#define ADDRESSSUBSECTION_H

#include <QGraphicsTextItem>

class AddressSectionItem;

//-----------------------------------------------------------------------------
//! Address subsection.
//-----------------------------------------------------------------------------
class AddressSubsection : public QObject
{
    Q_OBJECT

public:

    /*!
     *  Constructor.
     *  
     *      @param [in] parent          The parent section item.
     *      @param [in] top             The top y coordinate.
     *      @param [in] bottom          The bottom y coordinate.
     *      @param [in] startAddress    The start address.
     *      @param [in] endAddress      The end address.
     */
    AddressSubsection(AddressSectionItem* parent, int offsetX,
                      int top, int bottom,
                      unsigned int startAddress, unsigned int endAddress);

    /*!
     *  Destructor.
     */
    ~AddressSubsection();

    /*!
     *  Sets the top y coordinate.
     *  
     *      @param [in] top The top y coordinate.
     */
    void setTop(int top);

    /*!
     *  Sets the bottom y coordinate.
     *  
     *      @param [in] bottom  The bottom y coordinate.
     */
    void setBottom(int bottom);

    /*!
     *  Sets the start address.
     *
     *      @param [in] startAddress The start address to set.
     */
    void setStartAddress(unsigned int startAddress);

    /*!
     *  Sets the end address.
     *  
     *      @param [in] endAddress  The end address to set.
     */
    void setEndAddress(unsigned int endAddress);

    /*!
     *  Sets the start address fixed (i.e. can no longer be changed).
     *  
     *      @param [in] fixed   True if fixed.
     */
    void setStartAddressFixed(bool fixed);

    /*!
     *  Returns the top y coordinate.
     */
    int getTop() const;

    /*!
     *  Returns the bottom y coordinate.
     */
    int getBottom() const;

    /*!
     *  Returns the start address.
     */
    unsigned int getStartAddress() const;

    /*!
     *  Returns the end address.
     */
    unsigned int getEndAddress() const;

    /*!
     *  Returns the address range.
     */
    unsigned int getRange() const;

signals:
    //! Signaled when the start address has been edited.
    void startAddressEdited(AddressSubsection* subsection);

private slots:
    //! Called when the start address has been edited.
    void onStartAddressEdited();

private:
    // Disable copying.
    AddressSubsection(AddressSubsection const& rhs);
    AddressSubsection& operator=(AddressSubsection const& rhs);

    /*!
     *  Converts the address to a hexadecimal string.
     *  
     *      @param [in] address The address.
     *  
     *      @return Address as a string.
     */
    static QString toHexString(unsigned int address);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The top y coordinate for the subsection.
    int top_;

    //! The bottom y coordinate for the subsection.
    int bottom_;

    //! The start address.
    unsigned int startAddress_;

    //! The end address.
    unsigned int endAddress_;

    //! The label for start address.
    QGraphicsTextItem* startAddressLabel_;

    //! The label for end address.
    QGraphicsTextItem* endAddressLabel_;
};

//-----------------------------------------------------------------------------

#endif // ADDRESSSUBSECTION_H
