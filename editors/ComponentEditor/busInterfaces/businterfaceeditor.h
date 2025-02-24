//-----------------------------------------------------------------------------
// File: businterfaceeditor.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 05.04.2011
//
// Description:
// Container for editors to edit a bus interface.
//-----------------------------------------------------------------------------

#ifndef BUSINTERFACEEDITOR_H
#define BUSINTERFACEEDITOR_H

#include <editors/common/ExpressionSet.h>
#include <editors/ComponentEditor/ParameterItemEditor.h>
#include <editors/ComponentEditor/busInterfaces/general/busifgeneraltab.h>
#include <editors/ComponentEditor/busInterfaces/portmaps/BusInterfacePortMapTab.h>

#include <KactusAPI/include/ParameterFinder.h>
#include <KactusAPI/include/ExpressionFormatter.h>
#include <KactusAPI/include/ExpressionParser.h>

#include <IPXACTmodels/Component/validators/BusInterfaceValidator.h>

#include <QSharedPointer>
#include <QTabWidget>

class BusInterface;
class Component;
class LibraryInterface;
class BusInterfaceInterface;
class PortMapInterface;

//-----------------------------------------------------------------------------
//! Container for editors to edit a bus interface.
//-----------------------------------------------------------------------------
class BusInterfaceEditor : public ParameterItemEditor
{
	Q_OBJECT

public:

	/*!
	 *  The constructor.
	 *
	 *    @param [in] libHandler              The library handler instance.
	 *    @param [in] component               The component being edited.
	 *    @param [in] busif                   The bus interface being edited.
	 *    @param [in] parameterFinder         The parameter finder.
	 *    @param [in] expressionFormatter     The expression formatter.
     *    @param [in] expressionParser        The expression parser.
     *    @param [in] busInterface            Interface for accessing bus interfaces.
     *    @param [in] portMapInterface        Interface for accessing port maps.
     *    @param [in] parent                  The owner of this container.
	 *    @param [in] parentWnd               The parent window.
	 */
    BusInterfaceEditor(LibraryInterface* libHandler,
        QSharedPointer<Component> component,
        QSharedPointer<BusInterface> busif,
		ExpressionSet expressions,
        BusInterfaceInterface* busInterface,
        PortMapInterface* portMapInterface,
        QWidget* parent,
        QWidget* parentWnd);

	//! The destructor
	virtual ~BusInterfaceEditor();

	/*! Reload the information from the model to the editor.
	*/
	virtual void refresh();

    /*!
     *  Sets the protection state of the editor.
     *
     *    @param [in] locked  True for locked state; false for unlocked.
     */
    virtual void setProtection(bool locked);

private slots:
	//! When tab page changes
	void onTabChange(int index);

private:
	
	//! No copying
	BusInterfaceEditor(const BusInterfaceEditor& other);

	//! No assignment
	BusInterfaceEditor& operator=(const BusInterfaceEditor& other);
   
	//! The widget that contains the editor for bus interface.
	QTabWidget tabs_;

	//! The tab for general settings of bus interface
	BusIfGeneralTab generalEditor_;

	//! The tab for port maps of bus interface
    BusInterfacePortMapTab portmapsEditor_;
};

#endif // BUSINTERFACEEDITOR_H
