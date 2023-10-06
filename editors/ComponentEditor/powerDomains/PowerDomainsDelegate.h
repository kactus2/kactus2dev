//-----------------------------------------------------------------------------
// File: PowerDomainsDelegate.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 5.10.2023
//
// Description:
// The delegate that provides editors to edit PowerDomains of a component.
//-----------------------------------------------------------------------------

#ifndef POWERDOMAINSDELEGATE_H
#define POWERDOMAINSDELEGATE_H

#include <editors/ComponentEditor/common/ExpressionDelegate.h>

#include <IPXACTmodels/Component/PowerDomain.h>


//-----------------------------------------------------------------------------
//! The delegate that provides editors to edit PowerDomains of a component.
//-----------------------------------------------------------------------------
class PowerDomainsDelegate : public ExpressionDelegate
{
	Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] component           Component containing the edited PowerDomains.
     *      @param [in] completionModel     Model containing the completions used in expression editor.
     *      @param [in] parameterFinder     The parameter finder to use for for expression editor.
     *      @param [in] parent              The parent object.
     */
	PowerDomainsDelegate(QSharedPointer<QList<QSharedPointer<PowerDomain> > > powerDomains, 
        QAbstractItemModel* completionModel,
        QSharedPointer<ParameterFinder> parameterFinder,
        QObject* parent = nullptr);
	
	/*!
     *  The destructor.
     */
	virtual ~PowerDomainsDelegate() = default;

    // Disable copying.
    PowerDomainsDelegate(PowerDomainsDelegate const& rhs) = delete;
    PowerDomainsDelegate& operator=(PowerDomainsDelegate const& rhs) = delete;

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
	virtual void setModelData(QWidget* editor, QAbstractItemModel* model, 
		QModelIndex const& index) const;


signals:

    /*!
     *  Transmits changes in the content.
     */
    void contentChanged();

protected:
    

    
    /*!
     *  Checks if the given column supPowerDomains expressions in the editor.
     *
     *      @param [in] column   The column to check.
     *
     *      @return True, if the cells in the column allow expressions, otherwise false.
     */
    virtual bool columnAcceptsExpression(int column) const;

    /*!
     *  Gets the description column.
     *
     *      @return Description column index.
     */
    virtual int descriptionColumn() const;

private:



    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Component containing the PowerDomains.
    QSharedPointer<QList<QSharedPointer<PowerDomain> > > powerDomains_ = nullptr;

};

#endif // POWERDOMAINSDELEGATE_H
