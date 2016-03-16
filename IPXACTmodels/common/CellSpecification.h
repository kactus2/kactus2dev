//-----------------------------------------------------------------------------
// File: CellSpecification.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 13.08.2015
//
// Description:
// Implementation for ipxact:cellSpecification.
//-----------------------------------------------------------------------------

#ifndef CELLSPECIFICATION_H
#define CELLSPECIFICATION_H

#include <QString>

//-----------------------------------------------------------------------------
//! Implementation for ipxact:cellSpecification.
//-----------------------------------------------------------------------------
class CellSpecification
{
public:

    //! Possible values for cell strength.
    enum CellStrengthType
    {
        LOW = 0,
        MEDIAN,
        HIGH,
        UNKNOWN
    };

    //! Possible values for cell class.
    enum CellClassType
    {
        COMBINATORIAL = 0,
        SEQUENTIAL,
        NO_CLASS
    };

    //! The constructor.
    CellSpecification();

    //! The copy constructor.
    CellSpecification(CellSpecification const& other);

    //! The destructor.
    ~CellSpecification();
        
    /*!
     *  Sets the cell strength.
     *
     *      @param [in] strength   The strength to set.
     */
    void setCellStrength(CellStrengthType strength);
        
    /*!
     *  Gets the cell strength.
     *
     *      @return The cell strength.
     */
    CellStrengthType getCellStrength() const;
            
    /*!
     *  Sets the cell function.
     *
     *      @param [in] function   The function to set.
     */
    void setCellFunction(QString const& function);
            
    /*!
     *  Gets the cell function.
     *
     *      @return The cell function.
     */
    QString getCellFunction() const;
                
    /*!
     *  Sets the cell class.
     *
     *      @param [in] cellClass   The class to set.
     */
    void setCellClass(CellClassType cellClass);
                
    /*!
     *  Gets the cell class.
     *
     *      @return The cell class.
     */
    CellClassType getCellClass() const;

private:
    // Disable assignment.
    CellSpecification& operator=(CellSpecification const& rhs);

    //! The cell strength.
    CellStrengthType cellStrength_;

    //! The cell function.
    QString function_;

    //! The cell class.
    CellClassType cellClass_;

};

#endif // CELLSPECIFICATION_H
