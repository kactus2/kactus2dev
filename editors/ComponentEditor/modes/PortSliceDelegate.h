//-----------------------------------------------------------------------------
// File: PortSliceDelegate.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 14.08.2023
//
// Description:
// Delegate that provides widgets for editing port slices in mode.
//-----------------------------------------------------------------------------

#ifndef PORTSLICE_DELEGATE_H
#define PORTSLICE_DELEGATE_H

#include <editors/ComponentEditor/common/ExpressionDelegate.h>
#include <KactusAPI/include/ExpressionFormatter.h>

#include <editors/common/ExpressionSet.h>

#include <QCompleter>

class ExpressionParser;

//-----------------------------------------------------------------------------
//! Delegate that provides widgets for editing remap condition.
//-----------------------------------------------------------------------------
class PortSliceDelegate : public ExpressionDelegate
{
	Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *    @param [in] portNameList            The list of available port names.
     *    @param [in] completionModel         Model containing the completions used in expression editor.
     *    @param [in] expressions             The collection of objects for expression handling.
     *    @param [in] parent                  The parent of the object.
     */
    PortSliceDelegate(QStringList const& portNameList,
        QAbstractItemModel* completionModel,
        ExpressionSet expressions,
        QObject* parent = 0);

	//! The destructor
    virtual ~PortSliceDelegate() = default;

    //! No copying
    PortSliceDelegate(const PortSliceDelegate& other) = delete;

    //! No assignment
    PortSliceDelegate& operator=(const PortSliceDelegate& other) = delete;

	/*! Create a new editor for the given item
	 *
	 *    @param [in] parent   Owner for the editor.
	 *    @param [in] option   Contains options for the editor.
	 *    @param [in] index    Model index identifying the item.
	 *
	 *    @return Pointer to the editor to be used to edit the item.
	*/
	virtual QWidget* createEditor(QWidget* parent, QStyleOptionViewItem const& option, 
        QModelIndex const& index) const final;

	/*! Set the data for the editor.
	 *
	 *    @param [in] editor  Pointer to the editor where the data is to be set.
	 *    @param [in] index   Model index identifying the item that's data is to be set.
	*/
	virtual void setEditorData(QWidget* editor, QModelIndex const& index) const final;

	/*! Save the data from the editor to the model.
	 *
	 *    @param [in] editor   Pointer to the editor that contains the data to store.
	 *    @param [in] model    Model that contains the data structure where data is to be saved to.
	 *    @param [in] index    Model index identifying the item that's data is to be saved.
	 *
	*/
	virtual void setModelData(QWidget* editor, QAbstractItemModel* model, QModelIndex const& index) const final;


    /*!
     *  Set the component ports.
     *
     *    @param [in] componentPorts  All the ports of the component.
     */
    void setComponentPorts(QStringList const& componentPorts);

public slots:
    
    /*!
     *  Set the currently available port names.
     *
     *    @param [in] newPorts    The currently available port names.
     */
    void setNewPortNames(QStringList const& newPorts);

signals:

    //! Emitted when the values of the array change.
    void contentChanged();

protected:

    /*!
     *  Checks if the cells in given column accept expression as an input value.
     *
     *    @param [in] column   The column to check.
     *
     *    @return True, if the cells accept expressions, otherwise false.
     */
    virtual bool columnAcceptsExpression(int column) const;

    //! Gets the description column.
    virtual int descriptionColumn() const;

private:

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The expression formatter used to change parameter uuids in references to parameter names.
    QSharedPointer<ExpressionFormatter> expressionFormatter_;

    //! The expression parser.
    QSharedPointer<ExpressionParser> expressionParser_;

    //! The list of ports usable as remap ports.
    QStringList availablePortNames_;

    //! The list of port names within the component.
    QStringList componentPortNames_;
};

#endif // PORTSLICE_DELEGATE_H
