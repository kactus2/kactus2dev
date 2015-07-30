/* 
 *
 *  Created on: 7.4.2011
 *      Author: Antti Kamppi
 * 		filename: busifinterfacemslave.h
 */

#ifndef BUSIFINTERFACEMSLAVE_H
#define BUSIFINTERFACEMSLAVE_H

#include "busifinterfacemodeeditor.h"
#include <IPXACTmodels/businterface.h>
#include <IPXACTmodels/component.h>
#include <IPXACTmodels/mirroredslaveinterface.h>

#include <editors/ComponentEditor/common/ParameterFinder.h>

#include <QSharedPointer>
#include <QLineEdit>

class ExpressionEditor;
class ExpressionParser;

/*! \brief Editor to edit mirrored slave details of a bus interface.
 *
 */
class BusIfInterfaceMSlave : public BusIfInterfaceModeEditor {
	Q_OBJECT

public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] busif               Pointer to the bus interface being edited.
	 *      @param [in] component           Pointer to the component being edited.
	 *      @param [in] parameterFinder     Pointer to the parameter finder.
     *      @param [in] expressionParser    Pointer to the expression parser.
	 *      @param [in] parent              Pointer to the owner of this editor.
	 */
	BusIfInterfaceMSlave(QSharedPointer<BusInterface> busif,
		QSharedPointer<Component> component,
        QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<ExpressionParser> expressionParser,
		QWidget *parent);
	
	//! \brief The destructor
	virtual ~BusIfInterfaceMSlave();

	/*! \brief Check for the validity of the edited item.
	*
	* \return True if item is valid.
	*/
	virtual bool isValid() const;

	/*! \brief Restore the changes made in the editor back to ones in the model.
	*
	*/
	virtual void refresh();

	/*! \brief Get the interface mode of the editor
	 * 
	 * \return General::InterfaceMode Specifies the interface mode.
	*/
	virtual General::InterfaceMode getInterfaceMode() const;

	//! \brief Save the interface mode-specific details to the bus interface.
	virtual void saveModeSpecific();

    /*!
     *  Remove all the references from the expressions contained within this interface.
     */
    void removeReferencesFromExpressions();

private slots:

    /*!
     *  Handler for changes in remap address.
     */
    void onRemapChange();

    /*!
     *  Handler for changes in range.
     */
    void onRangeChange();

signals:

    /*!
     *  Increase the number of references in the selected parameter.
     *
     *      @param [in] id      The id of the selected parameter.
     */
    void increaseReferences(QString const& id);

    /*!
     *  Decrease the number of references in the selected parameter.
     *
     *      @param [in] id      The id of the selected parameter.
     */
    void decreaseReferences(QString const& id);

private:
	
	//! \brief No copying
	BusIfInterfaceMSlave(const BusIfInterfaceMSlave& other);

	//! No assignment
	BusIfInterfaceMSlave& operator=(const BusIfInterfaceMSlave& other);

    /*!
     *  Calculates a given expression.
     *
     *      @param [in] expression  The expression to be formatted.
     */
    QString formattedValueFor(QString const& expression) const;

	//! \brief Pointer to the mirrored slave interface mode being edited.
	QSharedPointer<MirroredSlaveInterface> mirroredSlave_;

    //! Editor for the mirrored slave remap address.
    ExpressionEditor* remapEditor_;

    //! Editor for the mirrored slave range.
    ExpressionEditor* rangeEditor_;

    //! The expression parser for calculating the expression.
    QSharedPointer<ExpressionParser> expressionParser_;

    //! The parameter finder for searching the component parameters.
    QSharedPointer<ParameterFinder> parameterFinder_;
};

#endif // BUSIFINTERFACEMSLAVE_H
