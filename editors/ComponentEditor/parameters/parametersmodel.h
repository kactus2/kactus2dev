//-----------------------------------------------------------------------------
// File: parametersmodel.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 04.04.2011
//
// Description:
// Table model that can be used to display parameters to be edited.
//-----------------------------------------------------------------------------

#ifndef PARAMETERSMODEL_H
#define PARAMETERSMODEL_H

#include <IPXACTmodels/common/Parameter.h>

#include <editors/ComponentEditor/common/AbstractParameterModel.h>
#include <editors/ComponentEditor/common/ExpressionFormatter.h>
#include <editors/ComponentEditor/common/ParameterFinder.h>

#include <QAbstractTableModel>

#include <QSharedPointer>
#include <QList>

class Choice;
class Component;
class ParametersInterface;

//-----------------------------------------------------------------------------
//! Table model that can be used to display parameters to be edited.
//-----------------------------------------------------------------------------
class ParametersModel : public AbstractParameterModel
{
	Q_OBJECT

public:

	/*!
     *  The constructor
	 *
     *      @param [in] parameterInterface  Interface for accessing parameters.
     *      @param [in] expressionParser    Expression parser for configurable elements.
     *      @param [in] parameterFinder     The parameter finder.
	 *      @param [in] parent              The owner of this model.
     */
    ParametersModel(ParametersInterface* parameterInterface,
        QSharedPointer<ExpressionParser> expressionParser,
        QSharedPointer<ParameterFinder> parameterFinder,
        QObject *parent);

	/*!
     *  The destructor.
     */
	virtual ~ParametersModel() = default;

	/*!
     *  Get the number of columns in the model.
	 *
	 *      @param [in] parent  Model index of the parent of the item.
	 *
	 *      @return  Number of columns currently in the model.
     */
	virtual int columnCount(QModelIndex const& parent = QModelIndex()) const;

    /*!
     *  Reset the model.
     */
    void resetModelItems();

public slots:

	/*!
     *  A new item should be added to given index.
	 *
	 *      @param [in]  index The position where new item should be added at.
     */
	virtual void onAddItem(QModelIndex const& index);

	/*!
     *  An item should be removed from the model.
	 * 
	 *      @param [in]  index Identifies the item that should be removed.
     */
	virtual void onRemoveItem(QModelIndex const& index);

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
     *  Gets the column index for parameter ids.
     *
     *      @return     The column index for parameter ids.
     */
    virtual int idColumn() const;

    /*!
     *  Gets the column index for usage count.
     *
     *      @return     The column index for usage count.
     */
    virtual int usageCountColumn() const;

private:

	//! No copying
	ParametersModel(const ParametersModel& other);

	//! No assignment
	ParametersModel& operator=(const ParametersModel& other);
};

#endif // PARAMETERSMODEL_H
