//-----------------------------------------------------------------------------
// File: SVDCPUDelegate.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 20.05.2021
//
// Description:
// The delegate that provides editors to edit SVD CPU details.
//-----------------------------------------------------------------------------

#ifndef SVDCPUDELEGATE_H
#define SVDCPUDELEGATE_H

#include <QCompleter>

#include <QStyledItemDelegate>

//-----------------------------------------------------------------------------
//! The delegate that provides editors to edit SVD CPU details.
//-----------------------------------------------------------------------------
class SVDCPUDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:

    /*!
     *  The constructor.
     */
	SVDCPUDelegate(QObject* parent = 0);
	
	/*!
     *  The destructor.
     */
    virtual ~SVDCPUDelegate() = default;

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
	 *  Renders an item in the given index.
	 *
	 *    @param [in] painter     The painter to use for rendering.
	 *    @param [in] option      The style options for the rendering.
	 *    @param [in] index       The index to the data being rendered.
	 */
	virtual void paint(QPainter* painter, QStyleOptionViewItem const& option, QModelIndex const& index) const;

    // Disable copying.
    SVDCPUDelegate(SVDCPUDelegate const& rhs) = delete;
    SVDCPUDelegate& operator=(SVDCPUDelegate const& rhs) = delete;

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

private:

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Boolean for boolean group modify.
    bool booleanModify_;

    //! The new state for the boolean modify.
    Qt::CheckState booleanState_;
};

#endif // SVDCPUDELEGATE_H
