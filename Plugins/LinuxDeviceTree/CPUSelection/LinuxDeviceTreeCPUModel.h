//-----------------------------------------------------------------------------
// File: LinuxDeviceTreeCPUModel.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 15.10.2021
//
// Description:
// The model to manage the Linux Device Tree CPU details.
//-----------------------------------------------------------------------------

#ifndef LINUXDEVICETREECPUMODEL_H
#define LINUXDEVICETREECPUMODEL_H

#include <Plugins/LinuxDeviceTree/CPUSelection/LinuxDeviceTreeCPUDetails.h>

#include <QAbstractTableModel>
#include <QSharedPointer>

class LinuxDeviceTreeCpuRoutesContainer;

//-----------------------------------------------------------------------------
//! The model to manage the Linux Device Tree CPU details.
//-----------------------------------------------------------------------------
class LinuxDeviceTreeCPUModel : public QAbstractTableModel
{
	Q_OBJECT

public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] parent  Pointer to the owner of this model.
	 */
    LinuxDeviceTreeCPUModel(QObject *parent);
	
	/*!
     *  The destructor.
     */
    virtual ~LinuxDeviceTreeCPUModel() = default;

    /*!
     *  Setup the selected CPU containers.
     *
     *      @param [in] cpuDetails  The selected CPU containers.
     */
    void setupCPUDetails(QVector<QSharedPointer<LinuxDeviceTreeCpuRoutesContainer> > cpuDetails);

    /*!
     *  Get the selected CPU containers.
     *
     *      @return List of CPU containers that should be written into files.
     */
    QVector<QSharedPointer<LinuxDeviceTreeCpuRoutesContainer> > getCPUDetails() const;

	/*!
     *  Get the number of rows an item contains.
	 *
	 *      @param [in] parent  Identifies the parent that's row count is requested.
	 *
	 *      @return Number of rows the item has.
	 */
	virtual int rowCount(QModelIndex const& parent = QModelIndex()) const;

	/*!
     *  Get the number of columns the item has to be displayed.
	 *
	 *      @param [in] parent  Identifies the parent that's column count is requested.
	 *
	 *      @return The number of columns to be displayed.
	 */
	virtual int columnCount(QModelIndex const& parent = QModelIndex()) const;

	/*!
     *  Get the item flags that defines the possible operations for the item.
	 *
	 *      @param [in] index   Model index that identifies the item.
	 *
	 *      @return Qt::ItemFlags specify the possible operations for the item.
	 */
	Qt::ItemFlags flags(QModelIndex const& index) const;

	/*!
     *  Get the header data for specified header.
	 *
	 *      @param [in] section         The section specifies the row/column number for the header.
	 *      @param [in] orientation     Specified if horizontal or vertical header is wanted.
	 *      @param [in] role            Specifies the type of the requested data.
	 *
	 *      @return QVariant Contains the requested data.
	 */
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

	/*!
     *  Get the data for specified item.
	 *
	 *      @param [in] index   Specifies the item that's data is requested.
	 *      @param [in] role    The role that defines what kind of data is requested.
	 *
	 *      @return QVariant Contains the data for the item.
	 */
	virtual QVariant data(QModelIndex const& index, int role = Qt::DisplayRole) const;

	/*!
     *  Save the data to the model for specified item.
	 *
	 *      @param [in] index   The model index of the item that's data is to be saved.
	 *      @param [in] value   The data that is to be saved.
	 *      @param [in] role    The role specifies what kind of data should be saved.
	 *
	 *      @return True if saving happened successfully.
	 */
	bool setData(QModelIndex const& index, const QVariant& value, int role = Qt::EditRole);
    
    //! No copying. No assignment.
    LinuxDeviceTreeCPUModel(const LinuxDeviceTreeCPUModel& other) = delete;
    LinuxDeviceTreeCPUModel& operator=(const LinuxDeviceTreeCPUModel& other) = delete;

private:
	
    /*!
     *  Gets the value for the given index.
     *
     *      @param [in] index   The index of the target data.
     *
     *      @return The data in the given index.
     */
    QVariant valueForIndex(QModelIndex const& index) const;

    /*!
     *  Gets the tooltip value for the given index.
     *
     *      @param [in] index   The index of the target data.
     *
     *      @return The tooltip data in the given index.
     */
    QVariant tooltipForIndex(QModelIndex const& index) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The CPU containers being edited.
    QVector<QSharedPointer<LinuxDeviceTreeCpuRoutesContainer> > cpus_;
};

#endif // LINUXDEVICETREECPUMODEL_H
