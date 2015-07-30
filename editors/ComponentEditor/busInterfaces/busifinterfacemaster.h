//-----------------------------------------------------------------------------
// File: busifinterfacemaster.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 7.4.2011
//
// Description:
// Editor to edit master or mirrored master settings of a bus interface.
//-----------------------------------------------------------------------------

#ifndef BUSIFINTERFACEMASTER_H
#define BUSIFINTERFACEMASTER_H

#include "busifinterfacemodeeditor.h"

#include <editors/ComponentEditor/common/ReferenceSelector/ReferenceSelector.h>

#include <IPXACTmodels/generaldeclarations.h>
#include <IPXACTmodels/component.h>
#include <IPXACTmodels/businterface.h>

#include <QString>
#include <QSharedPointer>
#include <QLineEdit>

class ExpressionEditor;
class ExpressionParser;
class ParameterFinder;

//-----------------------------------------------------------------------------
//! Editor to edit master or mirrored master settings of a bus interface.
//-----------------------------------------------------------------------------
class BusIfInterfaceMaster : public BusIfInterfaceModeEditor {
	Q_OBJECT

public:

    /*!
	 *  The constructor.
	 *
	 *      @param [in] mode                The mode of the bus interface (master / mirrored master).
	 *      @param [in] busif               Pointer to the bus interface being edited.
	 *      @param [in] component           Pointer to the component being edited.
	 *      @param [in] parameterFinder     Pointer to the parameter finder.
	 *      @param [in] expressionParser    Pointer to the expression parser.
	 *      @param [in] parent              Pointer to the owner of this editor.
	 */
	BusIfInterfaceMaster(General::InterfaceMode mode, 
		QSharedPointer<BusInterface> busif,
		QSharedPointer<Component> component,
        QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<ExpressionParser> expressionParser,
		QWidget *parent);
	
	//! The destructor
	virtual ~BusIfInterfaceMaster();

	/*! Check for the validity of the edited item.
	*
	*       @return True if item is valid.
	*/
	virtual bool isValid() const;

	/*! Restore the changes made in the editor back to ones in the model.
	*
	*/
	virtual void refresh();

	/*! Get the interface mode of the editor
	 * 
	 *      @return General::InterfaceMode Specifies the interface mode.
	*/
	virtual General::InterfaceMode getInterfaceMode() const;

	//! Save the interface mode-specific details to the bus interface.
	virtual void saveModeSpecific();

    /*!
     *  Remove the references from the expressions.
     */
    void removeReferencesFromExpressions();

signals:

    /*!
     *  Increase the number of references to the given parameter.
     *
     *      @param [in] id  The id of the target parameter.
     */
    void increaseReferences(QString id);

    /*!
     *  Decrease the number of references to the given parameter.
     *
     *      @param [in] id  The id of the target parameter.
     */
    void decreaseReferences(QString id);

private slots:

	/*!
	 *  Handler for changes in address space reference.
	 *
	 *      @param [in] addrSpaceName   The name of the referenced address space.
	 */
	void onAddressSpaceChange(const QString& addrSpaceName);

    /*!
     *  Handler for changes in base address.
     */
    void onBaseAddressChange();

private:

	//! No copying
	BusIfInterfaceMaster(const BusIfInterfaceMaster& other);

	//! No assignment
	BusIfInterfaceMaster& operator=(const BusIfInterfaceMaster& other);

    /*!
     *  Formats a given expression.
     *
     *      @param [in] expression  The given expression.
     */
    QString formattedValueFor(QString const& expression) const;

	//! Pointer to the master element being edited.
	QSharedPointer<MasterInterface> master_;
	
	//! Specifies if the edited mode is master or mirrored master
	General::InterfaceMode mode_;

	//! Combo box to select an address space within component
	ReferenceSelector addressSpaceReferenceSelector_;

    //! Editor for the base address of an address space.
    ExpressionEditor* baseAddressEditor_;

    //! The expression parser.
    QSharedPointer<ExpressionParser> expressionParser_;

    //! The parameter finder.
    QSharedPointer<ParameterFinder> parameterFinder_;
};

#endif // BUSIFINTERFACEMASTER_H
