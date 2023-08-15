//-----------------------------------------------------------------------------
// File: PeripheralTemplateModel.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 24.02.2023
//
// Description:
// The model to manage the peripheral templates.
//-----------------------------------------------------------------------------

#ifndef PERIPHERALTEMPLATEMODEL_H
#define PERIPHERALTEMPLATEMODEL_H

#include <Plugins/RenodeGenerator/CPUDialog/RenodeStructs.h>

#include <QAbstractTableModel>

//-----------------------------------------------------------------------------
//! The model to manage the peripheral templates.
//-----------------------------------------------------------------------------
class PeripheralTemplateModel : public QAbstractTableModel
{
	Q_OBJECT

public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] parent  Pointer to the owner of this model.
	 */
	PeripheralTemplateModel(QObject *parent);
	
	/*!
     *  The destructor.
     */
    virtual ~PeripheralTemplateModel() = default;

    /*!
     *  Setup the peripheral templates.
     *
     *      @param [in] newTemplates	The selected peripheral templates.
     */
	void setupTemplates(QVector<QSharedPointer<RenodeStructs::peripheralTemplate> > newTemplates);

	/*!
	 *  Get the list of peripheral templates.
	 *
	 *      @return List of peripheral templates.
	 */
	QVector<QSharedPointer<RenodeStructs::peripheralTemplate> > getTemplates() const;

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
	PeripheralTemplateModel(const PeripheralTemplateModel& other) = delete;
	PeripheralTemplateModel& operator=(const PeripheralTemplateModel& other) = delete;

public slots:
	
	/*!
	 *  Add a new empty row to the model
	 */
	void onAddRow();

	/*!
	 *  A new item should be added to given index.
	 *
	 *      @param [in] index   The position where new item should be added at.
	 */
	void onAddItem(const QModelIndex& index);

	/*!
	 *  An item should be removed from the model.
	 *
	 *      @param [in] index   Identifies the item that should be removed.
	 */
	void onRemoveItem(const QModelIndex& index);

	/*!
	 *  Remove a row from the model.
	 *
	 *      @param [in] row Specifies the row to remove
	 */
	void onRemoveRow(int row);

private:

	/*!
	 *  Add a new item to the selected row.
	 *	
	 *      @param [in] newItemRow	The selected row.
	 */
	void addItem(int newItemRow);

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

    /*!
     *  Get the color for the selected index.
     *	
     *      @param [in] index   The selected index.
     *
     *      @return Color of the selected index. Black for valid index, red for invalid index.
     */
    QVariant blackForValidRedForInvalidIndex(QModelIndex const& index) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The peripheral template details being edited.
	QVector<QSharedPointer<RenodeStructs::peripheralTemplate> > templates_;
};

#endif // PERIPHERALTEMPLATEMODEL_H
