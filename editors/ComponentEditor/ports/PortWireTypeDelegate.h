//-----------------------------------------------------------------------------
// File: PortWireTypeDelegate.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 24.11.2017
//
// Description:
// The delegate that provides editors to edit port wire type definition.
//-----------------------------------------------------------------------------

#ifndef PORTWIRETYPEDELEGATE_H
#define PORTWIRETYPEDELEGATE_H

#include <QStyledItemDelegate>
#include <QAbstractItemModel>

class Component;
class View;

//-----------------------------------------------------------------------------
//! The delegate that provides editors to edit port wire type definition.
//-----------------------------------------------------------------------------
class PortWireTypeDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] component   Component containing the edited ports.
     *      @param [in] parent      The parent object.
     */
	PortWireTypeDelegate(QSharedPointer<Component> component, QObject* parent = 0);
	
	/*!
     *  The destructor.
     */
    virtual ~PortWireTypeDelegate();

    /*!
     *  Set a new model for types.
     *
     *      @param [in] newModel    The new type model.
     */
    void setNewModel(QAbstractItemModel* newModel);

	/*!
     *  Create a new editor for the given item
	 *
	 *      @param [in] parent  Owner for the editor.
	 *      @param [in] option  Contains options for the editor.
	 *      @param [in] index   Model index identifying the item.
	 *
	 *      @return The editor to be used to edit the item.
     */
	virtual QWidget* createEditor(QWidget* parent, QStyleOptionViewItem const& option, QModelIndex const& index)
        const;

	/*!
     *  Set the data for the editor.
	 *
	 *      @param [in] editor  The editor where the data is to be set.
	 *      @param [in] index   Model index identifying the item that's data is to be set.
	 */
	virtual void setEditorData(QWidget* editor, QModelIndex const& index) const;

	/*!
     *  Save the data from the editor to the model.
	 *
	 *      @param [in] editor  The editor that contains the data to store.
	 *      @param [in] model   Model that contains the data structure where data is to be saved to.
	 *      @param [in] index   Model index identifying the item that's data is to be saved.
	 */
	virtual void setModelData(QWidget* editor, QAbstractItemModel* model, QModelIndex const& index) const;

    /*!
     *  Set a new component.
     *
     *      @param [in] newComponent    The new component.
     */
    void setComponent(QSharedPointer<Component> newComponent);

    /*!
     *  Get the size requirement for the editor.
     *
     *      @param [in] option  Style options for the editor.
     *      @param [in] index   Index of the selected editor.
     */
    virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

    /*!
     *  Paint the indexed area.
     *
     *      @param [in] painter     The painter.
     *      @param [in] option      Style options of the area.
     *      @param [in] index       The selected index.
     */
    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

    /*!
     *  Update the geometry of the editor in the selected index.
     *
     *      @param [in] editor  The contained editor.
     *      @param [in] option  Style options for the area containing the editor.
     *      @param [in] index   The selected index.
     */
    virtual void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
        const QModelIndex &index) const;

signals:

    /*!
     *  Set the new type definitions for the selected type.
     *
     *      @param [in] typeIndex       Index of the selected type.
     *      @param [in] newDefinitions  The new type definitions.
     */
    void setNewDefinitions(QModelIndex const& typeIndex, QStringList newDefinitions);

    /*!
     *  Transmits changes in the content of the port types.
     */
    void contentChanged();

private slots:

    /*!
     *  Handles the closing of the type definition editor.
     *
     *      @param [in] editorIndex     Index of the edited type.
     */
    void onDefinitionEditorClosed(QModelIndex const& editorIndex);

private:

    // No copying. No assignment.
    PortWireTypeDelegate(PortWireTypeDelegate const& rhs);
    PortWireTypeDelegate& operator=(PortWireTypeDelegate const& rhs);

    /*!
     *  Creates a selector for port type with common VHDL and Verilog types as options.
     *
     *      @param [in] parent   The parent widget for the selector.
     *
     *      @return The selector for port type.
     */
    QWidget* createSelectorWithCommonTypes(QWidget* parent) const;
  
    /*!
     *  Creates a selector for view reference with the components views as options.
     *
     *      @param [in] parent   The parent widget for the selector.
     *
     *      @return The selector for view reference.
     */
    QWidget* createViewSelector(QWidget* parent) const;

    /*!
     *  Creates an editor for port typedef with VHDL standard libraries as options.
     *
     *      @param [in] parent  The parent widget for the selector.
     *      @param [in] index   Index of the created definition editor.
     *
     *      @return The editor for port typedef.
     */  
    QWidget* createDefinitionEditor(QWidget* parent, QModelIndex const& index) const;

    /*!
     *  Reposition and resize the selected editor.
     *
     *      @param [in] editor  The selected editor.
     *      @param [in] option  Style options for the area containing the selected editor.
     */
    void repositionAndResizeEditor(QWidget* editor, QStyleOptionViewItem const& option) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! List of all the referable views.
    QSharedPointer<QList<QSharedPointer<View> > > availableViews_;

    //! Model containing the type data.
    QAbstractItemModel* containedModel_;
};

#endif // PORTSDELEGATE_H
