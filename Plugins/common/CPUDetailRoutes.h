//-----------------------------------------------------------------------------
// File: CPUDetailRoutes.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 24.02.2023
//
// Description:
// Container for routes connected to a master interface.
//-----------------------------------------------------------------------------

#ifndef CPUDETAILROUTES_H
#define CPUDETAILROUTES_H

#include <editors/MemoryDesigner/ConnectivityInterface.h>

#include <QString>
#include <QSharedPointer>

class Cpu;

//! CPU route utilities.
namespace CpuRouteStructs
{
    //! Container for CPU route details.
    struct CpuRoute
    {
        //! Interface containing the CPU.
        QSharedPointer<const ConnectivityInterface> cpuInterface_;

        //! Routes connected to the master cpu interface.
        QVector<QVector<QSharedPointer<const ConnectivityInterface> > > routes_;
    };
};

//-----------------------------------------------------------------------------
//! Container for routes connected to a master interface.
//-----------------------------------------------------------------------------
class CPUDetailRoutes
{

public:

    /*!
     *  The constructor.
     * 
     *      @param [in] CPU     The CPU.
     */
    CPUDetailRoutes(QSharedPointer<Cpu> cpu);

    /*!
     *  The destructor.
     */
    virtual ~CPUDetailRoutes() = default;

    /*!
     *  Copy constructor.
     */
    CPUDetailRoutes(const CPUDetailRoutes& other);

    /*!
     *  Get the containing CPU.
     *
     *      @return The containing CPU.
     */
    QSharedPointer<Cpu> getCpu() const;

    /*!
     *  Set the containing CPU.
     *
     *      @param [in] newCpu  The new CPU.
     */
    void setCpu(QSharedPointer<Cpu> newCpu);

    /*!
     *  Get the name.
     *
     *      @return Name of the CPU.
     */
    QString getCPUName() const;

    /*!
     *  Set a new name.
     *
     *      @param [in] newName     The new name for the CPU.
     */
    void setCPUName(QString const& newName);

    /*!
     *  Get the ID.
     *
     *      @return ID of the CPU.
     */
    QString getCPUID() const;

    /*!
     *  Set a new ID.
     *
     *      @param [in] newID   The new ID.
     */
    void setCPUID(QString const& newID);

    /*!
     *  Flag for creating a file from this CPU.
     *
     *      @return True, if a file should be created, false otherwise.
     */
    bool shouldCreateFile() const;

    /*!
     *  Set the flag fro creating a file from this CPU.
     *
     *      @param [in] newFlag     The new flag.
     */
    void setCreateFileFlag(bool newFlag);

    /*!
     *  Get a list of the interface routes connected to this CPU.
     *
     *      @return List of interface routes connected to this CPU.
     */
    QVector <QSharedPointer<CpuRouteStructs::CpuRoute> > getRoutes() const;

    /*!
     *  Set new interface routes.
     *
     *      @param [in] newRoutes   The selected interface routes.
     */
    void setRoutes(QVector <QSharedPointer<CpuRouteStructs::CpuRoute> > newRoutes);

    /*!
     *  Add a new CPU route.
     *
     *      @param [in] newRoute    The selected interface route.
     */
    void addRoute(QSharedPointer<CpuRouteStructs::CpuRoute> newRoute);

private:

    // Disable copying.
    CPUDetailRoutes& operator=(CPUDetailRoutes const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The containing CPU.
    QSharedPointer<Cpu> cpu_;

    //! Name of the CPU.
    QString cpuName_;

    //! ID of the CPU.
    QString cpuID_;

    //! Flag for creating an SVD file.
    bool createFile_;

    //! Routes connected to the CPU interface.
    QVector<QSharedPointer<CpuRouteStructs::CpuRoute> > routes_;
};

#endif // CPUDETAILROUTES_H