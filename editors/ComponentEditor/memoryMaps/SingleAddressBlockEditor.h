//-----------------------------------------------------------------------------
// File: SingleAddressBlockEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 23.04.2015
//
// Description:
// Editor for editing the details of a single address block.
//-----------------------------------------------------------------------------

#ifndef SINGLEADDRESSBLOCKEDITOR_H
#define SINGLEADDRESSBLOCKEDITOR_H

#include "addressblockeditor.h"

#include <editors/ComponentEditor/itemeditor.h>

#include <common/widgets/nameGroupEditor/namegroupeditor.h>

#include <IPXACTmodels/addressblock.h>
#include <IPXACTmodels/component.h>

#include <QSharedPointer>

class ExpressionFormatter;
class ExpressionEditor;
class ExpressionParser;
class LibraryInterface;
class ParameterFinder;
class UsageComboBox;
class AccessComboBox;
class BooleanComboBox;

//-----------------------------------------------------------------------------
//! Editor for editing the details of a single address block.
//-----------------------------------------------------------------------------
class SingleAddressBlockEditor : public ItemEditor
{
	Q_OBJECT

public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] addressBlock            Pointer to the address block being edited.
	 *      @param [in] component               Pointer to the component being edited.
	 *      @param [in] handler                 Pointer to the instance managing the library.
	 *      @param [in] parameterFinder         Pointer to the parameter finder.
	 *      @param [in] expressionFormatter     Pointer to the expression formatter.
     *      @param [in] expressionFormatter     The expression parser.
	 *      @param [in] parent                  Pointer to the parent of this editor.
	 */
    SingleAddressBlockEditor(QSharedPointer<AddressBlock> addressBlock,
        QSharedPointer<Component> component,
        LibraryInterface* handler,
        QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<ExpressionFormatter> expressionFormatter,
        QSharedPointer<ExpressionParser> expressionParser,
        QWidget* parent = 0);


	//! The destructor
    virtual ~SingleAddressBlockEditor();

    /*!
	 *  Check for the validity of the editor.
	 *
     *      @return True if the editor is in valid state.
	 */
	virtual bool isValid() const;

	/*!
	 *  Reload all the contained editors.
	 */
	virtual void refresh();

signals:

    /*!
     *  Change the value for address unit bits in the model.
     *
     *      @param [in] newAddressUnitBits  The new address unit bits value.
     */
    void addressUnitBitsChanged(int newAddressUnitBits);

protected:

    /*!
	 *  Shows the widget and informs of the required help document.
	 *
	 *      @param [in] event   The show event.
	 */
	virtual void showEvent(QShowEvent* event);

private slots:
    
    /*!
     *  Sets the value in the base address editor.
     */
    void onBaseAddressChanged();

    /*!
     *  Sets the value in the range editor.
     */
    void onRangeChanged();

    /*!
     *  Sets the value in the width editor.
     */
    void onWidthChanged();

    /*!
     *  Sets the value in the usage editor.
     *
     *      @param [in] newUsage    The selected usage.
     */
    void onUsageSelected(QString const& newUsage);

    /*!
     *  Sets the value in the access editor.
     *
     *      @param [in] newAccess   The selected access.
     */
    void onAccessSelected(QString const& newAccess);

    /*!
     *  Sets the value in the volatile editor.
     *
     *      @param [in] newVolatileValue    The selected volatile status.
     */
    void onVolatileSelected(QString const& newVolatileValue);

private:
	
	//! No copying.
    SingleAddressBlockEditor(const SingleAddressBlockEditor& other);

	//! No assignment.
    SingleAddressBlockEditor& operator=(const SingleAddressBlockEditor& other);

    /*!
     *  Sets the layout for the editor.
     */
    void setupLayout();

    /*!
     *  Connects all the signals of the editors.
     */
    void connectSignals() const;

    /*!
     *  Change the status of signal blocking in all the expression editors.
     *
     *      @param [in] blockStatus     The new signal blocking status.
     */
    void changeExpressionEditorSignalBlockStatus(bool blockStatus);

    /*!
     *  Calculates the value of an expression.
     *
     *      @param [in] expression  The expression, whose value is to be calculated.
     *
     *      @return The calculated value of the given expression.
     */
    QString formattedValueFor(QString const& expression) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Editor for containing the name group information of the address block.
    NameGroupEditor nameEditor_;

    //! Editor for containing the registers of the address block.
    AddressBlockEditor* registersEditor_;

    //! Editor for selecting the usage of the address block.
    UsageComboBox* usageEditor_;

    //! Editor for the base address of the address block.
    ExpressionEditor* baseAddressEditor_;

    //! Editor for the range of the address block.
    ExpressionEditor* rangeEditor_;

    //! Editor for the width of the address block-
    ExpressionEditor* widthEditor_;

    //! Editor for selecting the access of the address block.
    AccessComboBox* accessEditor_;

    //! Editor for selecting the volatility of the address block.
    BooleanComboBox* volatileEditor_;

    //! The current address block.
    QSharedPointer<AddressBlock> addressBlock_;

    //! The expression parser.
    QSharedPointer<ExpressionParser> expressionParser_;
};

#endif // SINGLEADDRESSBLOCKEDITOR_H
