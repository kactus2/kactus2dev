//-----------------------------------------------------------------------------
// File: ComPropertyModel.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 17.4.2012
//
// Description:
// Model for COM properties.
//-----------------------------------------------------------------------------

#ifndef COMPROPERTYMODEL_H
#define COMPROPERTYMODEL_H

#include <QAbstractTableModel>
#include <QMap>
#include <QList>
#include <QString>
#include <QSharedPointer>

class ComProperty;

//-----------------------------------------------------------------------------
//! ComPropertyModel class.
//-----------------------------------------------------------------------------
class ComPropertyModel : public QAbstractTableModel
{
	Q_OBJECT

public:
    /*!
     *  Constructor.
     *
     *      @param [in] parent The parent object.
	 */
	ComPropertyModel(QObject *parent);

	/*!
     *  Destructor.
     */
	virtual ~ComPropertyModel();

    /*!
     *  Sets the properties for editing.
     *
     *      @param [in] properties The list of properties.
     */
    void setProperties(QList< QSharedPointer<ComProperty> > const& properties);

    /*!
     *  Returns the properties.
     */
    QList< QSharedPointer<ComProperty> > const& getProperties() const;

    /*!
     *  Returns the number of rows in the model.
	 *
	 *      @param [in] parent Model index of the parent of the item. Must be invalid
	 *                         because this is not hierarchical model.
	 *
	 *      @return Number of rows currently in the model.
	 */
	virtual int rowCount(QModelIndex const& parent = QModelIndex()) const;

	/*! 
     *  Returns the number of columns in the model.
	 *
	 *      @param [in] parent Model index of the parent of the item. Must be invalid
	 *                         because this is not hierarchical model.
	 *
	 *      @return Always returns PROPERTY_COL_COUNT.
	 */
	virtual int columnCount(QModelIndex const& parent = QModelIndex()) const;

	/*!
     *  Returns the data for the specified item for specified role.
	 *
	 *      @param [in] index  Identifies the item that's data is wanted.
	 *      @param [in] role   Specifies what kind of data is wanted
	 *
	 *      @return QVariant containing the data.
	 */
	virtual QVariant data(QModelIndex const& index, int role = Qt::DisplayRole) const;

	/*!
     *  Returns the data for the headers.
	 *
	 *      @param [in] section      The column that's header is wanted
	 *      @param [in] orientation  Only Qt::Horizontal is supported
	 *      @param [in] role         Specified the type of data that is wanted.
	 *
	 *      @return QVariant containing the header data.
	 */
	virtual QVariant headerData(int section, Qt::Orientation orientation, 
		                        int role = Qt::DisplayRole) const;

	/*!
     *  Returns information on how specified item can be handled.
	 *
	 *      @param [in] index Specifies the item that's flags are wanted.
	 *
	 *      @return Item flags that define how object can be handled.
	 */
	virtual Qt::ItemFlags flags(QModelIndex const& index) const;

	/*!
     *  Returns the data for specified item.
	 *
	 *      @param [in] index  Specifies the item that's data is modified
	 *      @param [in] value  The value to be set.
	 *      @param [in] role   The role that is trying to modify the data. Only Qt::EditRole is supported.
	 *
	 *      @return True if data was successfully set.
	 */
	virtual bool setData(QModelIndex const& index, QVariant const& value, 
		                 int role = Qt::EditRole);

	/*!
     *  Checks whether the model is in valid state or not.
     *
	 *      @return True if all items in model are valid.
	 */
	bool isValid() const;

public slots:
    void onAdd();
    void onAddItem(QModelIndex const& index);
    void onRemove(QModelIndex const& index );
    void onRemoveItem(QModelIndex const& index );

signals:
	//! Emitted when contents of the model change.
	void contentChanged();

	//! Prints an error message to the user.
	void errorMessage(const QString& msg) const;
	
	//! Prints a notification to user.
	void noticeMessage(const QString& msg) const;

private:
    // Disable copying.
    ComPropertyModel(ComPropertyModel const& rhs);
    ComPropertyModel& operator=(ComPropertyModel const& rhs);
    
    //! IP address regular expression string.
	static QString const IP_ADDRESS_REGEX;

	//! Time unit regular expression string.
	static QString const TIME_UNIT_REGEX;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------
	
	//! The table that is displayed to the user.
	QList< QSharedPointer<ComProperty> > table_;
};

//-----------------------------------------------------------------------------

#endif // COMPROPERTYMODEL_H
