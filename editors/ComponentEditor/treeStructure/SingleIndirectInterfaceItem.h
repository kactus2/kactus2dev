
#ifndef SingleIndirectInterfaceItem_H
#define SingleIndirectInterfaceItem_H

#include "componenteditoritem.h"

#include <QSharedPointer>

class IndirectInterface;
class IndirectInterfaceValidator;
class ExpressionParser;

//-----------------------------------------------------------------------------
//! The item for single bus interface in the component editor's navigation tree.
//-----------------------------------------------------------------------------
class SingleIndirectInterfaceItem : public ComponentEditorItem
{
	Q_OBJECT

public:

    /*!
	 *  The constructor.
	 *
	 *      @param [in] busif                   The bus interface being edited.
	 *      @param [in] model                   The model that owns the items.
	 *      @param [in] libHandler              The instance that manages the library.
	 *      @param [in] component               The component being edited.
	 *      @param [in] referenceCounter        The reference counter.
	 *      @param [in] parameterFinder         The parameter finder.
	 *      @param [in] expressionFormatter     The expression formatter.
	 *      @param [in] expressionParser        The expression parser.
     *      @param [in] validator               The validator for bus interfaces.
	 *      @param [in] parent                  The owner of this item.
	 *      @param [in] parentWnd               The parent window.
	 */
	SingleIndirectInterfaceItem(QSharedPointer<IndirectInterface> busif,
		ComponentEditorTreeModel* model,
		LibraryInterface* libHandler,
		QSharedPointer<Component> component,
        QSharedPointer<ReferenceCounter> referenceCounter,
        QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<ExpressionFormatter> expressionFormatter,
        QSharedPointer<ExpressionParser> expressionParser,
        QSharedPointer<IndirectInterfaceValidator> validator,
		ComponentEditorItem* parent,
        QWidget* parentWnd);

	//! The destructor
	virtual ~SingleIndirectInterfaceItem();

	/*! Get the tool tip for the item.
	 * 
	 *      @return The text for the tool tip to print to user.
	*/
	virtual QString getTooltip() const;

	/*! Get the text to be displayed to user in the tree for this item.
	 *
	 *      @return QString Contains the text to display.
	*/
	virtual QString text() const;

	/*! Check the validity of this item and sub items.
	 *
	 *      @return bool True if item is in valid state.
	*/
	virtual bool isValid() const;

	/*! Get The editor of this item.
	 *
	 *      @return The editor to use for this item.
	*/
	virtual ItemEditor* editor();

private:
	//! No copying
	SingleIndirectInterfaceItem(const SingleIndirectInterfaceItem& other);

	//! No assignment
	SingleIndirectInterfaceItem& operator=(const SingleIndirectInterfaceItem& other);

	//! The bus interface being edited.
	QSharedPointer<IndirectInterface> indirectInterface_;

	//! The parent window.
	QWidget* parentWnd_;

    //! The expression parse used to form the results of the expressions.
    QSharedPointer<ExpressionParser> expressionParser_;

    //! The validator for indirect interfaces.
    QSharedPointer<IndirectInterfaceValidator> validator_;
};

#endif // SingleIndirectInterfaceItem_H
