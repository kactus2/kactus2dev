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

#include <Plugins/SVDGenerator/ConnectivityGraphUtilities.h>

class SVDCPUModel;

//-----------------------------------------------------------------------------
//! Editor for CPU details in SVD generation.
//-----------------------------------------------------------------------------
class SVDCPUEditor : public QWidget
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
     *      @param [in] cpuDetails  The selected CPUs.
     */
    void setupCPUDetails(QVector<QSharedPointer<ConnectivityGraphUtilities::cpuDetailRoutes> > cpuDetails);

    //! No copying. No assignment.
    SVDCPUEditor(const SVDCPUEditor& other) = delete;
    SVDCPUEditor& operator=(const SVDCPUEditor& other) = delete;

private:

    //! The view for the table.
    QTableView* view_;

    //! The containing model.
    SVDCPUModel* model_;
};

#endif // SVDCPUEDITOR_H
