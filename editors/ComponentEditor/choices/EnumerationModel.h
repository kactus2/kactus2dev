//-----------------------------------------------------------------------------
// File: EnumerationModel.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 04.11.2014
//
// Description:
// Model for editing Enumeration within a Choice element.
//-----------------------------------------------------------------------------

#ifndef ENUMERATIONMODEL_H
#define ENUMERATIONMODEL_H

#include <common/models/TableModelBase.h>

#include <QSharedPointer>
#include <QList>

class Enumeration;

//-----------------------------------------------------------------------------
// class EnumerationModel.
//-----------------------------------------------------------------------------
class EnumerationModel : public TableModelBase 
{
	Q_OBJECT

public:
    
    /*!
     *  The constructor.
     *
     *    @param [in] parent         The parent object.
     */
    EnumerationModel(QObject* parent = 0);

    /*!
     *  The destructor.
     */
    ~EnumerationModel() = default;

    /*!
     *  Returns the amount of columns in the model.
     *
     *    @param [in] parent  If given, the row count of the children of the given parent is returned.
     *
     *    @return The amount of columns.
     */
    int columnCount(QModelIndex const& parent = QModelIndex()) const;

    /*!
     *  Returns the amount of columns in the model.
     *
     *    @param [in] parent  If given, the column count of the children of the given parent is returned.
     *
     *    @return The amount of columns.
     */
    int rowCount(QModelIndex const& parent = QModelIndex()) const;

    /*!
     *  Fetches the data for appropriate role at given index.
     *
     *    @param [in] index   The index of data to be returned.
     *    @param [in] role    The role for which data is returned.
     *
     *    @return The requested data.
     */
    QVariant data(QModelIndex const& index, int role = Qt::DisplayRole) const;

    /*!
     *  Retrieves the header data for given column (used) or row (not used).
     *
     *    @param [in] section     The row or column from which the index is retrieved.
     *    @param [in] orientation Whether the header is vertical or horizontal.
     *    @param [in] role        The role for which the header is retrieved.
     *
     *    @return The header data.
     */
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    /*!
     *  Sets the data at given index.
     *    
     *    @param [in] index   The index at which the data is set.
     *    @param [in] value   The new value being set.
     *    @param [in] role    The role for which the data is set.
     *
     *    @return True, if data was set successfully, otherwise false.
     */
    bool setData(QModelIndex const& index, QVariant const& value, int role = Qt::EditRole);

    /*!
     *  Returns the flags for an object at given index.
     *
     *    @param [in] index   The index of the object queried.
     *
     *    @return The relevant flags.
     */
    Qt::ItemFlags flags(QModelIndex const& index) const;

signals:

    /*!
     *  Emitted when the model content has changed.
     */
    void contentChanged();

public slots:

    /*!
     *  Setup the available enumerations.
     *
     *    @param [in] newEnumerations     The new enumerations.
     */
    void setupEnumerations(QSharedPointer<QList<QSharedPointer<Enumeration> > > newEnumerations);

    /*!
     *  Clear the available enumerations.
     */
    void clearEnumerations();

    /*!
     *  Called when a new enumeration should be added to the model.
     *
     *    @param [in] index   The index to add the enumeration to.
     */
    void onAddItem(QModelIndex const& index);
       
    /*!
     *  Called when an enumeration should be removed from the model.
     *
     *    @param [in] index   The index to remove the enumeration from.
     */
    void onRemoveItem(QModelIndex const& index);

private:
	// Disable copying.
	EnumerationModel(EnumerationModel const& rhs);

	// Disable assignment.
	EnumerationModel& operator=(EnumerationModel const& rhs);

    /*!
     *  Validates the data in an index.
     *
     *    @param [in] index   The index whose data to validate
     *
     *    @return True, if the data in the index is valid, otherwise false.
     */
    bool validateIndex(QModelIndex const& index) const override;

    /*!
     *  Checks if the given index is marked mandatory.
     *
     *    @param[in] index   Model index to check.
     *
     *    @return True, if the index is a mandatory item, otherwise false.
     */
    bool indexIsMandatory(QModelIndex const& index) const override;

    /*!
     *  Checks if the given index is not valid and outside the model.
     *
     *    @param [in] index   The model index to check.
     *
     *    @return True, if the index is invalid, otherwise false.
     */
    bool isNotValidIndex(QModelIndex const& index) const;

    //-----------------------------------------------------------------------------
	// Data.
	//-----------------------------------------------------------------------------
    
    //! The list of enumerations in the choice.
    QSharedPointer<QList<QSharedPointer<Enumeration> > > enumerations_;
};
#endif // ENUMERATIONMODEL_H
