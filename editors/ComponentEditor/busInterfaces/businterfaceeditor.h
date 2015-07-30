/* 
 *
 *  Created on: 5.4.2011
 *      Author: Antti Kamppi
 * 		filename: businterfaceeditor.h
 */

#ifndef BUSINTERFACEEDITOR_H
#define BUSINTERFACEEDITOR_H

#include "busifgeneraltab.h"
#include "busifportmaptab.h"

#include <IPXACTmodels/businterface.h>
#include <IPXACTmodels/component.h>

#include <editors/ComponentEditor/itemeditor.h>
#include <editors/ComponentEditor/common/ParameterFinder.h>
#include <editors/ComponentEditor/common/ExpressionFormatter.h>
#include <editors/ComponentEditor/common/ExpressionParser.h>

#include <QSharedPointer>
#include <QTabWidget>

class LibraryInterface;

/*! \brief Container for editors to edit a bus interface.
 *
 */
class BusInterfaceEditor : public ItemEditor {
	Q_OBJECT

public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] libHandler              Pointer to the library handler instance.
	 *      @param [in] component               Pointer to the component being edited.
	 *      @param [in] busif                   Pointer to the bus interface being edited.
	 *      @param [in] parameterFinder         Pointer to the parameter finder.
	 *      @param [in] expressionFormatter     Pointer to the expression formatter.
     *      @param [in] expressionParser        Pointer to the expression parser.
	 *      @param [in] parent                  Pointer to the owner of this container.
	 *      @param [in] parentWnd               Pointer to the parent window.
	 */
	BusInterfaceEditor(LibraryInterface* libHandler,
		QSharedPointer<Component> component, 
		QSharedPointer<BusInterface> busif,
        QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<ExpressionFormatter> expressionFormatter,
        QSharedPointer<ExpressionParser> expressionParser,
		QWidget* parent,
        QWidget* parentWnd);
	
	//! \brief The destructor
	virtual ~BusInterfaceEditor();

	/*! \brief Check for the validity of the edited item.
	*
	* \return True if item is valid.
	*/
	virtual bool isValid() const;

	/*! \brief Reload the information from the model to the editor.
	*/
	virtual void refresh();

    /*!
     *  Sets the protection state of the editor.
     *
     *      @param [in] locked  True for locked state; false for unlocked.
     */
    virtual void setProtection(bool locked);

private slots:
	//! \brief When tab page changes
	void onTabChange(int index);

private:
	
	//! \brief No copying
	BusInterfaceEditor(const BusInterfaceEditor& other);

	//! No assignment
	BusInterfaceEditor& operator=(const BusInterfaceEditor& other);
   
	//! \brief Pointer to the bus interface being edited.
	QSharedPointer<BusInterface> busif_;

	//! \brief The widget that contains the editor for bus interface.
	QTabWidget tabs_;

	//! \brief The tab for general settings of bus interface
	BusIfGeneralTab general_;

	//! \brief The tab for port maps of bus interface
	BusIfPortmapTab portmaps_;
};

#endif // BUSINTERFACEEDITOR_H
