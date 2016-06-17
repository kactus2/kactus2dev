//-----------------------------------------------------------------------------
// File: enumeratedvaluemodel.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 28.08.2012
//
// Description:
// The model to manage the enumerated values of a field.
//-----------------------------------------------------------------------------

#ifndef ENUMERATEDVALUEMODEL_H
#define ENUMERATEDVALUEMODEL_H

#include <QAbstractTableModel>
#include <QSharedPointer>

class EnumeratedValue;
class EnumeratedValueValidator;

//-----------------------------------------------------------------------------
//! The model to manage the enumerated values of a field.
//-----------------------------------------------------------------------------
class EnumeratedValueModel : public QAbstractTableModel
{
	Q_OBJECT

public:

	/*!
     *  The constructor.
	 *
	 *      @param [in] enumeratedValues            The enumerated values being edited.
     *      @param [in] enumeratedValueValidator    The validator used for enumerated values.
	 *      @param [in] parent                      Pointer to the owner of the model.
	 */
    EnumeratedValueModel(QSharedPointer<QList<QSharedPointer<EnumeratedValue> > > enumeratedValues,
        QSharedPointer<EnumeratedValueValidator> enumeratedValueValidator, QObject *parent);
	
	//! The destructor.
	virtual ~EnumeratedValueModel();

	/*!
     *  Get the number of rows an item contains.
	 *
	 *      @param [in] parent  Identifies the parent that's row count is requested.
	 *
	 *      @return Number of rows the item has.
	 */
	virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;

	/*!
     *  Get the number of columns the item has to be displayed.
	 *
	 *      @param [in] parent  Identifies the parent that's column count is requested.
	 *
	 *      @return The number of columns to be displayed.
	 */
	virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;

	/*!
     *  Get the item flags that defines the possible operations for the item.
	 *
	 *      @param [in] index   Model index that identifies the item.
	 *
	 *      @return Qt::ItemFlags specify the possible operations for the item.
	 */
	Qt::ItemFlags flags(const QModelIndex& index) const;

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
	virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

	/*!
     *  Save the data to the model for specified item.
	 *
	 *      @param [in] index   The model index of the item that's data is to be saved.
	 *      @param [in] value   The data that is to be saved.
	 *      @param [in] role    The role specifies what kind of data should be saved.
	 *
	 *      @return True if saving happened successfully.
	 */
	bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);
    
    /*!
     *  Get the list of acceptable mime types.
     *
     *      @return The list of acceptable mime types.
     */
    virtual QStringList mimeTypes() const;

public slots:

	/*!
     *  Add a new item to the given index.
	 *
	 *      @param [in] index   The index identifying the position for new item.
	 */
	virtual void onAddItem(const QModelIndex& index);

	/*!
     *  Remove the item in the given index.
	 *
	 *      @param [in] index   The index identifying the item to remove.
	 */
	virtual void onRemoveItem(const QModelIndex& index);

    /*!
     *  Copy the items in the selected rows.
     *
     *      @param [in] indexList   List of indexes pointing to the selected rows.
     */
    void onCopyRows(QModelIndexList indexList);

    /*!
     *  Paste the copied items.
     */
    void onPasteRows();

signals:

	//! Emitted when the contents of the model change.
	void contentChanged();

	//! Emitted when a new enumeration value is added to the given index.
	void enumAdded(int index);

	//! Emitted when an enumeration value is removed from the given index.
	void enumRemoved(int index);

private:
	
	//! No copying.
	EnumeratedValueModel(const EnumeratedValueModel& other);

	//! No assignment.
	EnumeratedValueModel& operator=(const EnumeratedValueModel& other);

    /*!
     *  Gets the value for the given index.
     *
     *      @param [in] index   The index of the target data.
     *
     *      @return The data in the given index.
     */
    QVariant valueForIndex(QModelIndex const& index) const;

    /*!
     *  Get a unique name for the selected name.
     *
     *      @param [in] originalName    The selected name.
     *
     *      @return A unique name created from the selected name.
     */
    QString getUniqueName(QString const& originalName);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Contains the enumerated values being edited.
    QSharedPointer<QList<QSharedPointer<EnumeratedValue> > > enumValues_;

    //! The validator for enumerated values.
    QSharedPointer<EnumeratedValueValidator> enumeratedValueValidator_;
};

#endif // ENUMERATEDVALUEMODEL_H
