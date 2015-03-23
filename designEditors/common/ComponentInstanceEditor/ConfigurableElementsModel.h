/* 
 *  	Created on: 12.8.2011
 *      Author: Antti Kamppi
 * 		filename: componentinstancemodel.h
 *		Project: Kactus 2
 */

#ifndef CONFIGURABLEELEMENTSMODEL_H
#define CONFIGURABLEELEMENTSMODEL_H

#include <common/GenericEditProvider.h>
#include <designEditors/HWDesign/HWComponentItem.h>

#include <editors/ComponentEditor/common/ParameterizableTable.h>
#include <editors/ComponentEditor/common/ExpressionFormatter.h>
#include <editors/ComponentEditor/common/ParameterFinder.h>

#include <IPXACTmodels/parameter.h>
#include <IPXACTmodels/modelparameter.h>
#include <IPXACTmodels/choice.h>
#include <IPXACTmodels/validators/ParameterValidator2014.h>

#include <QAbstractTableModel>
#include <QMap>
#include <QList>
#include <QString>
#include <QSharedPointer>

class ComponentItem;
//-----------------------------------------------------------------------------
//! Model class to manage the configurable element values being edited.
//-----------------------------------------------------------------------------
class ConfigurableElementsModel : public ParameterizableTable
{
	Q_OBJECT

public:

	/*!
	 *  [Description].
	 *
	 *      @param [in] parameterFinder         Pointer to the instance for finding parameters.
	 *      @param [in] expressionFormatter     Pointer to the formatter for referencing expressions.
     *      @param [in] expressionParser        Pointer to the expression parser for parsing the expressions.
	 *      @param [in] parent                  Pointer to the owner of this model.
	 */
	ConfigurableElementsModel(QSharedPointer<ParameterFinder> parameterFinder, 
        QSharedPointer<ExpressionFormatter> expressionFormatter,
        QSharedPointer<ExpressionParser> expressionParser,
        QObject *parent);
	
	//! \brief The destructor
	virtual ~ConfigurableElementsModel();

	/*! \brief Set the component being edited.
	 *
	 * \param component Pointer to the component being edited.
	 *
	*/
	void setComponent(ComponentItem* component);

	/*! \brief Get the number of rows to be displayed.
	 *
	 * \param parent Identifies the parent, must always be invalid index.
	 *
	 * \return int - the number of rows to display.
	*/
	virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;

	/*! \brief Get the number of columns to display.
	 *
	 * \param parent Identifies the parent , must always be invalid index.
	 *
	 * \return int - the number of columns to display, always 2 for invalid parents.
	*/
	virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;

	/*! \brief Get that data for specified index in given data role.
	 *
	 * \param index Identifies the object that's data is wanted.
	 * \param role Identifies the type of data being requested.
	 *
	 * \return QVariant contains the requested data.
	*/
	virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

	/*! \brief Get the data for given header in given data role.
	 *
	 * \param section Identifies the header section.
	 * \param orientation Only Qt::Horizontal is supported.
	 * \param role Identifies the type of the data being requested.
	 *
	 * \return QVariant contains the requested data.
	*/
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;

	/*! \brief Set the data for specified index.
	 *
	 * \param index Identifies the object that's data is to be saved.
	 * \param value Contains the data to be saved.
	 * \param role Identifies the type of the data to be saved.
	 *
	 * \return bool True if data was saved successfully.
	*/
	virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole );

	/*! \brief Get info on what operations are possible for specified item.
	 *
	 * \param index Identifies the item that's operations are requested.
	 *
	 * \return Qt::ItemFlags Contains info on which operations are available for given index.
	*/
	virtual Qt::ItemFlags flags(const QModelIndex& index) const;

public slots:

	/*! \brief Remove the specified item from the model.
	 *
	 * \param index Identifies the configurable element to remove.
	 *
	*/
	void onRemove(const QModelIndex& index);

	/*! \brief An item should be removed from the model.
	 * 
	 * \param index Identifies the item that should be removed.
	 *
	*/
	void onRemoveItem(const QModelIndex& index);

	/*! \brief Clear the model.
	 *
	*/
	void clear();

	//! \brief Change the configurable elements of this model.
	void changeElements(const QMap<QString, QString>& confElements);

signals:

	//! \brief Emitted when contents of the model changes.
	void contentChanged();

protected:

    /*!
     *  Check if the column index is valid for containing expressions.
     *
     *      @param [in] index   The index being evaluated.
     *
     *      @return     True, if column can have expressions, false otherwise.
     */
    virtual bool isValidExpressionColumn(QModelIndex const& index) const;

    /*!
     *  Gets the expression for the given index, or plain value if there is no expression.
     *
     *      @param [in] index   The index of target data.
     *
     *      @return     Expression in the given index if there is one, or plain value.
     */
    virtual QVariant expressionOrValueForIndex(QModelIndex const& index) const;

    /*!
     *  Validates the data in a parameter corresponding to a given column.
     *
     *      @param [in] index   The index whose data to validate.
     *
     *      @return True, if the data in the parameter is valid, otherwise false.
     */
    virtual bool validateColumnForParameter(QModelIndex const& index) const;

    /*!
     *  Get all the references to a particular id from the configurable element value on selected row.
     *
     *      @param [in] row         The row of the selected configurable element value.
     *      @param [in] valueID     The id of the parameter being searched for.
     *
     *      @return The amount of references to the selected id.
     */
    virtual int getAllReferencesToIdInItemOnRow(const int& row, QString valueID) const;

private:
	//! No copying
	ConfigurableElementsModel(const ConfigurableElementsModel& other);

	//! No assignment
	ConfigurableElementsModel& operator=(const ConfigurableElementsModel& other);

    /*!
     *  Gets the value for the given index.
     *
     *      @param [in] index   The index of target data.
     *
     *      @return     The data in the given index.
     */
    QVariant valueForIndex(QModelIndex const& index) const;

    /*!
     *  Evaluate the value for the given index.
     *
     *      @param [in] index   The index of the value to be given.
     *      @param [in] value   The stored value of the index.
     *
     *      @return The true value for the given index, whether it is given using a choice or not.
     */
    QString evaluateValueForIndex(QModelIndex const& index, QString const& value) const;

    /*!
     *  Finds the display value for the currently selected enumeration.
     *
     *      @param [in] choice              The currently selected choice.
     *      @param [in] enumerationValue    The current value to be changed into an enumeration.
     *
     *      @return The displayable value of the enumeration.
     */
    QString findDisplayValueForEnumeration(QSharedPointer<Choice> choice, QString const& enumerationValue) const;

    /*!
     *  Finds the display values for the currently selected enumeration inside an array.
     *
     *      @param [in] choice      The currently selected choice.
     *      @param [in] arrayValue  The value that contains an array.
     *
     *      @return The displayable array value of the enumeration.
     */
    QString matchArrayValuesToSelectedChoice(QSharedPointer<Choice> choice, QString const& arrayValue) const;

    /*!
     *  Finds the currently selected choice.
     *
     *      @param [in] choiceName  The name of the choice to be searched for.
     *
     *      @return The currently selected choice.
     */
    QSharedPointer<Choice> findChoice(QString const& choiceName) const;

	//! Save the elements from the table to values_ map.
	void save();

	/*!
	 *  Read the configurable elements from the component and from the saved values.
	 */
	void readValues();

    /*!
     *  Read the configurable elements from the component.
     */
    void readComponentConfigurableElements();

    /*!
     *  Check if the configurable element is editable.
     *
     *      @param [in] parameterIndex  The index of the element.
     *
     *      @return True, if the element can be edited, false otherwise.
     */
    bool isParameterEditable(const int& parameterIndex) const;

    /*!
     *  Set the parameter into the configurable elements.
     *
     *      @param [in] parameterPointer    The pointer to the parameter to be set to configurable elements.
     */
    void setParameterToConfigurableElements(QSharedPointer <Parameter> parameterPointer);

	//! Pointer to the component instance being edited.
	ComponentItem* component_;

	//! Reference to the map containing the actual configurable elements.
	QMap<QString, QString> currentElementValues_;

    //! The list that is used to display the elements in a table form.
    QList <QSharedPointer <Parameter> > configurableElements_;

	//! Pointer to the generic edit provider that manages the undo/redo stack.
	GenericEditProvider* editProvider_;

    //! The formatter for referencing expressions.
    QSharedPointer<ExpressionFormatter> expressionFormatter_;

    //! Validator for parameters.
    ParameterValidator2014* validator_;
};

#endif // CONFIGURABLEELEMENTSMODEL_H
