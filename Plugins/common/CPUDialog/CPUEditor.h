//-----------------------------------------------------------------------------
// File: CPUEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 17.02.2023
//
// Description:
// Editor for CPU details in generators.
//-----------------------------------------------------------------------------

#ifndef CPUEDITOR_H
#define CPUEDITOR_H

#include <QWidget>
#include <QObject>
#include <QTableView>

#include <Plugins/common/ConnectivityGraphUtilities.h>

class CPUDetailRoutes;
class LibraryInterface;
class Component;

//-----------------------------------------------------------------------------
//! Editor for CPU details in generators.
//-----------------------------------------------------------------------------
class CPUEditor : public QWidget
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] parent  Pointer to the owner of this widget.
     */
    CPUEditor(QWidget *parent = 0);

    /*!
     *  The destructor.
     */
    virtual ~CPUEditor() = default;

    /*!
     *  Setup the selected CPUs.
     *
     *      @param [in] library     Interface for accessing the library.
     *      @param [in] component   Top component of the selected design.
     *      @param [in] activeView  Active view of the design.
     */
    virtual void setupCPUDetails(LibraryInterface* library, QSharedPointer<Component> component, QString const& activeView) = 0;

    /*!
     *  Get the selected CPUs.
     *
     *      @return List of the selected CPUs.
     */
    virtual QVector<QSharedPointer<CPUDetailRoutes> > getSelectedCPUs() const = 0;

    /*!
     *  Setup the folder path.
     *
     *      @param [in] newFolderPath   The new folder path.
     */
    virtual void setupFolderPath(QString const& newFolderPath);

    //! No copying. No assignment.
    CPUEditor(const CPUEditor& other) = delete;
    CPUEditor& operator=(const CPUEditor& other) = delete;

signals:

    /*!
     *  Inform of a change in the destination folder path.
     *
     *      @param [in] newPath     The new folder path.
     */
    void changeInSelectedPath(QString const& newPath);

private:
    
    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------
};

#endif // CPUEDITOR_H
