//-----------------------------------------------------------------------------
// File: portsdelegate.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 01.04.2011
//
// Description:
// The delegate that provides editors to edit ports of a component.
//-----------------------------------------------------------------------------

#ifndef PORTSDELEGATE_H
#define PORTSDELEGATE_H

#include <editors/ComponentEditor/common/ExpressionDelegate.h>

#include <QCompleter>

class ParameterFinder;
class Component;

class PortTypeValidator;

//-----------------------------------------------------------------------------
//! The delegate that provides editors to edit ports of a component.
//-----------------------------------------------------------------------------
class PortsDelegate : public ExpressionDelegate
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
	PortsDelegate(QSharedPointer<Component> component, QAbstractItemModel* completionModel,
        QSharedPointer<ParameterFinder> parameterFinder, QSharedPointer<PortTypeValidator> typeValidator,
        QObject* parent = 0);
	
	/*!
     *  The destructor.
     */
	virtual ~PortsDelegate() = default;

	/*!
     *  Create a new editor for the given item
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
	 *    @param [in] index   Model index identifying the item that's data is to be saved.
     */
	virtual void setModelData(QWidget* editor, QAbstractItemModel* model, 
		QModelIndex const& index) const;

	/*!
	 *  Renders an item in the given index.
	 *
	 *    @param [in] painter     The painter to use for rendering.
	 *    @param [in] option      The style options for the rendering.
	 *    @param [in] index       The index to the data being rendered.
	 */
	virtual void paint(QPainter* painter, QStyleOptionViewItem const& option, QModelIndex const& index) const;
    
    /*!
     *  Updates the editor geometry.
     *
     *    @param [in] editor  The editor being updated.
     *    @param [in] option  The options used to update the editor.
     *    @param [in] index   The model index being edited.
     */
    virtual void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
        const QModelIndex &index) const;

    /*!
     *  Set a new component.
     *
     *    @param [in] newComponent    The new component.
     */
    void setComponent(QSharedPointer<Component> newComponent);

    // Disable copying.
    PortsDelegate(PortsDelegate const& rhs) = delete;
    PortsDelegate& operator=(PortsDelegate const& rhs) = delete;

signals:

    /*!
     *  Transmits changes in the content.
     */
    void contentChanged();

protected:
    
    /*!
     *  Handler for preprocessing events that starts editing.
     *
     *    @param [in] event   The event that triggered the editing.
     *    @param [in] model   The underlying model to edit.
     *    @param [in] option  Style options for rendering the item.
     *    @param [in] index   The index being edited.
     *
     *    @return True, if the delegate handles the event, otherwise false.
     */
    virtual bool editorEvent(QEvent* event, QAbstractItemModel* model, QStyleOptionViewItem const& option, 
        QModelIndex const& index);
    
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
     *    @return Description column index.
     */
    virtual int descriptionColumn() const = 0;

private:

    /*!
     *  Get the name column index.
     *
     *    @return Name column index.
     */
    virtual int nameColumn() const = 0;

    /*!
     *  Get the type definition column index.
     *
     *    @return Type definition column index.
     */
    virtual int typeColumn() const = 0;

    /*!
     *  Get the tag group column index.
     *
     *    @return Tag group column index.
     */
    virtual int tagColumn() const = 0;

    /*!
     *  Get the adhoc column index.
     *
     *    @return Adhoc column index.
     */
    virtual int adHocColumn() const = 0;

    /*!
     *  Get the array left column index.
     *
     *    @return Array left column index.
     */
    virtual int arrayLeftColumn() const = 0;

    /*!
     *  Get the array right column index.
     *
     *    @return Array right column index.
     */
    virtual int arrayRightColumn() const = 0;

    /*!
     *  Creates a port type editor.
     *
     *    @param [in] parent  Parent widget for the editor.
     *
     *    @return The editor for port types.
     */
    QWidget* createTypeEditor(QWidget* parent) const;

    /*!
     *  Create a list editor used in defining tags for the ports.
     *
     *    @param [in] currentIndex    The currently selected index.
     *    @param [in] parent          The owner of the new editor.
     *
     *    @return The list editor for port tags.
     */
    QWidget* createListEditorForPortTags(const QModelIndex& currentIndex, QWidget* parent) const;

    /*!
     *  Reposition and resize the selected editor.
     *
     *    @param [in] editor  The selected editor.
     *    @param [in] option  Style options for the area containing the editor.
     *    @param [in] index   Index of the editor.
     */
    void repositionAndResizeEditor(QWidget* editor, QStyleOptionViewItem const& option, QModelIndex const& index)
        const;

    /*!
     *  Get the row count for the port type editor.
     *
     *    @param [in] index   Index of the selected port.
     */
    int getRowCountForPortTypes(QModelIndex const& index) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Component containing the ports.
    QSharedPointer<Component> component_;

    //! Boolean for ad-hoc group modify.
    bool adhocGroupModify_;

    //! The new state for the group modify.
    Qt::CheckState adhocGroupState_;

    //! Validator for the port type definitions.
    QSharedPointer<PortTypeValidator> typeValidator_;
};

#endif // PORTSDELEGATE_H
