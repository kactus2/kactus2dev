//-----------------------------------------------------------------------------
// File: PortMapDelegate.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 01.06.2022
//
// Description:
// Delegate to provide editors to edit bus interface port maps.
//-----------------------------------------------------------------------------

#ifndef PORTMAPDELEGATE_H
#define PORTMAPDELEGATE_H

#include <editors/ComponentEditor/common/ExpressionDelegate.h>

#include <IPXACTmodels/generaldeclarations.h>

#include <QPainter>
#include <QComboBox>

class PortMapInterface;

//-----------------------------------------------------------------------------
//! Delegate to provide editors to edit bus interface port maps.
//-----------------------------------------------------------------------------
class PortMapDelegate : public ExpressionDelegate
{
	Q_OBJECT

public:

	/*!
     *  The constructor.
	 *
     *      @param [in] parameterCompleter  Completer for the expression editor.
     *      @param [in] finder              The parameter finder.
     *      @param [in] portMapInterface    Interface for accessing port maps.
	 *      @param [in] parent              The owner of this delegate.
	 */
	PortMapDelegate(QCompleter* parameterCompleter,
        QSharedPointer<ParameterFinder> finder,
        PortMapInterface* portMapInterface,
        QObject *parent);
	
	/*!
     *  The destructor.
     */
    virtual ~PortMapDelegate() = default;

    /*!
     *  Set a new bus interface mode.
     *
     *      @param [in] newMode     The new bus interface mode.
     */
    void setBusMode(General::InterfaceMode newMode);

    /*!
     *  Set a new system group.
     *
     *      @param [in] newSystemGroup  The new system group.
     */
    void setSystemGroup(QString const& newSystemGroup);

    /*!
	 *  Create a new editor for the given item.
	 *
	 *      @param [in] parent  Owner for the editor.
	 *      @param [in] option  Contains options for the editor.
	 *      @param [in] index   Model index identifying the item.
     *
     *      @return Pointer to the editor to be used to edit the item.
	 */
	virtual QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index)
        const;

    /*!
	 *  Set the data for the editor.
	 *
	 *      @param [in] editor  Pointer to the editor where the data is to be set.
	 *      @param [in] index   The index of the item that's data is to be set.
	 */
	virtual void setEditorData(QWidget* editor, const QModelIndex& index) const;

    /*!
	 *  Save the data from the editor to the model.
	 *
	 *      @param [in] editor  Pointer to the editor that contains the data to store.
	 *      @param [in] model   Model that contains the data structure where data is to be saved to.
	 *      @param [in] index   The index of the item that's data is to be saved.
	 */
	virtual void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const;

    /*!
     *  Paint the memory maps.
     *
     *      @param [in] painter     The used painter.
     *      @param [in] option      Available style options.
     *      @param [in] index       The current index.
     */
    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    
    /*!
     *  Updates the editor geometry.
     *
     *      @param [in] editor  The editor being updated.
     *      @param [in] option  The options used to update the editor.
     *      @param [in] index   The model index being edited.
     */
    virtual void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
        const QModelIndex &index) const;

protected:

    /*!
     *  Column for the description.
     *
     *      @return -1. Port maps do not have a description column.
     */
    virtual int descriptionColumn() const;

    /*!
     *  Check if the selected column accepts expressions.
     *
     *      @param [in] column  The selected column.
     *
     *      @return True, if the column accepts expressions, otherwise false.
     */
    virtual bool columnAcceptsExpression(int column) const;

    /*!
     *  The editor events.
     *
     *      @param [in] event   The event itself.
     *      @param [in] model   The used item model.
     *      @param [in] option  The style options.
     *      @param [in] index   The current model index.
     */
    virtual bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option,
        const QModelIndex &index);

private:

	//! No copying.
	PortMapDelegate(const PortMapDelegate& other);
    PortMapDelegate& operator=(const PortMapDelegate& other);

    /*!
     *  Set direction icons for the physical port selector.
     *
     *      @param [in] portSelector    The port selector.
     */
    void setPhysicalPortDirectionIcons(QStringList const& physicalPortNames, QComboBox* portSelector) const;

    /*!
     *  Set direction icons for the logical port selector.
     *
     *      @param [in] portSelector    The port selector.
     */
    void setLogicalPortDirectionIcons(QStringList const& physicalPortNames, QComboBox* portSelector) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Invert modify for checkbox editor.
    bool invertModify_;

    //! The new state for invert.
    Qt::CheckState invertCheckState_;

    //! Interface for accessing port maps.
    PortMapInterface* portMapInterface_;

    //! The available bus interface mode.
    General::InterfaceMode busMode_;
    
    //! The available system group.
    QString systemGroup_;
};

#endif // PORTMAPDELEGATE_H
