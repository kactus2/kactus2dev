//-----------------------------------------------------------------------------
// File: LinuxDeviceTreeCPUEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 14.10.2021
//
// Description:
// Editor for CPU details in Linux Device Tree generation.
//-----------------------------------------------------------------------------

#ifndef LINUXDEVICETREECPUEDITOR_H
#define LINUXDEVICETREECPUEDITOR_H

#include <QWidget>
#include <QObject>
#include <QTableView>

#include <Plugins/LinuxDeviceTree/CPUSelection/LinuxDeviceTreeCPUDetails.h>

class LinuxDeviceTreeCPUModel;

//-----------------------------------------------------------------------------
//! Editor for CPU details in Linux Device Tree generation.
//-----------------------------------------------------------------------------
class LinuxDeviceTreeCPUEditor : public QWidget
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *    @param [in] parent  Pointer to the owner of this widget.
     */
    LinuxDeviceTreeCPUEditor(QWidget *parent = 0);

    /*!
     *  The destructor.
     */
    virtual ~LinuxDeviceTreeCPUEditor() = default;

    /*!
     *  Setup the selected CPU containers.
     *
     *    @param [in] cpuDetails  The selected CPU containers.
     */
    void setupCPUDetails(QVector<QSharedPointer<LinuxDeviceTreeCpuRoutesContainer> > cpuDetails);

    /*!
     *  Get the selected CPU containers.
     *
     *    @return The selected CPU containers.
     */
    QVector<QSharedPointer<LinuxDeviceTreeCpuRoutesContainer> > getAcceptedContainers() const;

    //! No copying. No assignment.
    LinuxDeviceTreeCPUEditor(const LinuxDeviceTreeCPUEditor& other) = delete;
    LinuxDeviceTreeCPUEditor& operator=(const LinuxDeviceTreeCPUEditor& other) = delete;

private:

    //! The view for the table.
    QTableView* view_;

    //! The containing model.
    LinuxDeviceTreeCPUModel* model_;
};

#endif // LINUXDEVICETREECPUEDITOR_H
