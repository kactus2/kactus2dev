//-----------------------------------------------------------------------------
// File: RemapConditionDelegate.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 09.04.2015
//
// Description:
// Delegate that provides widgets for editing remap condition.
//-----------------------------------------------------------------------------

#ifndef REMAPCONDITIONDELEGATE_H
#define REMAPCONDITIONDELEGATE_H

#include <editors/ComponentEditor/common/ExpressionDelegate.h>
#include <editors/ComponentEditor/common/ExpressionFormatter.h>

#include <QCompleter>

//-----------------------------------------------------------------------------
//! Delegate that provides widgets for editing remap condition.
//-----------------------------------------------------------------------------
class RemapConditionDelegate : public ExpressionDelegate
{
	Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] portNameList            The list of available port names.
     *      @param [in] parameterCompleter      The completer to use for expression editors.
     *      @param [in] finder                  The parameter finder to use for expression editors.
     *      @param [in] expressionFormatter     The expression formatter for changing ids to parameter names.
     *      @param [in] parent                  The parent of the object.
     */
    RemapConditionDelegate(QStringList const& portNameList,
        QCompleter* parameterCompleter,
        QSharedPointer<ParameterFinder> finder,
        QSharedPointer<ExpressionFormatter> expressionFormatter,
        QObject* parent = 0);

	//! The destructor
    virtual ~RemapConditionDelegate();

	/*! Create a new editor for the given item
	 *
	 *      @param [in] parent   Owner for the editor.
	 *      @param [in] option   Contains options for the editor.
	 *      @param [in] index    Model index identifying the item.
	 *
	 *      @return Pointer to the editor to be used to edit the item.
	*/
	virtual QWidget* createEditor(QWidget* parent, QStyleOptionViewItem const& option, 
        QModelIndex const& index) const;

	/*! Set the data for the editor.
	 *
	 *      @param [in] editor  Pointer to the editor where the data is to be set.
	 *      @param [in] index   Model index identifying the item that's data is to be set.
	*/
	virtual void setEditorData(QWidget* editor, QModelIndex const& index) const;

	/*! Save the data from the editor to the model.
	 *
	 *     @param [in] editor   Pointer to the editor that contains the data to store.
	 *     @param [in] model    Model that contains the data structure where data is to be saved to.
	 *     @param [in] index    Model index identifying the item that's data is to be saved.
	 *
	*/
	virtual void setModelData(QWidget* editor, QAbstractItemModel* model, QModelIndex const& index) const;

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
     *  Set the component ports.
     *
     *      @param [in] componentPorts  All the ports of the component.
     */
    void setComponentPorts(QStringList const& componentPorts);

public slots:
    
    /*!
     *  Set the currently available port names.
     *
     *      @param [in] newPorts    The currently available port names.
     */
    void setNewPortNames(QStringList const& newPorts);

signals:

    //! Emitted when the values of the array change.
    void contentChanged();

protected:

    /*!
     *  Checks if the cells in given column accept expression as an input value.
     *
     *      @param [in] column   The column to check.
     *
     *      @return True, if the cells accept expressions, otherwise false.
     */
    virtual bool columnAcceptsExpression(int column) const;

private:

	//! No copying
    RemapConditionDelegate(const RemapConditionDelegate& other);

	//! No assignment
    RemapConditionDelegate& operator=(const RemapConditionDelegate& other);

    /*!
     *  Set the data for the array editor.
     *
     *      @param [in] editor  The editor used to edit the array.
     *      @param [in] index   The index of the current remap port.
     */
    void setArrayEditor(QWidget* editor, QModelIndex const& index) const;

    /*!
     *  Check if the value should be an array.
     *
     *      @param [in] index   The index of the current value.
     *
     *      @return True, if the value is an array, otherwise false.
     */
    bool valueIsArray(QModelIndex const& index) const;

    /*!
     *  Get the width of the remap port.
     *
     *      @param [in] index   The index of the current remap port.
     */
    int getPortWidth(QModelIndex const& index) const;

    /*!
     *  Repositions the editor if there is not enough space under the default position. The editor is then resized
     *  to use the available space.
     *
     *      @param [in] editor  The editor to reposition.
     *      @param [in] option  The style options for the editor.
     *      @param [in] index   The current index.
     */
    void repositionAndResizeEditor(QWidget* editor, QStyleOptionViewItem const& option, QModelIndex const& index)
        const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The expression formatter used to change parameter uuids in references to parameter names.
    QSharedPointer<ExpressionFormatter> expressionFormatter_;

    //! The list of ports usable as remap ports.
    QStringList availablePortNames_;

    //! The list of port names within the component.
    QStringList componentPortNames_;
};

#endif // REMAPCONDITIONDELEGATE_H
