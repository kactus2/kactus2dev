/* 
 *  	Created on: 21.8.2012
 *      Author: Antti Kamppi
 * 		filename: memorymapsmodel.h
 *		Project: Kactus 2
 */

#ifndef MEMORYMAPSMODEL_H
#define MEMORYMAPSMODEL_H

#include <IPXACTmodels/component.h>
#include <IPXACTmodels/memorymap.h>
#include <IPXACTmodels/MemoryRemap.h>

#include <editors/ComponentEditor/common/ParameterFinder.h>

#include <QAbstractTableModel>
#include <QAbstractItemModel>
#include <QSharedPointer>
#include <QColor>

//-----------------------------------------------------------------------------
//! The model to manage the memory maps summary.
//-----------------------------------------------------------------------------
class MemoryMapsModel : public QAbstractItemModel
{
	Q_OBJECT

public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] component           Pointer to the component that contains the memory maps to edit.
     *      @param [in] parameterFinder     Pointer to the instance used to find parameters.
	 *      @param [in] parent              Pointer to the owner of the model.
	 */
	MemoryMapsModel(QSharedPointer<Component> component, QSharedPointer<ParameterFinder> parameterFinder,
        QObject *parent);
	
	/*!
	 *  The destructor.
	 */
	virtual ~MemoryMapsModel();

    /*!
	 *  Get the number of rows an item contains.
	 *
	 *      @param [in] parent  Identifies the parent of the index that's row count is requested.
     *
     *      @return The number of rows an item has.
	 */
	virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;

    /*!
	 *  Get the number of columns the item has.
	 *
	 *      @param [in] parent  Identifies the parent of the index that's column count is requested.
     *
     *      @return The number of columns an item has.
	 */
	virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;

	/*!
	 *  Get the item flags that defines the possible operations for the item.
	 *
	 *      @param [in] index   The item's model index.
     *
     *      @return The possible operations for the item.
	 */
	Qt::ItemFlags flags(const QModelIndex& index) const;

    /*!
	 *  Get the header data for the specified header.
	 *
	 *      @param [in] section         The row / column number of the header.
	 *      @param [in] orientation     Specifies the orientation of the header.
	 *      @param [in] role            The type of the requested data.
     *
     *      @return The requested data.
	 */
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	
    /*!
	 *  Get the data for the specified item.
	 *
	 *      @param [in] index   Specifies the item that's data is requested.
	 *      @param [in] role    The role that defines what kind of data is requested.
     *
     *      @return The data for the item.
	 */
	virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

    /*!
	 *  Save the data to the model for the specified item.
	 *
	 *      @param [in] index   The index of the item that's data is to be saved.
	 *      @param [in] value   The data that is to be saved.
	 *      @param [in] role    Specifies what kind of data should be saved.
     *
     *      @return True, if saving was successful, false otherwise.
	 */
	bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);

    /*!
	 *  Check if the Memory maps model is in a valid state.
	 */
	bool isValid() const;

    /*!
     *  Get the model index of the specified object.
     *
     *      @param [in] row     Row number of the object.
     *      @param [in] column  Column number of the object.
     *      @param [in] parent  Model index of the parent of the object.
     *
     *      @return QModelIndex that identifies the object.
     */
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex() ) const;

    /*!
     *  Get the model index of the parent of the object.
     *
     *      @param [in] child   Model index that identifies the child of the object.
     *
     *      @return QModelindex that identifies the parent of the given object.
     */
    QModelIndex parent(const QModelIndex &child) const;

public slots:

    /*!
	 *  Add a new item to the given index.
	 *
	 *      @param [in] index   The index identifying the position for the new item.
	 */
	virtual void onAddItem(const QModelIndex& index);

    /*!
	 *  Remove the item in the given index.
	 *
	 *      @param [in] index   The index identifying the item to be removed.
	 */
	virtual void onRemoveItem(const QModelIndex& index);

    /*!
     *  Add a new memory remap item.
     *
     *      @param [in] index   The parent index of the new memory remap item.
     */
    virtual void onAddMemoryRemap(const QModelIndex& index);

signals:

	//! Emitted when the contents of the model change.
	void contentChanged();

    /*!
     *  Informs of a need to redraw the visualization.
     */
    void graphicsChanged();

	//! Emitted when a new memory map is added to the given index.
	void memoryMapAdded(int index);

	//! Emitted when a memory map is removed from the given index.
	void memoryMapRemoved(int index);

    //! Emitted when address unit bits are changed.
    void aubChangedOnRow(int memoryMapIndex);
    
    //! Emitted when a new memory remap is added to the given memorymap.
    void memoryRemapAdded(int index, QSharedPointer<MemoryMap> parentMemoryMap);

    //! Emitted when a memory remap is removed from the given index.
    void memoryRemapRemoved(int index, QSharedPointer<MemoryMap> parentMemoryMap);

    /*!
     *  Informs of an increase in references for a given parameter.
     *
     *      @param [in] id  The id of the given parameter.
     */
    void increaseReferences(QString const& id);

    /*!
     *  Informs of a decrease in references for a given parameter.
     *
     *      @param [in] id  The id of the given parameter.
     */
    void decreaseReferences(QString const& id);

private:
	
    //! No copying
    MemoryMapsModel(const MemoryMapsModel& other);

    //! No assignment
    MemoryMapsModel& operator=(const MemoryMapsModel& other);

    /*!
     *  Increaser the number of references when creating a new memory remap.
     *
     *      @param [in] parentMemoryMap     The parent memory map of the memory remap.
     */
    void increaseReferencesWithNewRemap(QSharedPointer<MemoryMap> parentMemoryMap);

    /*!
     *  Decrease the number of references when removing a memory remap.
     *
     *      @param [in] removedMemoryRemap  The removed memory remap.
     */
    void decreaseReferencesWithRemovedMemoryRemap(QSharedPointer<MemoryRemap> removedMemoryRemap);

    /*!
     *  Get the referenced parameters in the given abstract memory map.
     *
     *      @param [in] memoryMap   The given memory map.
     */
    QMap<QString, int> getReferencedParameters(QSharedPointer<AbstractMemoryMap> memoryMap) const;

    /*!
     *  Decrease the number of references when removing a memory map.
     *
     *      @param [in] removedMemoryMap    The removed memory map.
     */
    void decreaseReferencesWithRemovedMemoryMap(QSharedPointer<MemoryMap> removedMemoryMap);

    /*!
     *  Create a parent index for the index of a memory remap.
     *
     *      @param [in] childItem   The memory remap.
     *
     *      @return The parent index of the memory remap child item.
     */
    QModelIndex createParentIndexForMemoryRemap(MemoryRemap* childItem) const;

    /*!
     *  Get the indexed memory remap.
     *
     *      @param [in] parentIndex     The parent index of the memory remap.
     *      @param [in] row             The row of the memory remap.
     *
     *      @return Pointer to the indexed memory remap.
     */
    QSharedPointer<MemoryRemap> getIndexedMemoryRemap(QModelIndex const& parentIndex, int row) const;

    /*!
     *  Get the parent memory map of a memory remap.
     *
     *      @param [in] memoryRemap     The memory remap whose parent is wanted.
     *
     *      @return The parent memory map of the given memory remap.
     */
    QSharedPointer<MemoryMap> getParentMemoryMap(QSharedPointer<MemoryRemap> memoryRemap) const;

    /*!
     *  Check if the index is valid in the model.
     *
     *      @param [in] index   The index whose validity is being checked.
     *
     *      @return True, if the index is valid, false otherwise.
     */
    bool isIndexValid(QModelIndex const& index) const;

    /*!
     *  Gets the value corresponding to the index.
     *
     *      @param [in] index   The index of the item that's value is wanted.
     *
     *      @return The value of the item corresponding to the index.
     */
    QVariant valueForIndex(QModelIndex const& index) const;

    /*!
     *  Gets the foreground colour for the given index.
     *
     *      @param [in] index   The index whose foreground color is wanted.
     *
     *      @return Black for valid, red for invalid and gray for data that cannot be changed.
     */
    QColor getForegroundColor(QModelIndex const& index) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

	//! Pointer to the component that contains the memory maps to edit.
	QSharedPointer<Component> component_;

    //! The parameter finder.
    QSharedPointer<ParameterFinder> parameterFinder_;

	//! Contains the memory maps to show in the summary.
	QList<QSharedPointer<MemoryMap> >& rootMemoryMaps_;
};

#endif // MEMORYMAPSMODEL_H
