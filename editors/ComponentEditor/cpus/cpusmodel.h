//-----------------------------------------------------------------------------
// File: cpusmodel.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 14.06.2012
//
// Description:
//! Model for cpu elements within a component.
//-----------------------------------------------------------------------------

#ifndef CPUSMODEL_H
#define CPUSMODEL_H

#include <QAbstractTableModel>
#include <QSharedPointer>
#include <QList>

class Component;
class Cpu;
class CPUValidator;

//-----------------------------------------------------------------------------
//! Model for cpu elements within a component.
//-----------------------------------------------------------------------------
class CpusModel : public QAbstractTableModel
{
	Q_OBJECT

public:

	/*! The constructor
	 *
	 *      @param [in] component   The component being edited.
     *      @param [in] validator   The validator for cpus.
	 *      @param [in] parent      The owner of this model.
	*/
	CpusModel(QSharedPointer<Component> component, QSharedPointer<CPUValidator> validator,
        QObject* parent);
	
	//! The destructor
	virtual ~CpusModel();

	/*! Get the number of rows an item contains.
	 *
	 *      @param [in] parent Identifies the parent that's row count is requested.
	 *
	 *      @return Number of rows the item has.
	*/
	virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;

	/*! Get the number of columns the item has to be displayed.
	 *
	 *      @param [in] parent Identifies the parent that's column count is requested.
	 *
	 *      @return The number of columns to be displayed.
	*/
	virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;

	/*! Get the item flags that defines the possible operations for the item.
	 *
	 *      @param [in] index Model index that identifies the item.
	 *
	 *      @return Qt::ItemFlags specify the possible operations for the item.
	*/
	Qt::ItemFlags flags(const QModelIndex& index) const;

	/*! Get the header data for specified header.
	 *
	 *      @param [in] section The section specifies the row/column number for the header.
	 *      @param [in] orientation Specified if horizontal or vertical header is wanted.
	 *      @param [in] role Specifies the type of the requested data.
	 *
	 *      @return QVariant Contains the requested data.
	*/
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

	/*! Get the data for specified item.
	 *
	 *      @param [in] index Specifies the item that's data is requested.
	 *      @param [in] role The role that defines what kind of data is requested.
	 *
	 *      @return QVariant Contains the data for the item.
	*/
	virtual QVariant data(QModelIndex const& index, int role = Qt::DisplayRole) const;

	/*! Save the data to the model for specified item
	 *
	 *      @param [in] index The model index of the item that's data is to be saved.
	 *      @param [in] value The data that is to be saved.
	 *      @param [in] role The role specifies what kind of data should be saved.
	 *
	 *      @return True if saving was successfull.
	*/
	bool setData(QModelIndex const& index, QVariant const& value, int role = Qt::EditRole);

public slots:

	/*! Add a new item to the given index.
	 *
	 *      @param [in] index The index identifying the position for new item.
	*/
	virtual void onAddItem(const QModelIndex& index);

	/*! Remove the item in the given index.
	 *
	 *      @param [in] index The index identifying the item to remove.
	*/
	virtual void onRemoveItem(const QModelIndex& index);

signals:

	//! Emitted when the contents of the model change.
	void contentChanged();

	/*! Emitted when a new cpu is added to the model.
	 *
	 *      @param [in] index The index of the added cpu.
	*/
	void cpuAdded(int index);

	/*! Emitted when a cpu is removed from the model.
	 *
	 *      @param [in] index The index of the removed cpu.
	*/
	void cpuRemoved(int index);

private:
	//! No copying
	CpusModel(const CpusModel& other);
	CpusModel& operator=(const CpusModel& other);

	//! The component being edited.
	QSharedPointer<Component> component_;

	//! Contains the cpus being edited.
	QSharedPointer<QList<QSharedPointer<Cpu> > > cpus_;

    QSharedPointer<CPUValidator> validator_;
};

#endif // CPUSMODEL_H
