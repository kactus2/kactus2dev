//-----------------------------------------------------------------------------
// File: memorymapsmodel.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 21.08.2012
//
// Description:
// The model to manage the memory maps summary.
//-----------------------------------------------------------------------------

#ifndef MEMORYMAPSMODEL_H
#define MEMORYMAPSMODEL_H

#include <IPXACTmodels/common/Document.h>

#include <editors/ComponentEditor/common/ParameterizableTable.h>

#include <KactusAPI/include/ParameterFinder.h>

#include <QAbstractItemModel>
#include <QSharedPointer>

#include <QMap>

class Component;
class ExpressionFormatter;
class MemoryMapBase;
class MemoryMap;
class MemoryRemap;
class MemoryMapValidator;
class MemoryMapExpressionGatherer;
class MemoryRemapExpressionGatherer;
class ReferenceCalculator;
class MemoryMapInterface;

//-----------------------------------------------------------------------------
//! The model to manage the memory maps summary.
//-----------------------------------------------------------------------------
class MemoryMapsModel : public QAbstractItemModel, public ParameterizableTable
{
	Q_OBJECT

public:

	/*!
	 *  The constructor.
	 *
     *    @param [in] parameterFinder     The instance used to find parameters.
     *    @param [in] expressionParser    The used expression parser.
     *    @param [in] mapInterface        Interface for memory maps.
     *    @param [in] docRevision         The IP-XACT standard revision to use.
     *    @param [in] ROWHEIGHT           Height for rows.
	 *    @param [in] parent              The owner of the model.
	 */
	MemoryMapsModel( QSharedPointer<ParameterFinder> parameterFinder, 
        QSharedPointer<ExpressionParser> expressionParser, MemoryMapInterface* mapInterface, 
        Document::Revision docRevision, const int ROWHEIGHT, QObject *parent);

    //! No copying
    MemoryMapsModel(const MemoryMapsModel& other) = delete;

    //! No assignment
    MemoryMapsModel& operator=(const MemoryMapsModel& other) = delete;

	/*!
	 *  The destructor.
	 */
	virtual ~MemoryMapsModel() = default;

    /*!
	 *  Get the number of rows an item contains.
	 *
	 *    @param [in] parent  Identifies the parent of the index that's row count is requested.
     *
     *    @return The number of rows an item has.
	 */
	virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;

    /*!
	 *  Get the number of columns the item has.
	 *
	 *    @param [in] parent  Identifies the parent of the index that's column count is requested.
     *
     *    @return The number of columns an item has.
	 */
	virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;

	/*!
	 *  Get the item flags that defines the possible operations for the item.
	 *
	 *    @param [in] index   The item's model index.
     *
     *    @return The possible operations for the item.
	 */
	Qt::ItemFlags flags(const QModelIndex& index) const;

    /*!
	 *  Get the header data for the specified header.
	 *
	 *    @param [in] section         The row / column number of the header.
	 *    @param [in] orientation     Specifies the orientation of the header.
	 *    @param [in] role            The type of the requested data.
     *
     *    @return The requested data.
	 */
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	
    /*!
	 *  Get the data for the specified item.
	 *
	 *    @param [in] index   Specifies the item that's data is requested.
	 *    @param [in] role    The role that defines what kind of data is requested.
     *
     *    @return The data for the item.
	 */
	virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

    /*!
	 *  Save the data to the model for the specified item.
	 *
	 *    @param [in] index   The index of the item that's data is to be saved.
	 *    @param [in] value   The data that is to be saved.
	 *    @param [in] role    Specifies what kind of data should be saved.
     *
     *    @return True, if saving was successful, false otherwise.
	 */
	bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);

    /*!
     *  Get the model index of the specified object.
     *
     *    @param [in] row     Row number of the object.
     *    @param [in] column  Column number of the object.
     *    @param [in] parent  Model index of the parent of the object.
     *
     *    @return QModelIndex that identifies the object.
     */
    virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex() ) const;

    /*!
     *  Get the model index of the parent of the object.
     *
     *    @param [in] child   Model index that identifies the child of the object.
     *
     *    @return QModelindex that identifies the parent of the given object.
     */
    virtual QModelIndex parent(const QModelIndex &child) const;
    
    /*!
     *  Get the list of acceptable mime types.
     *
     *    @return The list of acceptable mime types.
     */
    virtual QStringList mimeTypes() const;

public slots:

    /*!
	 *  Add a new item to the given index.
	 *
	 *    @param [in] index   The index identifying the position for the new item.
	 */
	virtual void onAddItem(const QModelIndex& index);

    /*!
	 *  Remove the item in the given index.
	 *
	 *    @param [in] index   The index identifying the item to be removed.
	 */
	virtual void onRemoveItem(const QModelIndex& index);

    /*!
     *  Add a new memory remap item.
     *
     *    @param [in] index   The parent index of the new memory remap item.
     */
    virtual void onAddMemoryRemap(const QModelIndex& index);

    /*!
     *  Copy the selected items matching the selected indexes.
     *
     *    @param [in] indexList   A list of the selected indexes.
     */
    void onCopyRows(QModelIndexList const& indexList);

    /*!
     *  Paste the selected items.
     *
     *    @param [in] index   The owner of the pasted memory remap items.
     */
    void onPasteRows(QModelIndex const& index);

    /*!
     *  Remove all memory remaps from the selected memory map.
     *
     *    @param [in] itemIndex   Index of the selected memory map.
     */
    void onRemoveAllChildItemsFrom(QModelIndex const& itemIndex);

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
    
    //! Emitted when a new memory remap is added to the given memory map.
    void memoryRemapAdded(int index, QString const& parentMemoryMapName);

    //! Emitted when a memory remap is removed from the given index.
    void memoryRemapRemoved(int index, QString const& parentMemoryMapName);

    /*!
     *  Informs of an increase in references for a given parameter.
     *
     *    @param [in] id  The id of the given parameter.
     */
    void increaseReferences(QString const& id);

    /*!
     *  Informs of a decrease in references for a given parameter.
     *
     *    @param [in] id  The id of the given parameter.
     */
    void decreaseReferences(QString const& id);

    /*
     *  Informs of memory map name change.
     *
     *    @param [in] oldName     The old name.
     *    @param [in] newName     The new name.
     */
    void memoryMapNameChanged(QString const& oldName, QString const& newName);

    /*
     *  Informs of memory remap name change.
     *
     *    @param [in] parentName  Name of the containing memory map.
     *    @param [in] oldName     The old name.
     *    @param [in] newName     The new name.
     */
    void memoryRemapNameChanged(QString const& parentName, QString const& oldName, QString const& newName);

protected:
    
    /*!
     *  Check if the column index is valid for containing expressions.
     *
     *    @param [in] index   The index being evaluated.
     *
     *    @return     True, if column can have expressions, false otherwise.
     */
    virtual bool isValidExpressionColumn(QModelIndex const& index) const;

    /*!
     *  Gets the expression for the given index or the plain value if expression is not available.
     *
     *    @param [in] index   The index whose expression to get.
     *
     *    @return The expression for the index if available, otherwise the value for the given index.
     */
    virtual QVariant expressionOrValueForIndex(QModelIndex const& index) const;

    /*!
     *  Validates the data in an index.
     *
     *    @param [in] index   The index whose data to validate
     *
     *    @return True, if the data in the index is valid, otherwise false.
     */
    virtual bool validateIndex(QModelIndex const& index) const;

private:
    
    /*!
     *  Get the formatted value of an expression in the selected index.
     *
     *    @param [in] index   The selected index.
     *
     *    @return The formatted value of an expression in the selected index.
     */
    virtual QVariant formattedExpressionForIndex(QModelIndex const& index) const;

    /*!
     *  Get the expression of the selected index.
     *
     *    @param [in] index   The selected index.
     *
     *    @return The expression of the selected index.
     */
    virtual QVariant expressionForIndex(QModelIndex const& index) const;

    /*!
     *  Gets the value for the given index.
     *
     *    @param [in] index   The index of target data.
     *
     *    @return     The data in the given index.
     */
    QVariant valueForIndex(QModelIndex const& index) const;

    /*!
     *  Increaser the number of references when creating a new memory remap.
     *
     *    @param [in] memoryMapName   Name of the memory map.
     */
    void increaseReferencesWithNewRemap(QString const& memoryMapName);

    /*!
     *  Decrease the number of references when removing a memory remap.
     *
     *    @param [in] mapName     Name of the memory map containing the selected memory remap.
     *    @param [in] remapName   Name of the removed memory remap.
     */
    void decreaseReferencesWithRemovedMemoryMap(QString const& mapName, QString const& remapName);

    /*!
     *  Create a parent index for the index of a memory remap.
     *
     *    @param [in] childItem   The memory remap.
     *
     *    @return The parent index of the memory remap child item.
     */
    QModelIndex createParentIndexForMemoryRemap(MemoryRemap* childItem) const;

    /*!
     *  Check if the index is valid in the model.
     *
     *    @param [in] index   The index whose validity is being checked.
     *
     *    @return True, if the index is valid, false otherwise.
     */
    bool isIndexValid(QModelIndex const& index) const;

    /*!
     *  Increase the number of references in the referenced parameters.
     *
     *    @param [in] mapExpressions          List of expressions contained within the selected map.
     *    @param [in] referenceCalculator     The used reference calculator.
     */
    void increaseReferencesInPastedMap(QStringList mapExpressions, ReferenceCalculator& referenceCalculator);

    /*!
     *  Get the memory map and remap names associated with the selected index.
     *
     *    @param [in] index   The selected index.
     *
     *    @return Pair containing the memory map and memory remap names.
     */
    QPair<QString, QString> getIndexedMapRemapNames(QModelIndex const& index) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Interface for accessing memory maps.
    MemoryMapInterface* mapInterface_;

    //! The IP-XACT standard revision to use.
    Document::Revision docRevision_;

    //! Height for rows.
    const int ROWHEIGHT_;
};

#endif // MEMORYMAPSMODEL_H
