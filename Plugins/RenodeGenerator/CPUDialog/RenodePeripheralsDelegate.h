//-----------------------------------------------------------------------------
// File: RenodePeripheralsDelegate.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 03.03.2023
//
// Description:
// The delegate that provides editors to edit Renode peripheral details.
//-----------------------------------------------------------------------------

#ifndef RENODEPERIPHERALSDELEGATE_H
#define RENODEPERIPHERALSDELEGATE_H

#include <Plugins/RenodeGenerator/CPUDialog/RenodeStructs.h>

#include <QCompleter>
#include <QStyledItemDelegate>

class Component;
class LibraryInterface;

//-----------------------------------------------------------------------------
//! The delegate that provides editors to edit Renode peripheral details.
//-----------------------------------------------------------------------------
class RenodePeripheralsDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:

    /*!
     *  The constructor.
     * 
     *      @param [in] parentWidget    The parent widget.
     */
    RenodePeripheralsDelegate(QWidget* parentWidget);

	/*!
     *  The destructor.
     */
    virtual ~RenodePeripheralsDelegate() = default;

    /*!
     *  Setup the available peripheral templates.
     *	
     *      @param [in] newTemplates    List of the available peripheral templates.
     */
    void setupTemplates(QVector<QSharedPointer<RenodeStructs::peripheralTemplate> > newTemplates);

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
	 *  Renders an item in the given index.
	 *
	 *      @param [in] painter     The painter to use for rendering.
	 *      @param [in] option      The style options for the rendering.
	 *      @param [in] index       The index to the data being rendered.
	 */
	virtual void paint(QPainter* painter, QStyleOptionViewItem const& option, QModelIndex const& index) const;

    // Disable copying.
    RenodePeripheralsDelegate(RenodePeripheralsDelegate const& rhs) = delete;
    RenodePeripheralsDelegate& operator=(RenodePeripheralsDelegate const& rhs) = delete;

public slots:

    /*!
     *  Handle the change in file location.
     *
     *      @param [in] newFolder   The new location.
     */
    void onFolderChanged(QString const& newFolder);

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

private:

    /*!
     *  Handler for editor events in file path editor.
     *	
     *      @param [in] event   The event that triggered the editing.
     *      @param [in] model   The underlying model to edit.
     *      @param [in] option  Style options for rendering the item.
     *      @param [in] index   The index being edited.
     *
     *      @return True, if the event is handled, otherwise false.
     */
    bool handleEditorEventForPath(QEvent* event, QAbstractItemModel* model, QStyleOptionViewItem const& option, QModelIndex const& index);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Boolean for boolean group modify.
    bool booleanModify_ = false;

    //! The new state for the boolean modify.
    Qt::CheckState booleanState_ = Qt::Unchecked;

    //! The current folder location.
    QString generatorTargetFolder_ = QString("");

    //! List of the available peripheral templates.
    QVector<QSharedPointer<RenodeStructs::peripheralTemplate> > pythonTemplates_;

    //! The parent widget.
    QWidget* parentWidget_;
};

#endif // RENODEPERIPHERALSDELEGATE_H
