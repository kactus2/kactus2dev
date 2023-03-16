//-----------------------------------------------------------------------------
// File: RenodePeripheralsEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 21.02.2023
//
// Description:
// Editor for CPU details in renode generation.
//-----------------------------------------------------------------------------

#ifndef RENODEPERIPHERALSEDITOR_H
#define RENODEPERIPHERALSEDITOR_H

#include <QWidget>
#include <QObject>
#include <QTableView>

// #include <Plugins/SVDGenerator/ConnectivityGraphUtilities.h>
#include <Plugins/common/ConnectivityGraphUtilities.h>
#include <Plugins/common/CPUDialog/CPUEditor.h>

// class SVDCPUModel;

class RenodePeripheralsModel;

class CPUDetailRoutes;

class LibraryInterface;
class Component;

class RenodeCPUDetailRoutes;

class RenodePeripheralsDelegate;

//-----------------------------------------------------------------------------
//! Editor for CPU details in renode generation.
//-----------------------------------------------------------------------------
// class SVDCPUEditor : public QWidget
class RenodePeripheralsEditor : public CPUEditor
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] parent  Pointer to the owner of this widget.
     */
    RenodePeripheralsEditor(QWidget *parent = 0);

    /*!
     *  The destructor.
     */
    virtual ~RenodePeripheralsEditor() = default;

    /*!
     *  Setup the selected CPUs.
     *
     *      @param [in] library     Interface for accessing the library.
     *      @param [in] component   Top component of the selected design.
     *      @param [in] activeView  Active view of the design.
     */
    virtual void setupCPUDetails(LibraryInterface* library, QSharedPointer<Component> component, QString const& activeView) override final;

    /*!
     *  Get the selected CPUs.
     *
     *      @return List of the selected CPUs.
     */
    virtual QVector<QSharedPointer<CPUDetailRoutes> > getSelectedCPUs() const override final;

    /*!
     *  Setup the folder path.
     *
     *      @param [in] newFolderPath   The new folder path.
     */
    virtual void setupFolderPath(QString const& newFolderPath) override final;

    //! No copying. No assignment.
    RenodePeripheralsEditor(const RenodePeripheralsEditor& other) = delete;
    RenodePeripheralsEditor& operator=(const RenodePeripheralsEditor& other) = delete;

private:

    //! The containing model.
    RenodePeripheralsModel* model_;

    //! The delegate.
    RenodePeripheralsDelegate* peripheralDelegate_;

    //! The selected CPU (WORK IN PROGRESS, SHOULD BE MULTIPLE CPUs).
    QSharedPointer<RenodeCPUDetailRoutes> renodeCPU_;
};

#endif // RENODEPERIPHERALSEDITOR_H
