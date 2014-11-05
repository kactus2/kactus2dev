//-----------------------------------------------------------------------------
// File: EnumerationModel.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 04.11.2014
//
// Description:
// Model for editing Enumeration within a Choice element.
//-----------------------------------------------------------------------------

#ifndef ENUMERATIONMODEL_H
#define ENUMERATIONMODEL_H

#include <QAbstractTableModel>

#include <QSharedPointer>
#include <QList>

class Enumeration;

//-----------------------------------------------------------------------------
// class EnumerationModel.
//-----------------------------------------------------------------------------
class EnumerationModel : public QAbstractTableModel 
{
	Q_OBJECT

public:
	/*!
	 *  The constructor.
	 *
     *      @param [in] enumerations   The enumerations represented by the model.
	 *      @param [in] parent         The parent object.
	 */
	EnumerationModel(QSharedPointer<QList<QSharedPointer<Enumeration> > > enumerations, QObject *parent = 0);

    //! The destructor.
    ~EnumerationModel();

    /*!
     *  Returns the amount of columns in the model.
     *
     *      @param [in] parent  If given, the row count of the children of the given parent is returned.
     *
     *      @return The amount of columns.
     */
    int columnCount(QModelIndex const& parent = QModelIndex()) const;

    /*!
     *  Returns the amount of columns in the model.
     *
     *      @param [in] parent  If given, the column count of the children of the given parent is returned.
     *
     *      @return The amount of columns.
     */
    int rowCount(QModelIndex const& parent = QModelIndex()) const;

    /*!
     *  Fetches the data for appropriate role at given index.
     *
     *      @param [in] index   The index of data to be returned.
     *      @param [in] role    The role for which data is returned.
     *
     *      @return The requested data.
     */
    QVariant data(QModelIndex const& index, int role = Qt::DisplayRole) const;

    /*!
     *  Retrieves the header data for given column (used) or row (not used).
     *
     *      @param [in] section     The row or column from which the index is retrieved.
     *      @param [in] orientation Whether the header is vertical or horizontal.
     *      @param [in] role        The role for which the header is retrieved.
     *
     *      @return The header data.
     */
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    /*!
     *  Sets the data at given index.
     *    
     *      @param [in] index   The index at which the data is set.
     *      @param [in] value   The new value being set.
     *      @param [in] role    The role for which the data is set.
     *
     *      @return True, if data was set successfully, otherwise false.
     */
    bool setData(QModelIndex const& index, QVariant const& value, int role = Qt::EditRole);

    /*!
     *  Returns the flags for an object at given index.
     *
     *      @param [in] index   The index of the object queried.
     *
     *      @return The relevant flags.
     */
    Qt::ItemFlags flags(QModelIndex const& index) const;

signals:

    //! Emitted when the model content has changed.
    void contentChanged();

public slots:
    
    /*!
     *  Called when a new enumeration should be added to the model.
     *
     *      @param [in] index   The index to add the enumeration to.
     */
    void onAddItem(QModelIndex const& index);
       
    /*!
     *  Called when an enumeration should be removed from the model.
     *
     *      @param [in] index   The index to remove the enumeration from.
     */
    void onRemoveItem(QModelIndex const& index);

private:
	// Disable copying.
	EnumerationModel(EnumerationModel const& rhs);

	// Disable assignment.
	EnumerationModel& operator=(EnumerationModel const& rhs);

    /*!
     *  Checks if the given index is not valid and outside the model.
     *
     *      @param [in] index   The model index to check.
     *
     *      @return True, if the index is invalid, otherwise false.
     */
    bool isNotValidIndex(QModelIndex const& index) const;

    /*!
     *  Checks if the given row represents an enumeration.
     *
     *      @param [in] row   The row number to check.
     *
     *      @return True, if the row represents an enumeration, otherwise false.
     */
    bool hasEnumerationOnRow(int row) const;

    /*!
     *  Checks if the given row is the last row in the model.
     *
     *      @param [in] row   The row number to check.
     *
     *      @return True, if the given row is the last row in the model, otherwise false.
     */
    bool isLastRow(int row);

    /*!
     *  Gets the data in the enumeration column.
     *
     *      @param [in] row     The row of data to be returned.
     *      @param [in] role    The role for which data is returned.
     *
     *      @return The data in enumeration column for the given row and role.
     */
    QVariant getEnumerationColumnData(int row, int role) const;

    /*!
     *  Gets the foreground color for the given enumeration.
     *
     *      @param [in] enumeration   The enumeration for which the foreground color is fetched.
     *
     *      @return The foreground color for the enumeration cells.
     */
    QVariant getForegroundColorForEnumeration(QSharedPointer<Enumeration> enumeration) const;

    /*!
     *  Gets the data in the text column.
     *
     *      @param [in] row     The row of data to be returned.
     *      @param [in] role    The role for which data is returned.
     *
     *      @return The data in text column for the given row and role.
     */
    QVariant getTextColumnData(int row, int role) const;
    
    /*!
     *  Gets the data in the help column.
     *
     *      @param [in] row     The row of data to be returned.
     *      @param [in] role    The role for which data is returned.
     *
     *      @return The data in help column for the given row and role.
     */
    QVariant getHelpColumnData(int row, int role) const;

    //-----------------------------------------------------------------------------
	// Data.
	//-----------------------------------------------------------------------------
    
    //! The list of enumerations in the choice.
    QSharedPointer<QList<QSharedPointer<Enumeration> > > enumerations_;
};
#endif // ENUMERATIONMODEL_H
