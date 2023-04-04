//-----------------------------------------------------------------------------
// File: SVDCPUEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 18.05.2021
//
// Description:
// Editor for CPU details in SVD generation.
//-----------------------------------------------------------------------------

#ifndef SVDCPUEDITOR_H
#define SVDCPUEDITOR_H

#include <QWidget>
#include <QObject>
#include <QTableView>

#include <Plugins/common/ConnectivityGraphUtilities.h>
#include <Plugins/common/CPUDialog/CPUEditor.h>

class SVDCPUModel;
class CPUDetailRoutes;
class SVDCPUDetailRoutes;

//-----------------------------------------------------------------------------
//! Editor for CPU details in SVD generation.
//-----------------------------------------------------------------------------
class SVDCPUEditor : public CPUEditor
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] parent  Pointer to the owner of this widget.
     */
    SVDCPUEditor(QWidget *parent = 0);

    /*!
     *  The destructor.
     */
    virtual ~SVDCPUEditor() = default;

    /*!
     *  Setup the selected CPUs.
     *
     *      @param [in] library     Interface for accessing the library.
     *      @param [in] component   The top component of the design.
     *      @param [in] activeView  Active view of the design.
     */
    virtual void setupCPUDetails(LibraryInterface* library, QSharedPointer<Component> component, QString const& activeView) override final;

    /*!
     *  Get a list of the selected CPU routes.
     *
     *      @return List of the selected CPU routes.
     */
    virtual QVector<QSharedPointer<CPUDetailRoutes> > getSelectedCPUs() const override final;

    //! No copying. No assignment.
    SVDCPUEditor(const SVDCPUEditor& other) = delete;
    SVDCPUEditor& operator=(const SVDCPUEditor& other) = delete;

private:

    /*!
     *  Create SVD CPU routes from the selected design.
     *
     *      @param [in] library     Interface for accessing the library.
     *      @param [in] component   The top component of the design.
     *      @param [in] activeView  Active view of the design.
     *
     *      @return The SVD CPU routes of the selected design.
     */
    QVector<QSharedPointer<SVDCPUDetailRoutes> > getSVDCPURoutes(LibraryInterface* library, QSharedPointer<Component> component, QString const& activeView);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The view for the table.
    QTableView* view_;

    //! The containing model.
    SVDCPUModel* model_;
};

#endif // SVDCPUEDITOR_H
