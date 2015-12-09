//-----------------------------------------------------------------------------
// File: channelsmodel.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 14.06.2012
//
// Description:
// The model class to manage the objects for channels editor.
//-----------------------------------------------------------------------------

#ifndef CHANNELSMODEL_H
#define CHANNELSMODEL_H

#include <QAbstractTableModel>
#include <QList>
#include <QSharedPointer>

class Channel;
class ChannelValidator;
//-----------------------------------------------------------------------------
//! The model class to manage the objects for channels editor.
//-----------------------------------------------------------------------------
class ChannelsModel : public QAbstractTableModel
{
	Q_OBJECT

public:

	/*! The constructor
	 *
	 *      @param [in] channels    The channels being edited.
     *      @param [in] validator   The validator for channel elements.
	 *      @param [in] parent      The owner of this model.
	*/
	ChannelsModel(QSharedPointer<QList<QSharedPointer<Channel> > > channels, 
        QSharedPointer<ChannelValidator> validator, QObject *parent);
	
	//! The destructor.
	virtual ~ChannelsModel();

	/*! Get the number of rows an item contains.
	 *
	 *      @param [in] parent Identifies the parent that's row count is requested.
	 *
	 *      @return Number of rows the item has.
	*/
	virtual int rowCount(QModelIndex const& parent = QModelIndex()) const;

	/*! Get the number of columns the item has to be displayed.
	 *
	 *      @param [in] parent Identifies the parent that's column count is requested.
	 *
	 *      @return The number of columns to be displayed.
	*/
	virtual int columnCount(QModelIndex const& parent = QModelIndex()) const;

	/*! Get the item flags that defines the possible operations for the item.
	 *
	 *      @param [in] index Model index that identifies the item.
	 *
	 *      @return Qt::ItemFlags specify the possible operations for the item.
	*/
	Qt::ItemFlags flags(QModelIndex const& index) const;

	/*! Get the header data for specified header.
	 *
	 *      @param [in] section         The section specifies the row/column number for the header.
	 *      @param [in] orientation     Specified if horizontal or vertical header is wanted.
	 *      @param [in] role            Specifies the type of the requested data.
	 *
	 *      @return QVariant Contains the requested data.
	*/
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

	/*! Get the data for specified item.
	 *
	 *      @param [in] index   Specifies the item that's data is requested.
	 *      @param [in] role    The role that defines what kind of data is requested.
	 *
	 *      @return QVariant Contains the data for the item.
	*/
	virtual QVariant data(QModelIndex const& index, int role = Qt::DisplayRole) const;

	/*! Save the data to the model for specified item
	 *
	 *      @param [in] index   The model index of the item that's data is to be saved.
	 *      @param [in] value   The data that is to be saved.
	 *      @param [in] role    The role specifies what kind of data should be saved.
	 *
	 *      @return True if saving happened successfully.
	*/
	bool setData(QModelIndex const& index, const QVariant& value, int role = Qt::EditRole);


public slots:

	/*! Add a new item to the given index.
	 *
	 *      @param [in] index The index identifying the position for new item.
	 *
	*/
	virtual void onAddItem(QModelIndex const& index);

	/*! Remove the item in the given index.
	 *
	 *      @param [in] index The index identifying the item to remove.
	 *
	*/
	virtual void onRemoveItem(QModelIndex const& index);

signals:

	//! Emitted when the contents of the model change.
	void contentChanged();

	/*! Emitted when a new channel is added to the model.
	 *
	 *      @param [in] index The index of the added channel.
	 *
	*/
	void channelAdded(int index);

	/*! Emitted when a channel is removed from the model.
	 *
	 *      @param [in] index The index of the channel to remove.
	 *
	*/
	void channelRemoved(int index);

private:
	
	//! No copying
	ChannelsModel(const ChannelsModel& other);
	ChannelsModel& operator=(const ChannelsModel& other);

	//! The channels being edited.
	QSharedPointer<QList<QSharedPointer<Channel> > > channels_;

    //! The validator for the channels.
    QSharedPointer<ChannelValidator> validator_;
};

#endif // CHANNELSMODEL_H
