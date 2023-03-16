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

//-----------------------------------------------------------------------------
//! Container for routes connected to a master interface.
//-----------------------------------------------------------------------------
class CPUDetailRoutes
{

public:

    /*!
     *  The constructor.
     */
    CPUDetailRoutes();

    /*!
     *  The destructor.
     */
    virtual ~CPUDetailRoutes() = default;

    /*!
     *  Copy constructor.
     */
    CPUDetailRoutes(const CPUDetailRoutes& other);

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
     *  Get the connectivity graph interface matching this CPU.
     *
     *      @return The connectivity graph interface.
     */
    QSharedPointer<const ConnectivityInterface> getCPUInterface() const;

    /*!
     *  Set the connectivity graph interface for this CPU.
     *
     *      @param [in] newCPUInterface     The selected connectivity graph interface.
     */
    void setCPUInterface(QSharedPointer<const ConnectivityInterface> newCPUInterface);

    /*!
     *  Get a list of the interface routes connected to this CPU.
     *
     *      @return List of interface routes connected to this CPU.
     */
    QSharedPointer<QVector < QVector<QSharedPointer<const ConnectivityInterface> > > > getRoutes() const;

    /*!
     *  Set new interface routes.
     *
     *      @param [in] newRoutes   The selected interface routes.
     */
    void setRoutes(QVector < QVector<QSharedPointer<const ConnectivityInterface> > > newRoutes);

private:

    // Disable copying.
    CPUDetailRoutes& operator=(CPUDetailRoutes const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Name of the CPU.
    QString cpuName_;

    //! ID of the CPU.
    QString cpuID_;

    //! Flag for creating an SVD file.
    bool createFile_;

    //! Interface containing the CPU.
    QSharedPointer<const ConnectivityInterface> cpuInterface_;

    //! Routes connected to the master interface.
    QSharedPointer<QVector < QVector<QSharedPointer<const ConnectivityInterface> > > > routes_;
};

#endif // CPUDETAILROUTES_H