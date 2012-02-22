//-----------------------------------------------------------------------------
// File: AdHocModel.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 20.2.2012
//
// Description:
// Table model for visualizing ad-hoc visibility for component ports.
//-----------------------------------------------------------------------------

#ifndef ADHOCMODEL_H
#define ADHOCMODEL_H

#include <QAbstractTableModel>
#include <QSharedPointer>

class DiagramComponent;
class Port;
class GenericEditProvider;

//-----------------------------------------------------------------------------
//! Table model for visualizing ad-hoc visibility for component ports.
//-----------------------------------------------------------------------------
class AdHocModel : public QAbstractTableModel
{
	Q_OBJECT

public:

	/*!
     *  Constructor.
	 *
     *      @param [in] editProvider  The edit provider managing the undo/redo stack.
	 *      @param [in] parent        Pointer to the owner of this model.
	 *
	 */
	AdHocModel(QObject *parent);
	
	/*!
     *  Destructor.
     */
	virtual ~AdHocModel();

    /*!
     *  Sets the component whose ad-hoc port visibility is being edited.
     *
     *      @param [in] component The component.
     */
    void setComponent(DiagramComponent* component);

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
	 *      @return Always returns 3.
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
	 *      @return QVariant containing the data to be displayed.
	 */
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

	/*!
     *  Set the data for specified item.
	 *
	 *      @param [in] index  Specifies the item that's data is modified
	 *      @param [in] value  The value to be set.
	 *      @param [in] role   The role that is trying to modify the data. Only Qt::EditRole
	 *                         is supported.
	 *
	 *      @return True if data was successfully set.
	 */
	virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);

	/*!
     *  Returns information on how specified item can be handled.
	 *
	 *      @param [in] index Specifies the item that's flags are wanted.
	 *
	 *      @return Qt::ItemFlags that define how object can be handled.
	 */
	virtual Qt::ItemFlags flags(const QModelIndex& index) const;

public slots:
    /*!
     *  Updates the visibilities.
     */
    void updateVisibilities();

signals:
	//! \brief Emitted when contents of the model change
	void contentChanged();

private:
    // Disable copying.
    AdHocModel(AdHocModel const& rhs);
    AdHocModel& operator=(AdHocModel const& rhs);

    //! The component whose ad-hoc port visibility is being edited.
    DiagramComponent* component_;

	//! The table that is displayed to the user.
	QList< QSharedPointer<Port> > table_;

    //! Pointer to the generic edit provider that manages the undo/redo stack.
    QSharedPointer<GenericEditProvider> editProvider_;
};

//-----------------------------------------------------------------------------

#endif // ADHOCMODEL_H