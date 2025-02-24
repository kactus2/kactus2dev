//-----------------------------------------------------------------------------
// File: ModeReferenceModel.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 10.8.2023
//
// Description:
// Model to manage mode references.
//-----------------------------------------------------------------------------

#ifndef MODEREFERENCEMODEL_H
#define MODEREFERENCEMODEL_H

#include <QAbstractTableModel>

class ModeReferenceInterface;

class ModeReferenceModel : public QAbstractTableModel
{
    Q_OBJECT

public:

    /*!
     *	The constructor.
     * 
     *    @param [in] modeRefInterface    The mode reference interface to use.
     *    @param [in] modeRefs            The mode references to edit.
     */
    ModeReferenceModel(ModeReferenceInterface* modeRefInterface, QObject* parent);

    /*!
     *  Get the number of modes the model contains.
     *
     *    @param [in] parent  Identifies the parent whose row count is requested.
     *
     *    @return Number of rows the model has.
     */
    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;

    /*!
     *  Get the number of columns the model has to be displayed.
     *
     *    @param [in] parent  Identifies the parent whose column count is requested.
     *
     *    @return The number of columns to be displayed.
     */
    virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;

    /*!
     *  Get the item flags that defines the possible operations for the item.
     *
     *    @param [in] index   Model index that identifies the item.
     *
     *    @return Qt::ItemFlags specifying the possible operations for the item.
     */
    Qt::ItemFlags flags(const QModelIndex& index) const;

    /*!
     *  Get the header data for specified header.
     *
     *    @param [in] section         The section specifies the row/column number for the header.
     *    @param [in] orientation     Specified if horizontal or vertical header is wanted.
     *    @param [in] role            Specifies the type of the requested data.
     *
     *    @return QVariant containing the requested data.
     */
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    /*!
     *  Get the data for specified item.
     *
     *    @param [in] index   Specifies the item that's data is requested.
     *    @param [in] role    The role that defines what kind of data is requested.
     *
     *    @return QVariant containing the data for the item.
     */
    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

    /*!
     *  Save the data to the model for specified item.
     *
     *    @param [in] index   The model index of the item that's data is to be saved.
     *    @param [in] value   The data that is to be saved.
     *    @param [in] role    The role specifies what kind of data should be saved.
     *
     *    @return True if saving happened successfully.
     */
    virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);

public slots:
    
    /*!
     *	Handler for adding a new mode reference row.
     *  
     *    @param [in] index     The model index that was selected.
     */
    virtual void onAddRow(QModelIndex const& index);

    /*!
     *	Handler for removing a mode reference row.
     *  
     *    @param [in] index     The mode reference to be removed.
     */
    virtual void onRemoveItem(QModelIndex const& index);

signals:

    /*!
     *	Emitted when there has been a change in the number of rows.
     */
    void invalidateFilter();

    /*!
     *	Emitted whenever a mode reference has been edited.
     */
    void contentChanged();

private:

    //! The interface to access mode references.
    ModeReferenceInterface* interface_;

};

#endif // MODEREFERENCEMODEL_H



