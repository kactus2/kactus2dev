//-----------------------------------------------------------------------------
// File: RenodeCpuRoutesContainer.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 02.03.2023
//
// Description:
// Container for routes connected to a master interface for Renode generator.
//-----------------------------------------------------------------------------

#ifndef RENODECPUROUTESCONTAINER_H
#define RENODECPUROUTESCONTAINER_H

#include <Plugins/common/SingleCpuRoutesContainer.h>
#include <Plugins/RenodeGenerator/CPUDialog/RenodeStructs.h>

//-----------------------------------------------------------------------------
//! Container for routes connected to a master interface for Renode generator.
//-----------------------------------------------------------------------------
class RenodeCpuRoutesContainer : public SingleCpuRoutesContainer
{

public:

    /*!
     *  The constructor.
     *
     *    @param [in] renodeCpu   CPU for this container.
     */
    RenodeCpuRoutesContainer(QSharedPointer<Cpu> renodeCpu);

    /*!
     *  The destructor.
     */
    virtual ~RenodeCpuRoutesContainer() = default;

    /*!
     *  Creates a Renode CPU route from CPU route.
     *
     *    @return Renode CPU route.
     */
    RenodeCpuRoutesContainer(SingleCpuRoutesContainer const& other);

    /*!
     *  Set the peripheral data.
     *
     *    @param [in] newPeripherals  List of new peripherals.
     */
    void setPeripherals(QVector<QSharedPointer<RenodeStructs::cpuPeripheral> > newPeripherals);

    /*!
     *  Get the peripheral data.
     *
     *    @return List of peripheral data.
     */
    QVector<QSharedPointer<RenodeStructs::cpuPeripheral> > getPeripherals() const;

    /*!
     *  Set the available memory data.
     *
     *    @param [in] newMemories     List of new memory data.
     */
    void setMemories(QVector<QSharedPointer<RenodeStructs::cpuMemories> > newMemories);

    /*!
     *  Get the memory data.
     *
     *    @return List of the memory data.
     */
    QVector<QSharedPointer<RenodeStructs::cpuMemories> > getMemories() const;

    /*!
     *  Set a class name for the CPU.
     *
     *    @param [in] newClass    The new class name.
     */
    void setClassName(QString const& newClass);

    /*!
     *  Get the CPU class name.
     *
     *    @return The CPU class name.
     */
    QString getClassName();

    /*!
     *  Set the CPU type.
     *
     *    @param [in] newType     The new CPU type.
     */
    void setCpuType(QString const& newType);

    /*!
     *  Get the CPU type.
     *
     *    @return The CPU type.
     */
    QString getCpuType();

    /*!
     *  Set a new time provider.
     *
     *    @param [in] newTimeProvider     The new time provider.
     */
    void setTimeProvider(QString const& newTimeProvider);

    /*!
     *  Get the CPU time provider.
     *
     *    @return The CPU time provider.
     */
    QString getTimeProvider();

private:

    // Disable copying.
    RenodeCpuRoutesContainer(RenodeCpuRoutesContainer const& rhs);
    RenodeCpuRoutesContainer& operator=(RenodeCpuRoutesContainer const& rhs);

    /*!
     *  Initialize the CPU data.
     */
    void initializeCpuData();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! List of available peripheral data.
    QVector<QSharedPointer<RenodeStructs::cpuPeripheral> > peripherals_;

    //! List of available memory data.
    QVector<QSharedPointer<RenodeStructs::cpuMemories> > memories_;

    //! Class name of the CPU.
    QString className_;

    //! Type of the CPU.
    QString cpuType_;

    //! CPU time provider.
    QString timeProvider_;
};

#endif // RENODECPUROUTESCONTAINER_H