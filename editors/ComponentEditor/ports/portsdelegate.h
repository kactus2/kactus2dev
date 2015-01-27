//-----------------------------------------------------------------------------
// File: portsdelegate.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
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
     *      @param [in] parameterNameCompleter      The completer to use for parameter names in expression editor.
     *      @param [in] parameterFinder             The parameter finder to use for for expression editor.
     *      @param [in] parent                      The parent object.
     */
	PortsDelegate(QCompleter* parameterCompleter, QSharedPointer<ParameterFinder> parameterFinder, 
        QObject *parent = 0);
	
	//! The destructor
	virtual ~PortsDelegate();

	/*! Create a new editor for the given item
	 *
	 * @param [in] parent   Owner for the editor.
	 * @param [in] option   Contains options for the editor.
	 * @param [in] index    Model index identifying the item.
	 *
	 * @return Pointer to the editor to be used to edit the item.
	*/
	virtual QWidget* createEditor(QWidget* parent, QStyleOptionViewItem const& option, 
        QModelIndex const& index) const;

	/*! Set the data for the editor.
	 *
	 * @param [in] editor   Pointer to the editor where the data is to be set.
	 * @param [in] index    Model index identifying the item that's data is to be set.
	 *
	*/
	virtual void setEditorData(QWidget* editor, QModelIndex const& index) const;

	/*! Save the data from the editor to the model.
	 *
	 * @param [in] editor   Pointer to the editor that contains the data to store.
	 * @param [in] model    Model that contains the data structure where data is to be saved to.
	 * @param [in] index    Model index identifying the item that's data is to be saved.
	 *
	*/
	virtual void setModelData(QWidget* editor, QAbstractItemModel* model, 
		QModelIndex const& index) const;

	/*!
	 *  Renders an item in the given index.
	 *
	 *      @param [in] painter     The painter to use for rendering.
	 *      @param [in] option      The style options for the rendering.
	 *      @param [in] index       The index to the data being rendered.
	 */
	virtual void paint(QPainter* painter, QStyleOptionViewItem const& option, QModelIndex const& index) const;

protected:
    
    /*!
     *  Handler for preprocessing events that starts editing.
     *
     *      @param [in] event   The event that triggered the editing.
     *      @param [in] model   The underlying model to edit.
     *      @param [in] option  Style options for rendering the item.
     *      @param [in] index   The index being edited.
     *
     *      @return True, if the delegate handles the event, otherwise false.
     */
    virtual bool editorEvent(QEvent* event, QAbstractItemModel* model, QStyleOptionViewItem const& option, 
        QModelIndex const& index);
    
    /*!
     *  Checks if the given column supports expressions in the editor.
     *
     *      @param [in] column   The column to check.
     *
     *      @return True, if the cells in the column allow expressions, otherwise false.
     */
    virtual bool columnAcceptsExpression(int column) const;

private:
    // Disable copying.
    PortsDelegate(PortsDelegate const& rhs);
    PortsDelegate& operator=(PortsDelegate const& rhs);

    /*!
     *  Creates a selector for port direction.
     *
     *      @param [in] parent   The parent widget for the selector.
     *
     *      @return The selector for port direction.
     */
    QWidget* createSelectorForDirection(QWidget* parent) const;
        
    /*!
     *  Creates a selector for port type with VHDL types as options.
     *
     *      @param [in] parent   The parent widget for the selector.
     *
     *      @return The selector for port type.
     */
    QWidget* createSelectorWithVHDLTypes(QWidget* parent) const;
          
    /*!
     *  Creates a selector for port typedef with VHDL standard libraries as options.
     *
     *      @param [in] parent   The parent widget for the selector.
     *
     *      @return The selector for port typedef.
     */  
    QWidget* createSelectorWithVHDLStandardLibraries(QWidget* parent) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Boolean for ad-hoc group modify.
    bool adhocGroupModify_;

    //! The new state for the group modify.
    Qt::CheckState adhocGroupState_;

};

#endif // PORTSDELEGATE_H
