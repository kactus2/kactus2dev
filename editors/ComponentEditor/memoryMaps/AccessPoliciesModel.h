//-----------------------------------------------------------------------------
// File: AccessPoliciesModel.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 28.8.2023
//
// Description:
// Model describing the access policies of an address block, register or register file.
//-----------------------------------------------------------------------------

#ifndef ACCESSPOLICIESMODEL_H
#define ACCESSPOLICIESMODEL_H

#include <QAbstractTableModel>

class RegisterInterface;

class AccessPoliciesModel : public QAbstractTableModel
{
    Q_OBJECT

public:

    AccessPoliciesModel(QString const& registerName, RegisterInterface* interface, QObject* parent);

    virtual ~AccessPoliciesModel() = default;

    //! No copying.
    AccessPoliciesModel(AccessPoliciesModel const& other) = delete;
    AccessPoliciesModel& operator=(AccessPoliciesModel const& other) = delete;

    /*!
     *  Get the number of access policies the model contains.
     *
     *      @param [in] parent  Identifies the parent whose row count is requested.
     *
     *      @return Number of rows the model has.
     */
    int rowCount(const QModelIndex& parent = QModelIndex()) const;

    /*!
     *  Get the number of columns the model has to be displayed.
     *
     *      @param [in] parent  Identifies the parent whose column count is requested.
     *
     *      @return The number of columns to be displayed.
     */
    int columnCount(const QModelIndex& parent = QModelIndex()) const;

    /*!
     *  Get the item flags that defines the possible operations for the item.
     *
     *      @param [in] index   Model index that identifies the item.
     *
     *      @return Qt::ItemFlags specifying the possible operations for the item.
     */
    Qt::ItemFlags flags(const QModelIndex& index) const;

    /*!
     *  Get the header data for specified header.
     *
     *      @param [in] section         The section specifies the row/column number for the header.
     *      @param [in] orientation     Specified if horizontal or vertical header is wanted.
     *      @param [in] role            Specifies the type of the requested data.
     *
     *      @return QVariant containing the requested data.
     */
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    /*!
     *  Get the data for specified item.
     *
     *      @param [in] index   Specifies the item that's data is requested.
     *      @param [in] role    The role that defines what kind of data is requested.
     *
     *      @return QVariant containing the data for the item.
     */
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

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

signals:

    /*!
     *	Emitted whenever an access policy has been edited.
     */
    void contentChanged();

    /*!
     *	Emitted when there has been a change in the number of rows.
     */
    void invalidateFilter();

public slots:

    /*!
     *	Handler for adding a new field access policy.
     *
     *      @param [in] index     The model index of the item that was selected.
     */
    void onAddRow(QModelIndex const& index);

    /*!
     *	Handler for removing a field access policy.
     *
     *      @param [in] index     The model index for the item/row to be removed.
     */
    void onRemoveItem(QModelIndex const& index);

private:

    /*!
     *  Gets the value for the given index.
     *
     *      @param [in] index   The index of target data.
     *
     *      @return     The data in the given index.
     */
    QVariant valueForIndex(QModelIndex const& index) const;

    /*!
     *  Validates the data in the index.
     *
     *      @param [in] index   The index being validated.
     *
     *      @return     True, if the data is valid, otherwise false.
     */
    bool validateIndex(QModelIndex const& index) const;

    //! The interface 
    RegisterInterface* interface_;
    
    //! The name of the register being edited.
    std::string registerName_;

};

#endif // ACCESSPOLICIESMODEL_H
