//-----------------------------------------------------------------------------
// File: DesignConfigurationInstantiationsModel.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 28.01.2016
//
// Description:
// Model for design configuration instantiations of a component.
//-----------------------------------------------------------------------------

#ifndef DesignConfigurationINSTANTIATIONSMODEL_H
#define DesignConfigurationINSTANTIATIONSMODEL_H

#include <IPXACTmodels/Component/Component.h>

#include <QAbstractTableModel>
#include <QList>
#include <QSharedPointer>

class InstantiationsValidator;
//-----------------------------------------------------------------------------
//! Model for design configuration instantiations of a component.
//-----------------------------------------------------------------------------
class DesignConfigurationInstantiationsModel : public QAbstractTableModel
{
	Q_OBJECT

public:

	/*!
     *  The constructor.
	 *
	 *      @param [in] component       The component being edited.
     *      @param [in] validator       The validator used for design configuration instantiations.
	 *      @param [in] parent          The owner of the model.
	 */
	DesignConfigurationInstantiationsModel(
        QSharedPointer<QList<QSharedPointer<DesignConfigurationInstantiation> > > instantiations,
        QSharedPointer<InstantiationsValidator> validator, QObject* parent);
	
	//! The destructor.
	virtual ~DesignConfigurationInstantiationsModel();

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
	virtual Qt::ItemFlags flags(QModelIndex const& index) const;

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
	virtual bool setData(QModelIndex const& index, const QVariant& value, int role = Qt::EditRole);

     /*!
     *  Returns the supported actions of a drop.
     *
     *      @return The drop actions supported by the model.
     */
    virtual Qt::DropActions supportedDropActions() const;

    /*!
     *  Returns a list of supported MIME data types.
     *
     *      @return The supported MIME types.
     */
    virtual QStringList mimeTypes() const;

    /*!
     *  Handler for the dropped MIME data.
     *
     *      @param [in] data   The data associated to the drop.
     *      @param [in] action The drop action.  
     *      @param [in] row    The row beneath the drop position.
     *      @param [in] column The column beneath the drop position.
     *      @param [in] parent The parent index of the drop position.
     *
     *      @return True, if the model could handle the data, otherwise false.
     */
    virtual bool dropMimeData(QMimeData const* data, Qt::DropAction action, int row, int column, 
        QModelIndex const& parent);

public slots:

	/*!
     *  Add a new item to the given index.
	 *
	 *      @param [in] index   The index identifying the position for new item.
	 */
	virtual void onAddItem(QModelIndex const& index);

	/*!
     *  Remove the item in the given index.
	 *
	 *      @param [in] index   The index identifying the item to remove.
	 */
	virtual void onRemoveItem(QModelIndex const& index);

signals:

	//! Emitted when the contents of the model change.
	void contentChanged();

	//! Emitted when a new design configuration instantiation is added to the given index.
	void designConfigurationInstantiationAdded(int index);

	//! Emitted when a design configuration instantiation is removed from the given index.
	void designConfigurationInstantiationRemoved(int index);

private:
	//! No copying.
	DesignConfigurationInstantiationsModel(const DesignConfigurationInstantiationsModel& other);
	DesignConfigurationInstantiationsModel& operator=(const DesignConfigurationInstantiationsModel& other);

	//! Contains the instantiations to edit.
    QSharedPointer<QList<QSharedPointer<DesignConfigurationInstantiation> > > instantiations_;

    //! The validator used for design configurations.
    QSharedPointer<InstantiationsValidator> validator_;
};

#endif // DesignConfigurationINSTANTIATIONSMODEL_H
