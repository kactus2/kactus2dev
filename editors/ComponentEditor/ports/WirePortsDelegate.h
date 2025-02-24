//-----------------------------------------------------------------------------
// File: WirePortsDelegate.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 09.05.2019
//
// Description:
// The delegate that provides editors to edit wire ports of a component.
//-----------------------------------------------------------------------------

#ifndef WIREPORTSDELEGATE_H
#define WIREPORTSDELEGATE_H

#include <editors/ComponentEditor/ports/portsdelegate.h>

#include <QCompleter>

class ParameterFinder;
class Component;
class PortTypeValidator;

//-----------------------------------------------------------------------------
//! The delegate that provides editors to edit wire ports of a component.
//-----------------------------------------------------------------------------
class WirePortsDelegate : public PortsDelegate
{
	Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *    @param [in] component           Component containing the edited ports.
     *    @param [in] completionModel     Model containing the completions used in expression editor.
     *    @param [in] parameterFinder     The parameter finder to use for for expression editor.
     *    @param [in] typeValidator       Validator for port type definitions.
     *    @param [in] parent              The parent object.
     */
	WirePortsDelegate(QSharedPointer<Component> component, QAbstractItemModel* completionModel,
        QSharedPointer<ParameterFinder> parameterFinder, QSharedPointer<PortTypeValidator> typeValidator,
        QObject* parent = 0);
	
	/*!
     *  The destructor.
     */
    virtual ~WirePortsDelegate() = default;

	/*!
     *  Create a new editor for the given item.
	 *
	 *    @param [in] parent  Owner for the editor.
	 *    @param [in] option  Contains options for the editor.
	 *    @param [in] index   Model index identifying the item.
	 *
	 *    @return The editor to be used to edit the item.
     */
	virtual QWidget* createEditor(QWidget* parent, QStyleOptionViewItem const& option, QModelIndex const& index)
        const;

	/*!
     *  Set the data for the editor.
	 *
	 *    @param [in] editor  The editor where the data is to be set.
	 *    @param [in] index   Model index identifying the item that's data is to be set.
     */
	virtual void setEditorData(QWidget* editor, QModelIndex const& index) const;

	/*!
     *  Save the data from the editor to the model.
	 *
	 *    @param [in] editor  The editor that contains the data to store.
	 *    @param [in] model   Model that contains the data structure where data is to be saved to.
     *    @param [in] index   sModel index identifying the item that's data is to be saved.
     */
	virtual void setModelData(QWidget* editor, QAbstractItemModel* model, QModelIndex const& index) const;

    // Disable copying.
    WirePortsDelegate(WirePortsDelegate const& rhs) = delete;
    WirePortsDelegate& operator=(WirePortsDelegate const& rhs) = delete;

protected:
    
    /*!
     *  Checks if the given column supports expressions in the editor.
     *
     *    @param [in] column   The column to check.
     *
     *    @return True, if the cells in the column allow expressions, otherwise false.
     */
    virtual bool columnAcceptsExpression(int column) const;

    /*!
     *  Gets the description column.
     *
     *    @return Column number for description.
     */
    virtual int descriptionColumn() const;

private:

    /*!
     *  Gets the name column.
     *
     *    @return Column number for name.
     */
    virtual int nameColumn() const;

    /*!
     *  Gets the type definitions column.
     *
     *    @return Column number for type definitions.
     */
    virtual int typeColumn() const;

    /*!
     *  Gets the tag group column.
     *
     *    @return Column number for tag group.
     */
    virtual int tagColumn() const;

    /*!
     *  Gets the ad hoc column.
     *
     *    @return Column number for ad hoc.
     */
    virtual int adHocColumn() const;

    /*!
     *  Gets the array left column.
     *
     *    @return Column number for left value of the array.
     */
    virtual int arrayLeftColumn() const;

    /*!
     *  Gets the array right column.
     *
     *    @return Column number for right value of the array.
     */
    virtual int arrayRightColumn() const;

    /*!
     *  Creates a selector for port direction.
     *
     *    @param [in] parent   The parent widget for the selector.
     *
     *    @return The selector for port direction.
     */
     QWidget* createSelectorForDirection(QWidget* parent) const;
};

#endif // WIREPORTSDELEGATE_H
