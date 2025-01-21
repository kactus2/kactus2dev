//-----------------------------------------------------------------------------
// File: SingleCpuRoutesContainer.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 09.05.2023
//
// Description:
// Container for routes connected to a single CPU.
//-----------------------------------------------------------------------------

#ifndef SINGLECPUROUTESCONTAINER_H
#define SINGLECPUROUTESCONTAINER_H

#include <Plugins/common/CpuRoutesContainer.h>

//-----------------------------------------------------------------------------
//! Container for routes connected to a single CPU.
//-----------------------------------------------------------------------------
class SingleCpuRoutesContainer : public CpuRoutesContainer
{

public:

    /*!
     *  The constructor.
     * 
     *    @param [in] CPU     The CPU.
     */
    SingleCpuRoutesContainer(QSharedPointer<Cpu> cpu);

    /*!
     *  The destructor.
     */
    virtual ~SingleCpuRoutesContainer() = default;

    /*!
     *  Copy constructor.
     */
    SingleCpuRoutesContainer(const SingleCpuRoutesContainer& other);

    /*!
     *  Get the containing CPU.
     *
     *    @return The containing CPU.
     */
    QSharedPointer<Cpu> getCpu() const;

    /*!
     *  Set the containing CPU.
     *
     *    @param [in] newCpu  The new CPU.
     */
    void setCpu(QSharedPointer<Cpu> newCpu);

private:

    // Disable copying.
    SingleCpuRoutesContainer& operator=(SingleCpuRoutesContainer const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The containing CPU.
    QSharedPointer<Cpu> cpu_;
};

#endif // SINGLECPUROUTESCONTAINER_H