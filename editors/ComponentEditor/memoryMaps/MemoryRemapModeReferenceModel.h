//-----------------------------------------------------------------------------
// File: MemoryRemapModeReferenceModel.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 16.10.2023
//
// Description:
// Model to manage mode references of a memory remap.
//-----------------------------------------------------------------------------

#ifndef MEMORYREMAPMODEREFERENCEMODEL_H
#define MEMORYREMAPMODEREFERENCEMODEL_H

#include <editors/ComponentEditor/common/ModeReferenceModel.h>

class MemoryMapInterface;

class MemoryRemapModeReferenceModel : public ModeReferenceModel
{
public:

	/*!
	 *	The constructor.
	 *  
	 *      @param [in] memMapInterface     The memory map interface used to access mode references.
	 *      @param [in] memMapName          The name of the containing memory map.
	 *      @param [in] remapName           The name of the remap being edited.
	 *      @param [in] parent              The parent object.
	 */
	MemoryRemapModeReferenceModel(MemoryMapInterface* memMapInterface, QString const& memMapName, 
        QString const& remapName, QObject* parent);
	
	virtual ~MemoryRemapModeReferenceModel() = default;

	// No copying.
	MemoryRemapModeReferenceModel(MemoryRemapModeReferenceModel const& other) = delete;
	MemoryRemapModeReferenceModel& operator=(MemoryRemapModeReferenceModel const& other) = delete;

    /*!
     *  Get the number of modes the model contains.
     *
     *      @param [in] parent  Identifies the parent whose row count is requested.
     *
     *      @return Number of rows the model has.
     */
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;

    /*!
     *  Get the data for specified item.
     *
     *      @param [in] index   Specifies the item that's data is requested.
     *      @param [in] role    The role that defines what kind of data is requested.
     *
     *      @return QVariant containing the data for the item.
     */
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    /*!
     *  Save the data to the model for specified item.
     *
     *      @param [in] index   The model index of the item that's data is to be saved.
     *      @param [in] value   The data that is to be saved.
     *      @param [in] role    The role specifies what kind of data should be saved.
     *
     *      @return True if saving happened successfully.
     */
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;

public slots:

    /*!
     *	Handler for adding a new mode reference row.
     *
     *      @param [in] index     The model index that was selected.
     */
    void onAddRow(QModelIndex const& index) override;

    /*!
     *	Handler for removing a mode reference row.
     *
     *      @param [in] index     The mode reference to be removed.
     */
    void onRemoveItem(QModelIndex const& index) override;

private:

    //! The memory (re)map interface.
	MemoryMapInterface* interface_;

    //! The name of the memory map containing the remap.
    std::string memMapName_;

    //! The name of the remap.
    std::string remapName_;

};


#endif // MEMORYREMAPMODEREFERENCEMODEL_H
