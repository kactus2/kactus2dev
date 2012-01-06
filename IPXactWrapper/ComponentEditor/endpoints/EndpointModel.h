//-----------------------------------------------------------------------------
// File: EndpointModel.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 19.9.2011
//
// Description:
// Endpoint data model for the endpoint editor.
//-----------------------------------------------------------------------------

#ifndef ENDPOINTMODEL_H
#define ENDPOINTMODEL_H

#include "ImportEndpointsDialog.h"

#include <models/port.h>
#include <models/component.h>

#include <QAbstractTableModel>
#include <QMap>
#include <QSharedPointer>
#include <QString>

//-----------------------------------------------------------------------------
//! EndpointModel class.
//-----------------------------------------------------------------------------
class EndpointModel : public QAbstractTableModel
{
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
     * \param component Pointer to the component being edited.
	 * \param dataPointer Pointer to the QMap containing the endpoints (model parameters).
	 * \param parent Pointer to the owner of this model.
	 *
	*/
	EndpointModel(QSharedPointer<Component> component, void* dataPointer, QObject *parent);

	//! \brief The destructor
	virtual ~EndpointModel();

    /*!
     *  Imports the endpoints from the given source component.
     *
     *      @param [in] sourceComp The source component.
     *      @param [in] option     The import option.
     */
    bool importEndpoints(Component const& sourceComp, ImportEndpointsDialog::ImportOption option);

	/*! \brief Get the number of rows in the model.
	 *
	 * \param parent Model index of the parent of the item. Must be invalid
	 * because this is not hierarchical model.
	 *
	 * \return Number of rows currently in the model.
	*/
	virtual int rowCount(const QModelIndex& parent = QModelIndex() ) const;

	/*! \brief Get the number of columns in the model
	 *
	 * \param parent Model index of the parent of the item. Must be invalid
	 * because this is not hierarchical model.
	 *
	 * \return Always returns 4
	*/
	virtual int columnCount(const QModelIndex& parent = QModelIndex() ) const;

	/*! \brief Get the data for the specified item for specified role.
	 *
	 * \param index Identifies the item that's data is wanted.
	 * \param role Specifies what kind of data is wanted
	 *
	 * \return QVariant containing the data
	*/
	virtual QVariant data(const QModelIndex& index, 
		int role = Qt::DisplayRole ) const;

	/*! \brief Get the data for the headers
	 *
	 * \param section The column that's header is wanted
	 * \param orientation Only Qt::Horizontal is supported
	 * \param role Specified the type of data that is wanted.
	 *
	 * \return QT_NAMESPACE::QVariant
	*/
	virtual QVariant headerData(int section, Qt::Orientation orientation, 
		int role = Qt::DisplayRole ) const;

	/*! \brief Get information on how specified item can be handled.
	 *
	 * \param index Specifies the item that's flags are wanted.
	 *
	 * \return Qt::ItemFlags that define how object can be handled.
	*/
	virtual Qt::ItemFlags flags(const QModelIndex& index) const;

	/*! \brief Set the data for specified item.
	 *
	 * \param index Specifies the item that's data is modified
	 * \param value The value to be set.
	 * \param role The role that is trying to modify the data. Only Qt::EditRole
	 * is supported.
	 *
	 * \return True if data was successfully set.
	*/
	virtual bool setData(const QModelIndex& index, const QVariant& value, 
		int role = Qt::EditRole );

	/*! \brief Check if the model is in valid state or not.
	 *
	 * \return True if all items in model are valid.
	*/
	bool isValid() const;

	/*! \brief Write the changes to the original model.
	 *
	*/
	void apply();

	/*! \brief Restore the settings from the original model.
	 *
	*/
	void restore();

public slots:

	/*! \brief Remove a row from the model
	 *
	 * \param row Specifies the row to remove
	*/
	void onRemoveRow(int row);

	/*! \brief Add a new empty row to the model
	 *
	*/
	void onAddRow();

signals:

	//! \brief Emitted when contents of the model change
	void contentChanged();

	//! \brief Prints an error message to the user.
	void errorMessage(const QString& msg) const;
	
	//! \brief Prints a notification to user.
	void noticeMessage(const QString& msg) const;

private:

	//! \brief No copying
	EndpointModel(const EndpointModel& other);

	//! No assignment
	EndpointModel& operator=(const EndpointModel& other);
	
	//! \brief Pointer to the data structure that contains the actual endpoints (ports).
	QMap<QString, QSharedPointer<Port> >* endpoints_;

	//! \brief The table that is displayed to the user.
	QList<QSharedPointer<Port> > table_;

    //! \brief Pointer to the component being edited.
    QSharedPointer<Component> component_;
};

//-----------------------------------------------------------------------------

#endif // ENDPOINTMODEL_H
