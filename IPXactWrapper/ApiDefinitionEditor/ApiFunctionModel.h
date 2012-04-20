//-----------------------------------------------------------------------------
// File: ApiFunctionModel.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 18.4.2012
//
// Description:
// Model for API functions.
//-----------------------------------------------------------------------------

#ifndef APIFUNCTIONMODEL_H
#define APIFUNCTIONMODEL_H

#include <QObject>
#include <QAbstractListModel>
#include <QVariant>
#include <QStringList>
#include <QModelIndex>
#include <QSharedPointer>

class ApiFunction;
class ApiDefinition;

//-----------------------------------------------------------------------------
//! Model for API functions.
//-----------------------------------------------------------------------------
class ApiFunctionModel : public QAbstractListModel
{
	Q_OBJECT

public:
    /*!
     *  Constructor.
     *
     *      @param [in] parent The parent object.
     */
	ApiFunctionModel(QObject *parent = 0);

	/*!
     *  Destructor.
     */
	virtual ~ApiFunctionModel();

    /*!
     *  Restores the model from an API definition.
     *
     *      @param [in] apiDef The source API definition.
     */
    void restore(ApiDefinition const& apiDef);

    /*!
     *  Saves the changes to an API definition.
     *
     *      @param [in,out] apiDef The destination API definition.
     */
    void save(ApiDefinition& apiDef);

    /*!
     *  Returns the API function at the given index.
     *
     *      @param [in] index The index.
     */
    QSharedPointer<ApiFunction> getFunction(QModelIndex const& index);

	/*!
     *  Returns the number of rows in this model.
	 *
	 *      @param [in] parent ModelIndex of the item that's rowCount is requested.
	 */
	virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;

	/*!
     *  Returns the data stored for the specified item.
	 *
	 *      @param [in] index  ModelIndex of the wanted item.
	 *      @param [in] role   Specifies what kind of data is requested.
	 */
	virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

	/*!
     *  Returns the data for the header of the list.
	 *
	 *      @param [in] section      Specifies the column for which the header is requested.
	 *      @param [in] orientation  The orientation of the header, only Qt::Horizontal is supported.
	 *      @param [in] role         Specifies what kind of header data is requested.
	 */
	virtual QVariant headerData(int section, Qt::Orientation orientation, 
		                        int role = Qt::DisplayRole) const;

	/*!
     *  Saves the data to the model for specified item
	 *
	 *      @param [in] index  The model index of the item that's data is to be saved.
	 *      @param [in] value  The data that is to be saved.
	 *      @param [in] role   The role specifies what kind of data should be saved.
	 *
	 *      @return True if saving happened successfully.
	 */
	virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);

	/*!
     *  Returns the item flags that defines the possible operations for the item.
	 *
	 *      @param [in] index Model index that identifies the item.
	 *
	 *      @return Qt::ItemFlags specify the possible operations for the item.
	 */
	virtual Qt::ItemFlags flags(const QModelIndex& index) const;

	/*!
     *  Replace an item text in the list.
	 * 
	 *      @param [in] index    ModelIndex of the item that is to be replaced.
	 *      @param [in] newText  The new text for the given item.
	 */
	virtual void replace(QModelIndex& index, const QString newText);

public slots:
	 
	/*!
     *  Removes the specified item from the model.
	 *
     *      @param [in] index The model index of the item to remove.
     */
	virtual void remove(const QModelIndex& index);

	/*!
     *  A new item should be added to given index.
	 *
	 *      @param [in] index The position where new item should be added at.
	 */
	virtual void addItem(const QModelIndex& index);

	/*!
     *  Move item to another position.
	 *
	 *      @param [in] originalPos  Identifies the item that should be moved.
	 *      @param [in] newPos       The new position the item should be moved to.
	 */
	virtual void moveItem(const QModelIndex& originalPos, const QModelIndex& newPos);

signals: 
	//! Emitted when contents of the model change.
	void contentChanged();

private:
    // Disable copying.
    ApiFunctionModel(ApiFunctionModel const& rhs);
    ApiFunctionModel& operator=(ApiFunctionModel const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The list of edited API functions.
    QList< QSharedPointer<ApiFunction> > apiFunctions_;
};

//-----------------------------------------------------------------------------

#endif // APIFUNCTIONMODEL_H
