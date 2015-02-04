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

class ParameterComboBox;

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
	 *      @param [in] parent              Pointer to the owner of this editor.
	 */
	BusIfInterfaceMSlave(QSharedPointer<BusInterface> busif,
		QSharedPointer<Component> component,
        QSharedPointer<ParameterFinder> parameterFinder,
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
     *  Gets the current id of selected remap address.
     *
     *      @return The id of the selected remap address.
     */
    QString getRemapAddressID();

    /*!
     *  Gets the current id of selected range.
     *
     *      @return The id of the selected range.
     */
    QString getRangeID();

public slots:

    /*!
     *  Catches any parameter changes in the bus interface parameters.
     */
    void onBusIfParametersChanged();

private slots:

	//! \brief Handler for changes in remap address.
	void onRemapChange(const QString& newRemapAddress);

	//! \brief Handler for changes in range.
	void onRangeChange(const QString& newRange);

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
     *  Check if the selected value is within the combo box.
     *
     *      @param [in] combo       The selected combo box.
     *      @param [in] newText     The text that was placed in the box.
     *
     *      @return True, if the text is found in the combo box, false otherwise.
     */
    bool isBoxValueParameter(ParameterComboBox* combo, QString newText);

	//! \brief Pointer to the mirrored slave interface mode being edited.
	QSharedPointer<MirroredSlaveInterface> mirroredSlave_;

	//! \brief Editor to set the remap address.
	ParameterComboBox* remapEdit_;

	//! \brief Editor to set the range.
    ParameterComboBox* rangeEdit_;
};

#endif // BUSIFINTERFACEMSLAVE_H
