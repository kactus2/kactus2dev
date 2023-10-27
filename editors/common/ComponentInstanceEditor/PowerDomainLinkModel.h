//-----------------------------------------------------------------------------
// File: PowerDomainLinkModel.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 19.10.2023
//
// Description:
// Model for editing power domain links in a component instance.
//-----------------------------------------------------------------------------

#ifndef PowerDomainLinkModel_H
#define PowerDomainLinkModel_H

#include <QObject>
#include <QAbstractTableModel>
#include <QVariant>
#include <QModelIndex>
#include <QSharedPointer>

#include <IPXACTmodels/Design/ComponentInstance.h>

class Choice;
class ChoiceValidator;

//-----------------------------------------------------------------------------
//! Model for editing power domain links in a component instance.
//-----------------------------------------------------------------------------
class PowerDomainLinkModel : public QAbstractTableModel
{
	Q_OBJECT

public:
    
    /*!
     *  Constructor.
     *
     *      @param [in] parent      The parent object.
     */
    explicit PowerDomainLinkModel(QObject* parent = nullptr);
    
    /*!
     *  Destructor.
     */
    ~PowerDomainLinkModel() final = default;


    // Disable copying.
    PowerDomainLinkModel(PowerDomainLinkModel const& rhs) = delete;
    PowerDomainLinkModel& operator=(PowerDomainLinkModel const& rhs) = delete;

    /*!
     *  Returns the number of rows in this model.
     *
     *      @param [in] parent ModelIndex of the item that's rowCount is requested.
     */
    int rowCount(QModelIndex const& parent = QModelIndex()) const final;

    /*!
     *  Returns the number of rows in this model.
     *
     *      @param [in] parent ModelIndex of the item that's rowCount is requested.
     */
    int columnCount(QModelIndex const& parent = QModelIndex()) const final;

    /*!
     *  Returns the data stored for the specified item.
     *
     *      @param [in] index  ModelIndex of the wanted item.
     *      @param [in] role   Specifies what kind of data is requested.
     */
    QVariant data(QModelIndex const& index, int role = Qt::DisplayRole) const final;
    
    /*!
     *  Returns the data for the header of the list.
     *
     *      @param [in] section      Specifies the column for which the header is requested.
     *      @param [in] orientation  The orientation of the header, only Qt::Horizontal is supported.
     *      @param [in] role         Specifies what kind of header data is requested.
     */
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const final;
    
    /*!
     *  Saves the data to the model for specified item
     *
     *      @param [in] index  The model index of the item that's data is to be saved.
     *      @param [in] value  The data that is to be saved.
     *      @param [in] role   The role specifies what kind of data should be saved.
     *
     *      @return True if saving happened successfully.
     */
    bool setData(QModelIndex const& index, const QVariant& value, int role = Qt::EditRole) final;
    
    /*!
     *  Returns the item flags that defines the possible operations for the item.
     *
     *      @param [in] index Model index that identifies the item.
     *
     *      @return Qt::ItemFlags specify the possible operations for the item.
     */
    Qt::ItemFlags flags(QModelIndex const& index) const final;

    void setPowerDomains(ComponentInstance::PowerDomainLink::List powerDomains) noexcept;

public slots:

    
    /*!
     *  A new item should be added to given index.
     *
     *      @param [in] index The position where new item should be added at.
     */
    void addItem(QModelIndex const& index);

    /*!
     *  Removes the specified item from the model.
     *
     *      @param [in] index The model index of the item to remove.
     */
    void removeItem(QModelIndex const& index);

signals:

    /*!
     *  Emitted when contents of the model change.
     */
	void contentChanged();
    

private:
    
    
    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    ComponentInstance::PowerDomainLink::List powerDomains_;
};

#endif // PowerDomainLinkModel_H
