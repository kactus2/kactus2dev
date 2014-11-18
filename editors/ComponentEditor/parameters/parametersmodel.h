//-----------------------------------------------------------------------------
// File: parametersmodel.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 4.4.2011
//
// Description:
// Table model that can be used to display parameters to be edited.
//-----------------------------------------------------------------------------

#ifndef PARAMETERSMODEL_H
#define PARAMETERSMODEL_H

#include <IPXACTmodels/parameter.h>

#include <editors/ComponentEditor/common/AbstractParameterModel.h>

#include <QAbstractTableModel>

#include <QSharedPointer>
#include <QList>

class Choice;
class Component;

//-----------------------------------------------------------------------------
//! Table model that can be used to display parameters to be edited.
//-----------------------------------------------------------------------------
class ParametersModel : public AbstractParameterModel
{
	Q_OBJECT

public:

	/*! The constructor
	 *
	 *      @param [in]  parameters The parameters to edit in the model.
     *      @param [in]  choices    The choices available for the parameter values.
	 *      @param [in]  parent     The owner of this model.
	 *
	*/
	ParametersModel(QList<QSharedPointer<Parameter> >& parameters, 
        QSharedPointer<QList<QSharedPointer<Choice> > > choices,
		QObject *parent);
	
	//! The destructor
	virtual ~ParametersModel();

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
	virtual int columnCount(QModelIndex const& parent = QModelIndex()) const;

public slots:

	/*! A new item should be added to given index.
	 *
	 *      @param [in]  index The position where new item should be added at.
	 *
	*/
	virtual void onAddItem(QModelIndex const& index);

	/*! An item should be removed from the model.
	 * 
	 *      @param [in]  index Identifies the item that should be removed.
	 *
	*/
	virtual void onRemoveItem(QModelIndex const& index);

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
    virtual int formatColumn() const;
        
    /*!
     *  Gets the column for value bit string length.
     *
     *      @return The column for editing bit string length.
     */
    virtual int bitwidthColumn() const;

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

private:

	//! No copying
	ParametersModel(const ParametersModel& other);

	//! No assignment
	ParametersModel& operator=(const ParametersModel& other);
	
	//! The parameters to be edited.
	QList<QSharedPointer<Parameter> >& parameters_;
};

#endif // PARAMETERSMODEL_H
