//-----------------------------------------------------------------------------
// File: configurableelementdelegate.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 12.08.2011
//
// Description:
// Delegate that provides editors to edit the configurable element values.
//-----------------------------------------------------------------------------

#ifndef CONFIGURABLEELEMENTDELEGATE_H
#define CONFIGURABLEELEMENTDELEGATE_H

#include <common/IEditProvider.h>

#include <editors/ComponentEditor/parameters/ChoiceCreatorDelegate.h>
#include <KactusAPI/include/ParameterFinder.h>
#include <KactusAPI/include/ExpressionFormatter.h>

#include <QWidget>
#include <QSize>
#include <QModelIndex>

class Choice;
class ConfigurableElementValue;
class ConfigurableElementRemoveCommand;

//-----------------------------------------------------------------------------
//! Delegate that provides editors to edit the configurable element values.
//-----------------------------------------------------------------------------
class ConfigurableElementDelegate : public ChoiceCreatorDelegate
{
	Q_OBJECT

public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] parameterCompleter      Pointer to the completer, used in expression editor.
	 *      @param [in] parameterFinder         Pointer to the parameter finder, used in expression editor.
     *      @param [in] expressionFormatter     Expression formatter, used to change parameter ids to names.
	 *      @param [in] parent                  Pointer to the owner of this delegate.
	 */
	ConfigurableElementDelegate(QCompleter* parameterCompleter, QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<ExpressionFormatter> expressionFormatter, QObject *parent);
	
	/*!
     *  The destructor.
     */
	virtual ~ConfigurableElementDelegate();

    /*!
     *  Set a new edit provider.
     *
     *      @param [in] newEditProvider     The new edit provider.
     */
    void setEditProvider(QSharedPointer<IEditProvider> newEditProvider);

    /*!
     *  Create a new editor for the given item.
     *
     *      @param [in] parent  Owner of the editor.
     *      @param [in] option  Contains options for the editor.
     *      @param [in] index   Model index identifying the item.
     *
     *      @return Pointer to the editor to be used to edit the item.
     */
    virtual QWidget* createEditor(QWidget* parent, QStyleOptionViewItem const& option, QModelIndex const& index)
        const;

    /*!
     *  Set the data for the editor.
     *
     *      @param [in] editor  Pointer to the editor where the data is to be set.
     *      @param [in] index   Model index identifying the item that's data is to be set.
     */
    virtual void setEditorData(QWidget* editor, QModelIndex const& index) const;

    /*!
     *  Save the data from the editor to the model.
     *
     *      @param [in] editor  Pointer to the editor that contains the data to store.
     *      @param [in] model   Model that contains the data structure where data is to be saved to.
     *      @param [in] index   Model index identifying the item that's data is to be saved.
     */
    virtual void setModelData(QWidget* editor, QAbstractItemModel* model, QModelIndex const& index) const;

    /*!
     *  Set the choices for this delegate.
     *
     *      @param [in] choices     The choices of the current component.
     */
    void setChoices(QSharedPointer<QList<QSharedPointer<Choice> > > choices);

    /*!
     *  Updates the editor geometry.
     *
     *      @param [in] editor  The editor being updated.
     *      @param [in] option  The options used to update the editor.
     *      @param [in] index   The model index being edited.
     */
    virtual void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
        const QModelIndex &index) const;

    /*!
     *  Paint the delegate.
     *
     *      @param [in] painter     The painter used.
     *      @param [in] option      The style option in use.
     *      @param [in] index       The current index of the model.
     */
    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

    /*!
     *  Returns the size needed by the delegate to display the item in index.
     *
     *      @param [in] option  The style information.
     *      @param [in] index   The index of the item.
     *
     *      @return The size needed by the delegate to display the item in index.
     */
    virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

public slots:

    /*!
     *  Handles the creation of a configurable element remove command.
     *
     *      @param [in] indexes     Indexes of the selected configurable elements.
     */
    void onCreateRemoveElementCommands(QModelIndexList const& indexes);

    /*!
     *  Create remove commands for the sub items of the selected index.
     *
     *      @param [in] indexes     Indexes of the selected configurable elements.
     */
    void onCreateMultipleElementRemoveCommands(QModelIndexList const& indexes);

    /*!
     *  Create an undo command for configurable element value change.
     *
     *      @param [in] oldValue    Old value of the configurable element.
     *      @param [in] newValue    New value of the configurable element.
     *      @param [in] index       Index of the changed configurable element.
     *      @param [in] cevModel    Model containing the configurable element.
     */
    void createElementChangeCommand(QString const& oldValue, QString const& newValue, QModelIndex const& index,
        QAbstractItemModel* cevModel) const;

signals:

    /*!
     *  Informs the model to emit a data change signal for the selected configurable element.
     *
     *      @param [in] parameterID     ID of the parameter referenced by the selected configurable element.
     *      @param [in] newDataValue    The new configurable element value.
     */
    void dataChangedInID(QString const& parameterID, QString const& newDataValue);

    /*!
     *  Informs the model that a new configurable element should be added.
     *
     *      @param [in] elementID       ID of the parameter referenced by the new configurable element.
     *      @param [in] elementValue    New value for the configurable element.
     *      @param [in] parentName      Name of the parent item of the configurable element.
     *      @param [in] elementRow      Index row of the new configurable element.
     */
    void addConfigurableElement(QString const& elementID, QString const& elementValue, QString const& parentName,
        int elementRow);

    /*!
     *  Informs the model that a configurable element should be removed.
     *
     *      @param [in] elementID   ID of the parameter referenced by the removed configurable element.
     *      @param [in] parentName  Name of the parent item of the selected configurable element.
     *      @param [in] elementRow  Index row of the selected configurable element.
     */
    void removeConfigurableElement(QString const& elementID, QString const& parentName, int elementRow);

protected:

    /*!
     *  Checks if the given column supports expressions in the editor.
     *
     *      @param [in] column      The column to check.
     *
     *      @return True, if the cells in the column accept expressions, otherwise false.
     */
    bool columnAcceptsExpression(int column) const;

    /*!
     *  Gets the name of the choice on the currently selected row.
     *
     *      @param [in] index   The index of the currently selected row.
     *
     *      @return The name of the choice used in the row on the given index.
     */
    virtual QString choiceNameOnRow(QModelIndex const& index) const;

    /*!
     *  Gets the column number of the value column.
     *
     *      @return The column number of the value column.
     */
    virtual int valueColumn() const;
    
    //! Gets the description column.
    virtual int descriptionColumn() const;

private slots:

    /*!
     *  Increase the amount of references to parameters made in the selected value.
     *
     *      @param [in] newValue    The selected value.
     */
    void increaseReferencesInNewValue(QString const& newValue);

    /*!
     *  Decrease the amount of references to parameters made in the selected value.
     *
     *      @param [in] oldValue    The selected value.
     */
    void decreaseReferencesInOldValue(QString const& oldValue);

private:

	//! No copying. No assignment.
	ConfigurableElementDelegate(const ConfigurableElementDelegate& other);
	ConfigurableElementDelegate& operator=(const ConfigurableElementDelegate& other);

    /*!
     *  Check if the value at the given index is an array.
     *
     *      @param [in] index   The index of the current value.
     *
     *      @return True, if the value is an array, false otherwise.
     */
    bool valueIsArray(QModelIndex const& index) const;

    /*!
     *  Create the editor for the array value.
     *
     *      @param [in] editor  The editor to be changed to array editor.
     *      @param [in] index   The index of the current value.
     */
    void createArrayEditor(QWidget* editor, QModelIndex const& index) const;

    /*!
     *  Get the size of the array.
     *
     *      @param [in] arrayLeft   The left side of the array.
     *      @param [in] arrayRight  The right side of the array.
     *
     *      @return The size of the array.
     */
    int getArraySize(int arrayLeft, int arrayRight) const;

    /*!
     *  Repositions the editor to better use the available space. The editor is then resized to best cover the
     *  usable area.
     *
     *      @param [in] editor  The editor to reposition.
     *      @param [in] option  The style options for the editor.
     *      @param [in] index   The current index.
     */
    void repositionAndResizeEditor(QWidget* editor, QStyleOptionViewItem const& option, QModelIndex const& index)
        const;

    /*!
     *  Create an element remove command for the selected configurable element.
     *
     *      @param [in] index           Index of the selected configurable element.
     *      @param [in] parentCommand   The parent command.
     *
     *      @return Pointer to the created configurable element remove command.
     */
    ConfigurableElementRemoveCommand* createElementRemoveCommand(QModelIndex const& index,
        QSharedPointer<QUndoCommand> parentCommand);

    /*!
     *  Get the filtered index row of the selected index.
     *
     *      @param [in] index   The selected index.
     *
     *      @return The filtered index row of the selected index.
     */
    int getFilteredIndexRow(QModelIndex const& index) const;

    /*!
     *  Get the name of the parent item of the selected configurable element.
     *
     *      @param [in] parentIndex     Parent index of the selected configurable element.
     *
     *      @return Name of the parent item of the selected configurable element.
     */
    QString getIndexedParentName(QModelIndex const& parentIndex) const;

    /*!
     *  Get a list of configurable element values contained within the item of the selected index.
     *
     *      @param [in] index   The selected index.
     *
     *      @return A list of configurable element values.
     */
    QSharedPointer<QList<QSharedPointer<ConfigurableElementValue> > > getIndexedConfigurableElements(
        QModelIndex const& index) const;

    /*!
     *  Get the ID of the parameter referenced in the indexed item.
     *
     *      @param [in] index   The selected index.
     *
     *      @return ID of the referenced parameter.
     */
    QString getIndexedElementID(QModelIndex const& index) const;

    /*!
     *  Connect the selected configurable element remove command.
     *
     *      @param [in] removeCommand   The selected configurable element remove command.
     */
    void connectElementRemoveCommand(ConfigurableElementRemoveCommand* removeCommand);

    /*!
     *  Add the selected command to the command stack and redo the command.
     *
     *      @param [in] newCommand  The selected command.
     */
    void addCommandToStackAndRedo(QSharedPointer<QUndoCommand> newCommand) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Expression formatter, used in expressions to change parameter ids into names.
    QSharedPointer<ExpressionFormatter> expressionFormatter_;

    //! Edit provider for stacking undo commands.
    QSharedPointer<IEditProvider> editProvider_;
};

#endif // CONFIGURABLEELEMENTDELEGATE_H
