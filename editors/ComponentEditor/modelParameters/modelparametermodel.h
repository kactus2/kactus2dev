//-----------------------------------------------------------------------------
// File: modelparametermodel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 29.3.2011
//
// Description:
// This model can be used to edit and update the model parameters of a component
//-----------------------------------------------------------------------------

#ifndef MODELPARAMETERMODEL_H
#define MODELPARAMETERMODEL_H

#include <editors/ComponentEditor/common/AbstractParameterModel.h>

#include <QSharedPointer>
#include <QString>

class Component;
class ExpressionParser;
class Model;
class ModelParameter;

//-----------------------------------------------------------------------------
//! This model can be used to edit and update the model parameters of a component
//-----------------------------------------------------------------------------
class ModelParameterModel : public AbstractParameterModel
{
	Q_OBJECT

public:

	/*! The constructor
	 *
	 *      @param [in]  model               The model being edited.
     *      @param [in]  choices             The choices available for the model parameter values.
     *      @param [in]  expressionParser    Expression parser for configurable elements.
	 *      @param [in]  parent              The owner of this model.
	*/
    ModelParameterModel(QSharedPointer<Model> model, 
        QSharedPointer<QList<QSharedPointer<Choice> > > choices,
        QSharedPointer<ExpressionParser> expressionParser,
        QObject *parent);

	//! The destructor
	virtual ~ModelParameterModel();

	/*! Get the number of rows in the model.
	 *
	 *      @param [in]  parent Model index of the parent of the item. Must be invalid
     *                          because this is not hierarchical model.
	 *
	 *      @return  Number of rows currently in the model.
	*/
	virtual int rowCount(QModelIndex const& parent = QModelIndex()) const;

	/*! Get the number of columns in the model
	 *
	 *      @param [in]  parent Model index of the parent of the item. Must be invalid
	 *                          because this is not hierarchical model.
	 *
	 *      @return  Number of columns currently in the model.
	*/
	virtual int columnCount(const QModelIndex& parent = QModelIndex() ) const;

	/*! Get the data for the specified item for specified role.
	 *
	 *      @param [in]  index  Identifies the item that's data is wanted.
	 *      @param [in]  role   Specifies what kind of data is wanted.
	 *
	 *      @return  The data for the given index.
	*/
	virtual QVariant data(QModelIndex const& index, int role = Qt::DisplayRole) const;

	/*! Get the data for the headers
	 *
	 *      @param [in]  section        The column that's header is wanted.
	 *      @param [in]  orientation    The orientation of the header data.
	 *      @param [in]  role           Specified the type of data that is wanted.
	 *
	 *      @return  The header data for the given section.
	*/
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;

	/*! Get information on how specified item can be handled.
	 *
	 *      @param [in]  index Specifies the item that's flags are wanted.
	 *
	 *      @return  Qt::ItemFlags that define how object can be handled.
	*/
	virtual Qt::ItemFlags flags(QModelIndex const& index) const;

	/*! Set the data for specified item.
	 *
	 *      @param [in]  index  Specifies the item that's data is modified
	 *      @param [in]  value  The value to be set.
	 *      @param [in]  role   The role that is trying to modify the data. Only Qt::EditRole is supported.
	 *
	 *      @return  True, if data was successfully set, otherwise false.
	*/
	virtual bool setData(QModelIndex const& index, const QVariant& value, int role = Qt::EditRole);

    /*!
     *  Gets the model parameter in the given index.
     *
     *      @param [in] index   The index where to get the model parameter.
     *
     *      @return The model parameter whose data is in the given index.
     */
    virtual QSharedPointer<ModelParameter> getParameter(QModelIndex const& index) const;    
       
    /*!
     *  Sets the edited model and locks all current model parameters.
     *
     *      @param [in] component   The model whose model parameters to edit.
     */
    void setModelAndLockCurrentModelParameters(QSharedPointer<Model> model);

public slots:

	/*! A new item should be added to given index.
	 *
	 *      @param [in]  index The position where new item should be added at.
	 *
	*/
	virtual void onAddItem(const QModelIndex& index);

	/*! An item should be removed from the model.
	 * 
	 *      @param [in]  index Identifies the item that should be removed.
	 *
	*/
	virtual void onRemoveItem(const QModelIndex& index);

protected:
            
    /*!
     *  Gets the parameter on the given row.
     *
     *      @param [in] row   The row number where to get the parameter from.
     *
     *      @return The parameter on the given row.
     */
    virtual QSharedPointer<Parameter> getParameterOnRow(int row) const;

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
     *  Gets the column for value bit string length.
     *
     *      @return The column for editing bit string length.
     */
    virtual int bitWidthColumn() const;

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
     *  Gets the column for array size.
     *
     *      @return The column for editing array size.
     */
    virtual int arraySizeColumn() const;
        
    /*!
     *  Gets the column for array offset.
     *
     *      @return The column for editing array offset.
     */
    virtual int arrayOffsetColumn() const;
        
    /*!
     *  Gets the column for description.
     *
     *      @return The column for editing description.
     */
    virtual int descriptionColumn() const;

    /*!
     *  Gets the column index for description.
     *
     *      @return     The column index for value id.
     */
    virtual int valueIdColumn() const;

    /*!
     *  Gets the column index for usage count.
     *
     *      @return     The column index for usage count.
     */
    virtual int usageCountColumn() const;

    /*!
     *  Validates the data in a model parameter corresponding to a given column.
     *
     *      @param [in] column      The column whose data to validate.
     *      @param [in] parameter   The model parameter whose data to validate.
     *
     *      @return True, if the data in the parameter is valid, otherwise false.
     */
    virtual bool validateColumnForParameter(QModelIndex const& index) const;

private:

	//! No copying
	ModelParameterModel(const ModelParameterModel& other);

	//! No assignment
	ModelParameterModel& operator=(const ModelParameterModel& other);

    /*!
     *   Locks the name, type and usage columns of a parameter model.
     *
     *      @param [in] modelParam The parameter model to lock.
     */
    void lockModelParameter(QSharedPointer<ModelParameter> modelParam);
    
	/*! Get the index of the given model parameter.
	 *
	 *      @param [in]  modelParam     The model parameter whose index is requested.
	 *
	 *      @return The index of the first column of the model parameter.
	*/
	virtual QModelIndex indexFor(QSharedPointer<ModelParameter> modelParam) const;

    /*!
     *   Locks the given index disabling editing.
     *
     *      @param [in] index The index to lock.
     */
    void lockIndex(QModelIndex const& index);

    /*!
     *   Checks if given index is locked.
     *
     *      @param [in] index The index to check.
	 *
	 *      @return True if the index is locked, otherwise false.
     */
    bool isLocked(QModelIndex const& index) const;

    //! The model whose model parameters to edit.
    QSharedPointer<Model> model_;

    //! The locked indexes that cannot be edited.
    QList<QPersistentModelIndex> lockedIndexes_;
};

#endif // MODELPARAMETERMODEL_H
