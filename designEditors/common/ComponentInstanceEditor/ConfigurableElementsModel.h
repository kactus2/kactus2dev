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
#include <editors/ComponentEditor/common/ListParameterFinder.h>

#include <IPXACTmodels/parameter.h>
#include <IPXACTmodels/modelparameter.h>
#include <IPXACTmodels/choice.h>
#include <IPXACTmodels/validators/ParameterValidator2014.h>
#include <IPXACTmodels/designconfiguration.h>

#include <QAbstractTableModel>
#include <QMap>
#include <QList>
#include <QString>
#include <QSharedPointer>

class ComponentItem;
//-----------------------------------------------------------------------------
//! Model class to manage the configurable element values being edited.
//-----------------------------------------------------------------------------
class ConfigurableElementsModel : public QAbstractItemModel, public ParameterizableTable
{
	Q_OBJECT

public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] parameterFinder         Finds parameters in configurable elements and top component.
	 *      @param [in] listParameterFinder     Finds parameters in configurable elements.
	 *      @param [in] expressionFormatter     Formatter for referencing expressions in configurable elements.
	 *      @param [in] instanceFormatter       Formatter for referencing expressions in default value.
	 *      @param [in] expressionParser        Parses expressions in configurable element values.
	 *      @param [in] instanceParser          Parses expressions in default values.
	 *      @param [in] parent                  Pointer to the owner of this model.
	 */
	ConfigurableElementsModel(QSharedPointer<ParameterFinder> parameterFinder, 
        QSharedPointer<ListParameterFinder> listParameterFinder,
        QSharedPointer<ExpressionFormatter> expressionFormatter,
        QSharedPointer<ExpressionFormatter> instanceFormatter,
        QSharedPointer<ExpressionParser> expressionParser,
        QSharedPointer<ExpressionParser> instanceParser,
        QObject *parent);
	
	/*!
	 *  The destructor.
	 */
	virtual ~ConfigurableElementsModel();

	/*!
	 *  Set the component being edited.
	 *
	 *      @param [in] component   Pointer to the component being edited.
	 */
	void setComponent(ComponentItem* component);

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
     *  Set the design configuration model.
     *
     *      @param [in] designConfiguration     The design configuration model to be set.
     */
    void setDesignConfigurationModel(QSharedPointer<DesignConfiguration> designConfiguration);

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

public slots:

	/*!
	 *  Clear the model.
	 */
	void clear();

    /*!
     *  Change the configurable elements of this model.
     *
     *      @param [in] confElements    The new configurable elements.
     */
    void changeElements(const QMap<QString, QString>& confElements);

	/*!
	 *  An item should be removed from the model.
	 *
	 *      @param [in] index   Identifies the item that should be removed.
	 */
	void onRemoveItem(const QModelIndex& index);

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
	//! No copying
	ConfigurableElementsModel(const ConfigurableElementsModel& other);

	//! No assignment
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
     *  Get the configurable element at the given index.
     *
     *      @param [in] parentIndex     The index of the parent of the given object.
     *      @param [in] row             The row number of the configurable element.
     *
     *      @return The correct configurable element.
     */
    QSharedPointer<Parameter> getIndexedConfigurableElement(QModelIndex const& parentIndex, int row) const;

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

	//! Save the elements from the table to values_ map.
	void save();

	/*!
	 *  Read the configurable elements from the component and from the saved values.
	 */
	void setupConfigurableElements();

    /*!
     *  Read the configurable elements from the component.
     */
    void readComponentConfigurableElements();

    /*!
     *  Set the parameter into the configurable elements.
     *
     *      @param [in] parameterPointer    The pointer to the parameter to be set to configurable elements.
     *      @param [in] rootItemName        The root item of the configurable element.
     */
    void addParameterToConfigurableElements(QSharedPointer <Parameter> parameterPointer,
        QString const& rootItemName);

    /*!
     *  Restore the previously saved configurable element values to their correct element.
     */
    void restoreStoredConfigurableElements();

    /*!
     *  Get the configurable element that was saved previously.
     *
     *      @param [in] elementName     The id of the element to be searched for.
     */
    QSharedPointer<Parameter> getStoredConfigurableElement(QString const& elementID);

    /*!
     *  Check if the given index is valid.
     *
     *      @param [in] index   The index to be checked.
     *
     *      @return True, if the given index is valid, false otherwise.
     */
    bool isIndexValid(QModelIndex const& index) const;

    /*!
     *  Checks if it is possible to delete the element in the given index.
     *
     *      @param [in] index   The given index of the element.
     *
     *      @return True, if the element can be deleted, false otherwise.
     */
    bool isElementDeletable(QModelIndex const& index) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

	//! Pointer to the component instance being edited.
	ComponentItem* component_;

	//! Reference to the map containing the actual configurable elements.
	QMap<QString, QString> currentElementValues_;

    //! The list that is used to display the elements in a table form.
    QSharedPointer<QList<QSharedPointer<Parameter> > > configurableElements_;

	//! Pointer to the generic edit provider that manages the undo/redo stack.
	GenericEditProvider* editProvider_;

    //! The formatter for referencing expressions in values.
    QSharedPointer<ExpressionFormatter> configurableElementExpressionFormatter_;

    //! The formatter for referencing expressions in default values.
    QSharedPointer<ExpressionFormatter> componentInstanceExpressionFormatter_;

    //! The expression parser for default values.
    QSharedPointer<ExpressionParser> componentInstanceExpressionParser_;

    //! Validator for parameters.
    ParameterValidator2014* validator_;

    //! The design configuration model used to find the currently active view of the component instance.
    QSharedPointer<DesignConfiguration> designConfiguration_;

    //! The list containing all the root items in the model.
    QList<QString*> rootItems_;
};

#endif // CONFIGURABLEELEMENTSMODEL_H
