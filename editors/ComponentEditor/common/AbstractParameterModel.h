//-----------------------------------------------------------------------------
// File: AbstractParameterModel.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 18.11.2014
//
// Description:
// Base class for models editing parameters and model parameters.
//-----------------------------------------------------------------------------

#ifndef ABSTRACTPARAMETERMODEL_H
#define ABSTRACTPARAMETERMODEL_H

#include <IPXACTmodels/parameter.h>

#include <QAbstractTableModel>

#include <QSharedPointer>
#include <QList>

class Choice;
class Component;
class ExpressionParser;

//-----------------------------------------------------------------------------
//! Base class for models editing parameters and model parameters.
//-----------------------------------------------------------------------------
class AbstractParameterModel : public QAbstractTableModel 
{
	Q_OBJECT

public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] choices             The choices available for the parameter values.
     *      @param [in] expressionParser    Expression parser for configurable elements.
	 *      @param [in] parent              The parent object.
	 */
	AbstractParameterModel(QSharedPointer<QList<QSharedPointer<Choice> > > choices,
	QSharedPointer<ExpressionParser> expressionParser,
        QObject *parent);
	
    //! The destructor.
	virtual ~AbstractParameterModel();

	/*! Get the data for the specified item for specified role.
	 *
	 *      @param [in]  index  Identifies the item that's data is wanted.
	 *      @param [in]  role   Specifies what kind of data is wanted.
	 *
	 *      @return  The data for the given index.
	*/
	virtual QVariant data(QModelIndex const& index, int role = Qt::DisplayRole ) const;

	/*! Get the data for the headers
	 *
	 *      @param [in]  section        The column that's header is wanted.
	 *      @param [in]  orientation    The orientation of the header data.
	 *      @param [in]  role           Specified the type of data that is wanted.
	 *
	 *      @return  The header data for the given section.
	*/
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;

	/*! Set the data for specified item.
	 *
	 *      @param [in]  index  Specifies the item that's data is modified
	 *      @param [in]  value  The value to be set.
	 *      @param [in]  role   The role that is trying to modify the data. Only Qt::EditRole is supported.
	 *
	 *      @return  True, if data was successfully set, otherwise false.
	*/
	virtual bool setData(QModelIndex const& index, const QVariant& value, int role = Qt::EditRole );

	/*! Get information on how specified item can be handled.
	 *
	 *      @param [in]  index Specifies the item that's flags are wanted.
	 *
	 *      @return  Qt::ItemFlags that define how object can be handled.
	*/
	virtual Qt::ItemFlags flags(QModelIndex const& index) const;
    
	/*! Check if the model is in valid state or not.
	 *
	 *      @return  True, if all items in model are valid, otherwise false.
	*/
	virtual bool isValid() const;
    
    /*!
     *  Check if the model is in valid state or not.
     *
     *           @param [in]  [inout] errorList           The list to add the possible error messages to.
     *           @param [in]  [in]    parentIdentifier    String to identify the location of the error.
     *
     *           @return  True if all items in model are valid.
     */
    virtual bool isValid(QStringList& errorList, const QString& parentIdentifier) const;

public slots:

	/*! A new item should be added to given index.
	 *
	 *      @param [in]  index The position where new item should be added at.
	*/
	virtual void onAddItem(QModelIndex const& index) = 0;

	/*! An item should be removed from the model.
	 * 
	 *      @param [in]  index Identifies the item that should be removed.
	*/
	virtual void onRemoveItem(QModelIndex const& index) = 0;

signals:

	//! Emitted when contents of the model change.
	void contentChanged();

	//! Prints an error message to the user.
	void errorMessage(const QString& msg) const;
	
	//! Prints a notification to user.
	void noticeMessage(const QString& msg) const;

protected:
                  
    /*!
     *  Gets the parameter on the given row.
     *
     *      @param [in] row   The row number where to get the parameter from.
     *
     *      @return The parameter on the given row.
     */  
    virtual QSharedPointer<Parameter> getParameterOnRow(int row) const = 0;

    /*!
     *  Gets the column for name.
     *
     *      @return The column for editing name selection.
     */
    virtual int nameColumn() const = 0;
        
    /*!
     *  Gets the column for display name.
     *
     *      @return The column for editing display name selection.
     */
    virtual int displayNameColumn() const = 0;

    /*!
     *  Gets the column for value type.
     *
     *      @return The column for editing type selection.
     */
    virtual int typeColumn() const = 0;
        
    /*!
     *  Gets the column for value bit string length.
     *
     *      @return The column for editing bit string length.
     */
    virtual int bitWidthColumn() const = 0;

    /*!
     *  Gets the column for minimum value.
     *
     *      @return The column for editing the minimum value.
     */
    virtual int minimumColumn() const = 0;
        
    /*!
     *  Gets the column for maximum value.
     *
     *      @return The column for editing the maximum value.
     */
    virtual int maximumColumn() const = 0;
        
    /*!
     *  Gets the column for choices.
     *
     *      @return The column for editing choice selection.
     */
    virtual int choiceColumn() const = 0;

    /*!
     *  Gets the column for values.
     *
     *      @return The column for editing value selection.
     */
    virtual int valueColumn() const = 0;
    
    /*!
     *  Gets the column for resolve.
     *
     *      @return The column for editing value selection.
     */
    virtual int resolveColumn() const = 0;
    
    /*!
     *  Gets the column for array size.
     *
     *      @return The column for editing array size.
     */
    virtual int arraySizeColumn() const = 0;
        
    /*!
     *  Gets the column for array offset.
     *
     *      @return The column for editing array offset.
     */
    virtual int arrayOffsetColumn() const = 0;
        
    /*!
     *  Gets the column for description.
     *
     *      @return The column for editing description.
     */
    virtual int descriptionColumn() const = 0;

    /*!
     *  Finds the value for the given model parameter using either selected choice or model parameter value.
     *
     *      @param [in] modelParameter   The model parameter whose value to find.
     *
     *      @return The value to display for the model parameter.
     */
    QString evaluateValueFor(QSharedPointer<Parameter> parameter) const;

    /*!
     *  Finds the choice with the given name.
     *
     *      @param [in] choiceName   The name of the choice to find.
     *
     *      @return The found choice.
     */
    QSharedPointer<Choice> findChoice(QString const& choiceName) const;

    /*!
     *  Finds a human-readable value to display for a given enumeration.
     *
     *      @param [in] choice              The choice whose enumeration to find.
     *      @param [in] enumerationValue    The value used to search the enumeration.
     *
     *      @return A value for the enumeration to display.
     */
    QString findDisplayValueForEnumeration(QSharedPointer<Choice> choice, QString const& enumerationValue) const;
    
    /*!
     *  Validates the data in a parameter corresponding to a given column.
     *
     *      @param [in] column      The column whose data to validate.
     *      @param [in] parameter   The parameter whose data to validate.
     *
     *      @return True, if the data in the parameter is valid, otherwise false.
     */
    virtual bool validateColumnForParameter(int column, QSharedPointer<Parameter> parameter) const;

private:

	//! No copying
	AbstractParameterModel(const AbstractParameterModel& other);

	//! No assignment
	AbstractParameterModel& operator=(const AbstractParameterModel& other);

    //! The choices available for model parameter values.
    QSharedPointer<QList<QSharedPointer<Choice> > > choices_;

    //! Expression parser for configurable elements.
    QSharedPointer<ExpressionParser> expressionParser_;

};

#endif // ABSTRACTPARAMETERMODEL_H
