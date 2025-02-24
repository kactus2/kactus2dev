//-----------------------------------------------------------------------------
// File: ListManagerModel.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 28.01.2012
//
// Description:
// Model class for managing a list of strings.
//----------------------------------------------------------------------------- 

#ifndef LISTMANAGERMODEL_H
#define LISTMANAGERMODEL_H

#include <QObject>
#include <QAbstractListModel>
#include <QVariant>
#include <QStringList>
#include <QModelIndex>

//-----------------------------------------------------------------------------
//! Model class for managing a list of strings.
//-----------------------------------------------------------------------------
class ListManagerModel : public QAbstractListModel
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *     
     *    @param [in] items   QStringList that contains the items to add.
     *    @param [in] parent  The owner of this model.
     */
    ListManagerModel(QStringList const& items = QStringList(), QObject *parent = 0);

    /*!
     *  The destructor.
     */
    virtual ~ListManagerModel();

    /*!
     *  Get the number of rows in this model.
     *
     *    @param [in] parent  ModelIndex of the item that's rowCount is requested.
     */
    virtual int rowCount(QModelIndex const& parent = QModelIndex()) const;

    /*!
     *  Get the data stored for the specified item.
     *
     *    @param [in] index   ModelIndex of the wanted item.
     *    @param [in] role    Specifies what kind of data is requested.
     */
    virtual QVariant data(QModelIndex const& index, int role = Qt::DisplayRole) const;

    /*!
     *  Get the data for the header of the list.
     *
     *    @param [in] section         Specifies the column for which the header is requested.
     *    @param [in] orientation     The orientation of the header, only Qt::Horizontal is supported.
     *    @param [in] role            Specifies what kind of header data is requested.
     */
    virtual QVariant headerData(int section, Qt::Orientation orientation,  int role = Qt::DisplayRole) const;

    /*!
     *  Save the data to the model for specified item.
     *
     *    @param [in] index   The model index of the item that's data is to be saved.
     *    @param [in] value   The data that is to be saved.
     *    @param [in] role    The role specifies what kind of data should be saved.
     *
     *    @return True if saving happened successfully.
     */
    virtual bool setData(QModelIndex const& index, QVariant const& value, int role = Qt::EditRole);

    /*!
     *  Get the item flags that defines the possible operations for the item.
     *
     *    @param [in] index   Model index that identifies the item.
     *
     *    @return Qt::ItemFlags specify the possible operations for the item.
     */
    virtual Qt::ItemFlags flags(QModelIndex const& index) const;

    /*!
     *  Append a new item to the end of the list.
     *
     *    @param [in] item    The string to be appended to the end of the list.
     */
    virtual void appendItem(QString const& item);

    /*!
     *  Get the items currently stored in the model.
     *
     *    @return QStringList containing the items.
     */
    virtual QStringList const& items() const;

    /*!
     *  Set the items to the model.
     *
     *    @param [in] items   QStringList containing the items to be contained in the model.
     */
    virtual void setItems(const QStringList& items);

    /*!
     *  Replace an item text in the list.
     * 
     *    @param [in] index       ModelIndex of the item that is to be replaced.
     *    @param [in] newText     The new text for the given item.
     */
    virtual void replace(QModelIndex& index, const QString newText);

public slots:
     
    /*!
     *  Removes the specified item from the model.
     *
     *    @param [in] index   The model index of the item to remove.
     */
    virtual void remove(QModelIndex const& index);

    /*!
     *  A new item should be added to given index.
     *
     *    @param [in] index   The position where new item should be added at.
     */
    virtual void addItem(QModelIndex const& index);

    /*!
     *  Move item to another position.
     *
     *    @param [in] originalPos     Identifies the item that should be moved.
     *    @param [in] newPos          The new position the item should be moved to.
     */
    virtual void moveItem(QModelIndex const& originalPos, QModelIndex const& newPos);

signals: 

    /*!
     *  Emitted when contents of the model change.
     */
    void contentChanged();

protected:

    /*!
     *  Contains the items to be displayed in a list.
     */
    QStringList items_;

private:

    //! No copying. No assignment.
    ListManagerModel(const ListManagerModel& other);
    ListManagerModel& operator=(const ListManagerModel& other);
};

#endif // LISTMANAGERMODEL_H
