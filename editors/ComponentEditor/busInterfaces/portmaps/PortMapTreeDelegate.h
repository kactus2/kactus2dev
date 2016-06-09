//-----------------------------------------------------------------------------
// File: PortMapTreeDelegate.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 23.05.2016
//
// Description:
// Delegate to provide editors to edit bus interface port maps.
//-----------------------------------------------------------------------------

#ifndef PORTMAPTREEDELEGATE_H
#define PORTMAPTREEDELEGATE_H

#include <editors/ComponentEditor/common/ExpressionDelegate.h>

#include <IPXACTmodels/generaldeclarations.h>

#include <QPainter>
#include <QSharedPointer>
#include <QComboBox>

class Component;
class VLNV;
class LibraryInterface;

//-----------------------------------------------------------------------------
//! Delegate to provide editors to edit bus interface port maps.
//-----------------------------------------------------------------------------
class PortMapTreeDelegate : public ExpressionDelegate
{
	Q_OBJECT

public:

	/*!
     *  The constructor.
	 *
     *      @param [in] component           The component containing the bus interface port maps.
     *      @param [in] parameterCompleter  Completer for the expression editor.
     *      @param [in] finder              The parameter finder.
     *      @param [in] portNames           A list of the port names.
     *      @param [in] libraryHandler      Handler for the VLNV library.
	 *      @param [in] parent              The owner of this delegate.
	 */
	PortMapTreeDelegate(QSharedPointer<Component> component, QCompleter* parameterCompleter,
        QSharedPointer<ParameterFinder> finder, QStringList portNames, LibraryInterface* libraryHandler,
        QObject *parent);
	
	/*!
     *  The destructor.
     */
    ~PortMapTreeDelegate();

    /*!
     *  Update the available physical ports.
     *
     *      @param [in] newPortNames    A list of the new port names.
     */
    void updatePortNames(QStringList newPortNames);

    /*!
     *  Update the available logical ports.
     *
     *      @param [in] vlnv    The VLNV the of the abstraction definition containing the logical ports.
     *      @param [in] mode    The new bus interface mode.
     */
    void updateLogicalPortNames(VLNV const& vlnv, General::InterfaceMode mode);

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
	PortMapTreeDelegate(const PortMapTreeDelegate& other);
	PortMapTreeDelegate& operator=(const PortMapTreeDelegate& other);

    /*!
     *  Set direction icons for the port selector.
     *
     *      @param [in] portSelector    The port selector.
     */
    void setPortDirectionIcons(QComboBox* portSelector) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! List of the available physical port names.
    QStringList physicalPortNames_;

    //! List of the available logical port names.
    QStringList logicalPortNames_;

    //! The component containing the bus interface port maps.
    QSharedPointer<Component> component_;

    //! The library handler.
    LibraryInterface* libraryHandler_;

    //! Invert modify for checkbox editor.
    bool invertModify_;

    //! The new state for invert.
    Qt::CheckState invertCheckState_;
};

#endif // PORTMAPTREEDELEGATE_H
