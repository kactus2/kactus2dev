//-----------------------------------------------------------------------------
// File: SVDCPUDetailRoutes.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 24.02.2023
//
// Description:
// Container for routes connected to a master interface for SVD generator.
//-----------------------------------------------------------------------------

#ifndef SVDCPUDETAILROUTES_H
#define SVDCPUDETAILROUTES_H

#include <Plugins/common/CPUDetailRoutes.h>

//-----------------------------------------------------------------------------
//! Container for routes connected to a master interface for SVD generator.
//-----------------------------------------------------------------------------
class SVDCPUDetailRoutes : public CPUDetailRoutes
{

public:

    /*!
     *  The constructor.
     */
    SVDCPUDetailRoutes();

    /*!
     *  The destructor.
     */
    virtual ~SVDCPUDetailRoutes() = default;

    /*!
     *  Creates an SVD CPU route from CPU route.
     *
     *      @return SVD CPU route.
     */
    SVDCPUDetailRoutes(CPUDetailRoutes const& other);

    /*!
     *  Get the revision.
     *
     *      @return The revision.
     */
    QString getRevision() const;

    /*!
     *  Set the revision.
     *
     *      @param [in] newRevision     The new revision.
     */
    void setRevision(QString const& newRevision);

    /*!
     *  Get the endian.
     *
     *      @return The endian.
     */
    QString getEndian() const;

    /*!
     *  Set the endian.
     *
     *      @param [in] newEndian   The new endian.
     */
    void setEndian(QString const& newEndian);

    /*!
     *  Check the value of MPU presence.
     *
     *      @return True, if MPU is present, otherwise false.
     */
    bool isMPUPresent() const;

    /*!
     *  Set the MPU presence.
     *
     *      @param [in] newPresence     The new MPU presence.
     */
    void setMPUPresence(bool newPresence);

    /*!
     *  Check the FPU presence.
     *
     *      @return True, if the FPU is present, false otherwise.
     */
    bool isFPUPresent() const;

    /*!
     *  Set the FPU presence.
     *
     *      @param [in] newPresence     The new FPU presence.
     */
    void setFPUPresence(bool newPresence);

    /*!
     *  Get the number of NVIC bits.
     *
     *      @return Number of NVIC bits.
     */
    QString getNVICPrioBits() const;

    /*!
     *  Set the number of NVIC bits.
     *
     *      @param [in] newBits     The new NVIC bits.
     */
    void setNVICPrioBits(QString const& newBits);

    /*!
     *  Check if the CPU has a vendor specific system tick timer.
     *
     *      @return True, if the CPU has a vendor specific system tick timer, false otherwise.
     */
    bool isVendorSystickConfig() const;

    /*!
     *  Set the flag for vendor specific system tick timer.
     *
     *      @param [in] newConfig   The new vendor specific system tick timer.
     */
    void setVendorSystickConfig(bool newConfig);

private:

    // Disable copying.
    SVDCPUDetailRoutes(SVDCPUDetailRoutes const& rhs);
    SVDCPUDetailRoutes& operator=(SVDCPUDetailRoutes const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Revision of the CPU.
    QString revision_;

    //! Endian data of the CPU.
    QString endian_;

    //! MPU presence of the CPU.
    bool mpuPresent_;

    //! FPU presence of the CPU.
    bool fpuPresent_;

    //! Number of NVIC bits of the CPU.
    QString nvicPrioBits_;

    //! Vendor-specific system tick timer of the CPU.
    bool vendorSystickConfig_;

};

#endif // SVDCPUDETAILROUTES_H