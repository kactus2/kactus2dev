//-----------------------------------------------------------------------------
// File: ModuleParameterModel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 29.3.2011
//
// Description:
// This model can be used to edit and update a list of module parameters.
//-----------------------------------------------------------------------------

#ifndef MODULEPARAMETERMODEL_H
#define MODULEPARAMETERMODEL_H

#include <editors/ComponentEditor/common/AbstractParameterModel.h>
#include <KactusAPI/include/ExpressionFormatter.h>
#include <KactusAPI/include/ParameterFinder.h>
#include <KactusAPI/include/ModuleParameterInterface.h>

#include <QSharedPointer>
#include <QString>

class Component;
class ExpressionParser;
class ModuleParameter;

//-----------------------------------------------------------------------------
//! This model can be used to edit and update a list of module parameters.
//-----------------------------------------------------------------------------
class ModuleParameterModel : public AbstractParameterModel
{
	Q_OBJECT

public:

	/*!
     *  The constructor.
	 *
     *      @param [in] moduleParameterInterface    Interface for accessing module parameters.
     *      @param [in] expressionParser            Expression parser for configurable elements.
     *      @param [in] parameterFinder             The parameter finder.
	 *      @param [in] parent                      The owner of this model.
     */
    ModuleParameterModel(ModuleParameterInterface* moduleParameterInterface,
        QSharedPointer<ExpressionParser> expressionParser,
        QSharedPointer<ParameterFinder> parameterFinder,
        QObject *parent);

	/*!
     *  The destructor.
     */
	virtual ~ModuleParameterModel() = default;

	/*!
     *  Get the number of columns in the model
	 *
	 *      @param [in]  parent Model index of the parent of the item.
	 *
	 *      @return  Number of columns currently in the model.
	 */
	virtual int columnCount(const QModelIndex& parent = QModelIndex() ) const;

	/*!
     *  Get the data for the specified item for specified role.
	 *
	 *      @param [in]  index  Identifies the item that's data is wanted.
	 *      @param [in]  role   Specifies what kind of data is wanted.
	 *
	 *      @return  The data for the given index.
	 */
	virtual QVariant data(QModelIndex const& index, int role = Qt::DisplayRole) const;

	/*!
     *  Get the data for the headers.
	 *
	 *      @param [in]  section        The column that's header is wanted.
	 *      @param [in]  orientation    The orientation of the header data.
	 *      @param [in]  role           Specified the type of data that is wanted.
	 *
	 *      @return  The header data for the given section.
	 */
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;

	/*!
     *  Get information on how specified item can be handled.
	 *
	 *      @param [in]  index Specifies the item that's flags are wanted.
	 *
	 *      @return  Qt::ItemFlags that define how object can be handled.
	 */
	virtual Qt::ItemFlags flags(QModelIndex const& index) const;

	/*!
     *  Set the data for specified item.
	 *
	 *      @param [in]  index  Specifies the item that's data is modified
	 *      @param [in]  value  The value to be set.
	 *      @param [in]  role   The role that is trying to modify the data. Only Qt::EditRole is supported.
	 *
	 *      @return  True, if data was successfully set, otherwise false.
	 */
	virtual bool setData(QModelIndex const& index, const QVariant& value, int role = Qt::EditRole);

    /*!
     *  Reset the model.
     */
    void resetModelItems();

    /*!
     *  Enables the editing of module parameters in the model.
     */
    void enableEditing();

    /*!
     *  Disables the editing of module parameters in the model.
     */
    void disableEditing();

public slots:

	/*!
     *  A new item should be added to given index.
	 *
	 *      @param [in]  index The position where new item should be added at.
	 */
	virtual void onAddItem(const QModelIndex& index);

	/*!
     *  An item should be removed from the model.
	 * 
	 *      @param [in]  index Identifies the item that should be removed.
	 */
	virtual void onRemoveItem(const QModelIndex& index);

protected:

    /*!
     *  Gets the column for value format.
     *
     *      @return The column for editing format selection.
     */
    virtual int nameColumn() const;
        
    /*!
     *  Gets the column for value format.
     *
     *      @return The column for editing format selection.
     */
    virtual int displayNameColumn() const;

    /*!
     *  Gets the column for value format.
     *
     *      @return The column for editing format selection.
     */
    virtual int typeColumn() const;

    /*!
     *  Gets the column for the left value of bit width vector.
     *
     *      @return The column for editing the left value of the bit width vector.
     */
    virtual int bitWidthLeftColumn() const;

    /*!
     *  Gets the column for the right value of bit width vector.
     *
     *      @return The column for editing the right value of the bit width vector.
     */
    virtual int bitWidthRightColumn() const;

    /*!
     *  Gets the column for minimum value.
     *
     *      @return The column for editing the minimum value.
     */
    virtual int minimumColumn() const;
        
    /*!
     *  Gets the column for maximum value.
     *
     *      @return The column for editing the maximum value.
     */
    virtual int maximumColumn() const;
        
    /*!
     *  Gets the column for choices.
     *
     *      @return The column for editing choice selection.
     */
    virtual int choiceColumn() const;

    /*!
     *  Gets the column for values.
     *
     *      @return The column for editing value selection.
     */
    virtual int valueColumn() const;
    
    /*!
     *  Gets the column for resolve.
     *
     *      @return The column for editing value selection.
     */
    virtual int resolveColumn() const;
    
    /*!
     *  Gets the column for array left.
     *
     *      @return The column for editing array left.
     */
    virtual int arrayLeftColumn() const;

    /*!
     *  Gets the column for array right.
     *
     *      @return The column for editing array right.
     */
    virtual int arrayRightColumn() const;

    /*!
     *  Gets the column for description.
     *
     *      @return The column for editing description.
     */
    virtual int descriptionColumn() const;

    /*!
     *  Gets the column index for model parameter id.
     *
     *      @return     The column index for the model parameter id.
     */
    virtual int idColumn() const;

    /*!
     *  Gets the column index for usage count.
     *
     *      @return     The column index for usage count.
     */
    virtual int usageCountColumn() const;

private:

	//! No copying.
	ModuleParameterModel(const ModuleParameterModel& other);

	//! No assignment.
	ModuleParameterModel& operator=(const ModuleParameterModel& other);
    
    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Disable for editing of module parameters.
    bool editingDisabled_;
};

#endif // MODULEPARAMETERMODEL_H
