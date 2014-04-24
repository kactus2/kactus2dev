//-----------------------------------------------------------------------------
// File: PropertyValueDelegate.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 16.4.2012
//
// Description:
// Delegate for property values.
//-----------------------------------------------------------------------------

#ifndef PROPERTYVALUEDELEGATE_H
#define PROPERTYVALUEDELEGATE_H

#include <QStyledItemDelegate>
#include <QList>
#include <QSharedPointer>

class ComProperty;

//-----------------------------------------------------------------------------
// Delegate for property values.
//-----------------------------------------------------------------------------
class PropertyValueDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:
	/*!
     *  Constructor.
     *
     *      @param [in] parent The parent object.
     */
	PropertyValueDelegate(QObject* parent = 0);

	/*!
     *  Destructor.
     */
	virtual ~PropertyValueDelegate();

    /*!
     *  Sets the allowed properties.
     *
     *      @param [in] properties The list of allowed properties.
     */
    void setAllowedProperties(QList< QSharedPointer<ComProperty> > const* properties);

	/*!
     *  Creates a new editor for the given item.
	 *
	 *      @param [in] parent  The parent widget.
     *      @param [in] option  Contains options for the editor.
     *      @param [in] index   Model index identifying the item.
	 *
	 *      @return Pointer to the editor to be used to edit the item.
	 */
	virtual QWidget* createEditor(QWidget* parent, QStyleOptionViewItem const& option, 
		                          QModelIndex const& index) const;

	/*!
     *  Sets the data for the editor.
	 *
	 *      @param [in] editor  Pointer to the editor where the data is to be set.
	 *      @param [in] index   Model index identifying the item that's data is to be set.
	 */
	virtual void setEditorData(QWidget* editor, QModelIndex const& index) const;

	/*!
     *  Saves the data from the editor to the model.
	 *
	 *      @param [in] editor  Pointer to the editor that contains the data to store.
	 *      @param [in] model   Model that contains the data structure where data is to be saved to.
	 *      @param [in] index   Model index identifying the item that's data is to be saved.
	 */
	virtual void setModelData(QWidget* editor, QAbstractItemModel* model, 
		                      QModelIndex const& index) const;

private slots:
	/*! 
     *  Commits the data from the sending editor and close the editor.
	 */
	void commitAndCloseEditor();

private:
    // Disable copying.
    PropertyValueDelegate(PropertyValueDelegate const& rhs);
    PropertyValueDelegate& operator=(PropertyValueDelegate const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The list of allowed properties.
    //! The allowed properties.
    QList< QSharedPointer<ComProperty> > const* m_allowedProperties;
};

//-----------------------------------------------------------------------------

#endif // PROPERTYVALUEDELEGATE_H
