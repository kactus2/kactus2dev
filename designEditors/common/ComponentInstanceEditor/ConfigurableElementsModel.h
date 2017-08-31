//-----------------------------------------------------------------------------
// File: ConfigurableElementsModel.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 12.08.2011
//
// Description:
// Model class to manage the configurable element values being edited.
//-----------------------------------------------------------------------------

#ifndef CONFIGURABLEELEMENTSMODEL_H
#define CONFIGURABLEELEMENTSMODEL_H

#include <editors/ComponentEditor/common/ParameterizableTable.h>
#include <editors/ComponentEditor/common/ExpressionFormatter.h>
#include <editors/ComponentEditor/common/ConfigurableElementFinder.h>

#include <designEditors/common/ComponentInstanceEditor/EditorConfigurableElement.h>

#include <IPXACTmodels/common/validators/ParameterValidator2014.h>

#include <QAbstractItemModel>
#include <QList>
#include <QString>
#include <QSharedPointer>

class Parameter;
class Choice;
class ConfigurableElementValue;

//-----------------------------------------------------------------------------
//! Model class to manage the configurable element values being edited.
//-----------------------------------------------------------------------------
class ConfigurableElementsModel : public QAbstractItemModel, public ParameterizableTable
{
	Q_OBJECT

public:

    //! The different user roles for data.
    enum configurableElementsRoles
    {
        deletableElementCheckRole = Qt::UserRole,                   //! Checks if an item can be deleted.
        parameterIDRole = Qt::UserRole + 1,                         //! Gets the ID of the referenced parameter.
        getItemConfigurableElementValuesRole = Qt::UserRole + 2,    //! Gets the stored configurable elements.
        getConfigurableElementsFromTableRole = Qt::UserRole + 3     //! Gets the elements from the table.
    };

	/*!
	 *  The constructor.
	 *
	 *      @param [in] parameterFinder                         Finds parameters.
	 *      @param [in] elementFinder                           Finds configurable elements.
	 *      @param [in] configurableElementExpressionFormatter  Formatter for configurable elements.
	 *      @param [in] defaultValueFormatter                   Formatter for default values.
	 *      @param [in] configurableElementExpressionParser     Parses expressions in configurable element values.
	 *      @param [in] defaultValueParser                      Parses expressions in default values.
	 *      @param [in] parent                                  Pointer to the owner of this model.
	 */
	ConfigurableElementsModel(QSharedPointer<ParameterFinder> parameterFinder, 
        QSharedPointer<ConfigurableElementFinder> elementFinder,
        QSharedPointer<ExpressionFormatter> configurableElementExpressionFormatter,
        QSharedPointer<ExpressionFormatter> defaultValueFormatter,
        QSharedPointer<ExpressionParser> configurableElementExpressionParser,
        QSharedPointer<ExpressionParser> defaultValueParser, QObject *parent);
	
	/*!
	 *  The destructor.
	 */
	virtual ~ConfigurableElementsModel();

	/*!
	 *  Set the parameters for configurable element values.
	 *
     *      @param [in] containingItemName          Name of the item containing the configurable element values.
     *      @param [in] parameters                  The configurable parameters.
     *      @param [in] choices                     List of choices available to the configurable element values.
     *      @param [in] storedConfigurableElements  List of previously set configurable element values.
	 */
    void setParameters(QString const& containingItemName,
        QSharedPointer<QList<QSharedPointer<Parameter> > > parameters,
        QSharedPointer<QList<QSharedPointer<Choice> > > choices,
        QSharedPointer<QList<QSharedPointer<ConfigurableElementValue> > > storedConfigurableElements);

	/*!
	 *  Get the number of rows to be displayed.
	 *
	 *      @param [in] parent  Identifies the parent, must always be invalid index.
     *
     *      @return The number of rows to display.
	 */
	virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;

	/*!
	 *  Get the number of columns to display.
	 *
	 *      @param [in] parent  Identifies the parent, must always be invalid index.
     *
     *      @return The number of columns to display.
	 */
	virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;

	/*!
	 *  Get the data for specified index in given data role.
	 *
	 *      @param [in] index   Identifies the object that's data is wanted.
	 *      @param [in] role    Identifies the type of data being requested.
     *
     *      @return QVariant that contains the requested data.
	 */
	virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

	/*!
	 *  Get the data for the given header in the given data role.
	 *
	 *      @param [in] section         Identifies the header section.
	 *      @param [in] orientation     Only horizontal header is supported.
	 *      @param [in] role            Identifies the type of the data being requested.
     *
     *      @return QVariant that contains the requested data.
	 */
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;

	/*!
	 *  Set the data for the specified index.
	 *
	 *      @param [in] index   Identifies the object which data is to be saved.
	 *      @param [in] value   Contains the data to be saved.
	 *      @param [in] role    Identifies the type of the data to be saved.
     *
     *      @return True if data was saved successfully.
	 */
	virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole );

	/*!
	 *  Get info on what operations are possible for specified item.
	 *
	 *      @param [in] index   Identifies the item that's operations are requested.
     *
     *      @return Flags containing info on which operations are available for given index.
	 */
	virtual Qt::ItemFlags flags(const QModelIndex& index) const;

    /*!
     *  Get the model index of the specified object.
     *
     *      @param [in] row     Row number of the object.
     *      @param [in] column  Column number of the object.
     *      @param [in] parent  Model index of the parent of the object.
     *
     *      @return QModelIndex that identifies the object.
     */
    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;

    /*!
     *  Get the model index of the parent of the object.
     *
     *      @param [in] child   Model index that identifies the child of the object.
     *
     *      @return QModelindex that identifies the parent of the given object.
     */
    QModelIndex parent(const QModelIndex& child) const;

    /*!
     *  Sends a data change signal for the selected configurable element.
     *
     *      @param [in] parameterID     ID of the parameter referenced by the configurable element.
     *      @param [in] newValue        The new value of the configurable element.
     */
    void emitDataChangeForID(QString const& parameterID, QString const& newValue);

public slots:

	/*!
	 *  Clear the model.
	 */
	void clear();

	/*!
	 *  Handles item removal.
	 *
     *      @param [in] elementID   ID of the referenced parameter.
     *      @param [in] elementRow  Index row of the removed element.
	 */
    void onRemoveItem(QString const& elementID, int elementRow);

    /*!
     *  Handles the addition of new configurable elements.
     *
     *      @param [in] elementID       ID of the referenced parameter.
     *      @param [in] elementValue    The new configurable value.
     *      @param [in] elementRow      Index row for the new item.
     */
    void onAddItem(QString const& elementID, QString const& elementValue, int elementRow);

    /*!
     *  Check if the given index is valid.
     *
     *      @param [in] index   The index to be checked.
     *
     *      @return True, if the given index is valid, false otherwise.
     */
    bool isIndexValid(QModelIndex const& index) const;

    /*!
     *  Get the configurable elements from the table.
     *
     *      @return The configurable elements from the table.
     */
    QList<QSharedPointer<EditorConfigurableElement> > getConfigurableElements() const;

signals:

	/*!
     *  Emitted when contents of the model changes.
     */
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
     *  Gets a formatted value for a given expression in default value column.
     *
     *      @param [in] expression  The expression which is being formatted.
     *
     *      @return The formatted value for the expression.
     */
    virtual QString formattedValueFor(QString const& expression) const;

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
    virtual bool validateIndex(QModelIndex const& index) const;

private:
    //! No copying. //! No assignment.
	ConfigurableElementsModel(const ConfigurableElementsModel& other);
	ConfigurableElementsModel& operator=(const ConfigurableElementsModel& other);

    /*!
     *  Check if the configurable element is editable.
     *
     *      @param [in] parameterIndex  The index of the element.
     *
     *      @return True, if the element can be edited, false otherwise.
     */
    bool isParameterEditable(QModelIndex const& index) const;

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
     *      @param [in] column      The column of the required value.
     *      @param [in] choiceName  The name of the choice used in the given value.
     *      @param [in] value       The given value.
     *
     *      @return The true value for the given index, whether it is given using a choice or not.
     */
    QString evaluateValue(int column, QString const& choiceName, QString const& value) const;

    /*!
     *  Finds the currently selected choice.
     *
     *      @param [in] choiceName  The name of the choice to be searched for.
     *
     *      @return The currently selected choice.
     */
    QSharedPointer<Choice> findChoice(QString const& choiceName) const;

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
     *  Finds the display value for the currently selected enumeration.
     *
     *      @param [in] choice              The currently selected choice.
     *      @param [in] enumerationValue    The current value to be changed into an enumeration.
     *
     *      @return The displayable value of the enumeration.
     */
    QString findDisplayValueForEnumeration(QSharedPointer<Choice> choice, QString const& enumerationValue) const;

    /*!
     *  Gets the text for the tooltip related to the data in the given index.
     *
     *      @param [in] index   The index of the target data.
     *
     *      @return The corresponding text for a tool tip.
     */
    QString tooltipForIndex(QModelIndex const& index) const;

    /*!
     *  Restore the previously saved configurable element values to their correct element.
     */
    void restoreStoredConfigurableElements();

    /*!
     *  Get the selected configurable element.
     *
     *      @param [in] elementID   The id of the element to be searched for.
     *
     *      @return The selected configurable element.
     */
    QSharedPointer<EditorConfigurableElement> getStoredConfigurableElement(QString const& elementID);

    /*!
     *  Create an unknown parameter.
     *
     *      @param [in] parameterID     ID of the unknown parameter.
     *
     *      @return The created unknown parameter.
     */
    QSharedPointer<Parameter> createUnknownParameter(QString const& parameterID);

    /*!
     *  Checks if it is possible to delete the element in the given index.
     *
     *      @param [in] index   The given index of the element.
     *
     *      @return True, if the element can be deleted, false otherwise.
     */
    bool isElementDeletable(QModelIndex const& index) const;

    /*!
     *  Get the referenced parameter ID of the selected index.
     *
     *      @param [in] index   The selected index.
     *
     *      @return Parameter ID of the selected index.
     */
    QString parameterIDForIndex(QModelIndex const& index) const;

    /*!
     *  Get the configurable elements contained in the model.
     *
     *      @return The configurable elements contained within the model
     */
    QSharedPointer<QList<QSharedPointer<ConfigurableElementValue> > > getItemConfigurableElementValues() const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! A list of the configurable elements of the model.
    QList<QSharedPointer<EditorConfigurableElement> > configurableElements_;

    //! A list of the available choices.
    QSharedPointer<QList<QSharedPointer<Choice> > > choices_;

    //! Name of the containing item.
    QString containingItemName_;

    //! The formatter for referencing expressions in values.
    QSharedPointer<ExpressionFormatter> configurableElementExpressionFormatter_;

    //! The formatter for referencing expressions in default values.
    QSharedPointer<ExpressionFormatter> defaultValueFormatter_;

    //! The expression parser for default values.
    QSharedPointer<ExpressionParser> defaultValueParser_;

    //! Validator for parameters.
    QSharedPointer<ParameterValidator2014> validator_;

    //! Configurable element values contained within the containing item.
    QSharedPointer<QList<QSharedPointer<ConfigurableElementValue> > > itemConfigurableElementValues_;

    //! Parameters from which the configurable element values are constructed from.
    QSharedPointer<QList<QSharedPointer<Parameter> > > referableParameters_;
};

//! Meta types for QVariants.
Q_DECLARE_METATYPE(QSharedPointer<QList<QSharedPointer<ConfigurableElementValue> > >)
Q_DECLARE_METATYPE(QList<QSharedPointer<EditorConfigurableElement> >)

#endif // CONFIGURABLEELEMENTSMODEL_H
