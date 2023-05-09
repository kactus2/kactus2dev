//-----------------------------------------------------------------------------
// File: CpuRoutesContainer.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 24.02.2023
//
// Description:
// Container for routes connected to a master interface.
//-----------------------------------------------------------------------------

#ifndef CPUROUTESCONTAINER_H
#define CPUROUTESCONTAINER_H

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
class CpuRoutesContainer
{

public:

    /*!
     *  The constructor.
     */
    CpuRoutesContainer();

    /*!
     *  The destructor.
     */
    virtual ~CpuRoutesContainer() = default;

    /*!
     *  Copy constructor.
     */
    CpuRoutesContainer(const CpuRoutesContainer& other);

    /*!
     *  Get the name.
     *
     *      @return Name of the CPU.
     */
    QString getFileName() const;

    /*!
     *  Set a new name.
     *
     *      @param [in] newName     The new name for the CPU.
     */
    void setFileName(QString const& newName);

    /*!
     *  Get the ID.
     *
     *      @return ID of the CPU.
     */
    QString getFileID() const;

    /*!
     *  Set a new ID.
     *
     *      @param [in] newID   The new ID.
     */
    void setFileID(QString const& newID);

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
    CpuRoutesContainer& operator=(CpuRoutesContainer const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Name of the container.
    QString fileName_;

    //! ID of the CPU.
    QString fileID_;

    //! Flag for creating an SVD file.
    bool createFile_;

    //! Routes connected to the CPU interface.
    QVector<QSharedPointer<CpuRouteStructs::CpuRoute> > routes_;
};

#endif // CPUROUTESCONTAINER_H