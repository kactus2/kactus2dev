//-----------------------------------------------------------------------------
// File: RenodeCPUDetailRoutes.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 02.03.2023
//
// Description:
// Container for routes connected to a master interface for Renode generator.
//-----------------------------------------------------------------------------

#ifndef RENODECPUDETAILROUTES_H
#define RENODECPUDETAILROUTES_H

#include <Plugins/common/CPUDetailRoutes.h>
#include <Plugins/RenodeGenerator/CPUDialog/RenodeStructs.h>

//-----------------------------------------------------------------------------
//! Container for routes connected to a master interface for Renode generator.
//-----------------------------------------------------------------------------
class RenodeCPUDetailRoutes : public CPUDetailRoutes
{

public:

    /*!
     *  The constructor.
     */
    RenodeCPUDetailRoutes();

    /*!
     *  The destructor.
     */
    virtual ~RenodeCPUDetailRoutes() = default;

    /*!
     *  Creates a Renode CPU route from CPU route.
     *
     *      @return Renode CPU route.
     */
    RenodeCPUDetailRoutes(CPUDetailRoutes const& other);

    /*!
     *  Set the peripheral data.
     *
     *      @param [in] newPeripherals  List of new peripherals.
     */
    void setPeripherals(QVector<QSharedPointer<RenodeStructs::cpuPeripherals> > newPeripherals);

    /*!
     *  Get the peripheral data.
     *
     *      @return List of peripheral data.
     */
    QVector<QSharedPointer<RenodeStructs::cpuPeripherals> > getPeripherals() const;

private:

    // Disable copying.
    RenodeCPUDetailRoutes(RenodeCPUDetailRoutes const& rhs);
    RenodeCPUDetailRoutes& operator=(RenodeCPUDetailRoutes const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! List of peripheral data.
    QVector<QSharedPointer<RenodeStructs::cpuPeripherals> > peripherals_;
};

#endif // RENODECPUDETAILROUTES_H