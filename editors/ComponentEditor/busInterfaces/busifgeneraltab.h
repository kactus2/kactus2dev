/* 
 *
 *  Created on: 5.4.2011
 *      Author: Antti Kamppi
 * 		filename: busifgeneraltab.h
 */

#ifndef BUSIFGENERALTAB_H
#define BUSIFGENERALTAB_H

#include "busifgeneraldetails.h"
#include "interfacemodestack.h"

#include <common/widgets/nameGroupEditor/namegroupeditor.h>
#include <common/widgets/vlnvEditor/vlnveditor.h>
#include <common/widgets/ParameterGroupBox/parametergroupbox.h>

#include <editors/ComponentEditor/common/ParameterFinder.h>
#include <editors/ComponentEditor/common/ExpressionFormatter.h>
#include <editors/ComponentEditor/common/ExpressionParser.h>

#include <IPXACTmodels/component.h>
#include <IPXACTmodels/businterface.h>

#include <QWidget>
#include <QSharedPointer>
#include <QStackedWidget>

class LibraryInterface;

/*! \brief Container for editor on the general tab of a bus interface editor.
 *
 */
class BusIfGeneralTab : public QWidget {
	Q_OBJECT

public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] libHandler              Pointer to the instance that manages the library.
	 *      @param [in] busif                   Pointer to the bus interface being edited.
	 *      @param [in] component               Pointer to the component being edited.
	 *      @param [in] parameterFinder         Pointer to the parameter finder.
	 *      @param [in] expressionFormatter     Pointer to the expression formatter.
     *      @param [in] expressionParser        Pointer to the expression parser.
	 *      @param [in] parent                  Pointer to the owner of this editor.
	 *      @param [in] parentWnd               Pointer to the parent window.
	 */
	BusIfGeneralTab(LibraryInterface* libHandler,
		QSharedPointer<BusInterface> busif,
		QSharedPointer<Component> component,
        QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<ExpressionFormatter> expressionFormatter,
        QSharedPointer<ExpressionParser> expressionParser,
		QWidget* parent,
        QWidget* parentWnd);
	
	//! \brief The destructor
	virtual ~BusIfGeneralTab();

	/*! \brief Check for the validity of the edited item.
	*
	* \return True if item is valid.
	*/
	virtual bool isValid() const;

    /*!
     * Check for the validity of the edited item.
     *
     * \param errorList The list to add possible error messages to.
     *
     * \return True if item is valid
     */
    virtual bool isValid(QStringList& errorList) const; 

	/*! \brief Restore the changes made in the editor back to ones in the model.
	*
	*/
	virtual void refresh();

	/*! \brief Get the currently set bus type.
	 *
	 * \return VLNV that is set for the current bus type.
	*/
	VLNV getBusType() const;

	/*! \brief Get the currently set abstraction type.
	 *
	 * \return VLNV that is set for the current abstraction type.
	*/
	VLNV getAbsType() const;

	/*! \brief Set the abstraction type mandatory or not.
	 *
	 * \param isMandatory Whether abstraction type is mandatory or not.
	*/
    void setAbsTypeMandatory(bool isMandatory);


    /*!
     *  Locks/unlocks the VLNV selection for bus definition and abstraction definition.
     *
     *      @param [in] locked   The lock to set.
     */
    void setBusTypesLock( bool locked );

signals:

	//! \brief Emitted when contents of the model change
	void contentChanged();

	//! \brief Prints an error message to the user.
	void errorMessage(const QString& msg) const;

	//! \brief Prints a notification to user.
	void noticeMessage(const QString& msg) const;

	//! Emitted when a help page should be changed in the context help window.
	void helpUrlRequested(QString const& url);

    /*!
     *  Increase the amount of references to the parameter corresponding to the id.
     *
     *      @param [in] id      The id of the parameter being searched for.
     */
    void increaseReferences(QString id);

    /*!
     *  Decrease the amount of references to the parameter corresponding to the id.
     *
     *      @param [in] id      The id of the parameter being searched for.
     */
    void decreaseReferences(QString id);

    /*!
     *  Open the reference tree of the parameter corresponding to the id.
     *
     *      @param [in] id      The id of the parameter.
     */
    void openReferenceTree(QString const& id) const;

protected:

	//! \brief Handler for widget's show event
	virtual void showEvent(QShowEvent* event);

private slots:

	//! \brief Handler for changes in the bus type.
	void onBusTypeChanged();

	//! \brief Handler for changes in the abstraction type.
	void onAbsTypeChanged();

	//! \brief Handler for changes in interface mode.
	void onModeChanged(General::InterfaceMode mode);

	/*! \brief Set the bus type for the bus interface.
	 *
	 * Method: 		onSetBusType
	 * Full name:	BusIfGeneralTab::onSetBusType
	 * Access:		private 
	 *
	 * \param busDefVLNV The vlnv identifying the bus type.
	 *
	*/
	void onSetBusType(const VLNV& busDefVLNV);

	/*! \brief Set the abstraction type for the bus interface.
	 *
	 * Method: 		onSetAbsType
	 * Full name:	BusIfGeneralTab::onSetAbsType
	 * Access:		private 
	 *
	 * \param absDefVLNV The vlnv identifying the abstraction definition.
	 *
	*/
	void onSetAbsType(const VLNV& absDefVLNV);

private:

	//! \brief No copying
	BusIfGeneralTab(const BusIfGeneralTab& other);

	//! No assignment
	BusIfGeneralTab& operator=(const BusIfGeneralTab& other);

    //! Sets the widget layout.
    void setupLayout();

    //-----------------------------------------------------------------------------
    //! Data.
    //-----------------------------------------------------------------------------

	//! \brief Pointer to the bus interface being edited.
	QSharedPointer<BusInterface> busif_;

	//! \brief Contains the name, display name and description of bus interface
	NameGroupEditor nameEditor_;

	//! \brief Contains the vlnv of the bus type for this bus interface
	VLNVEditor busType_;

	//! \brief Contains the vlnv of the abstraction type for this interface
	VLNVEditor absType_;

	//! \brief Contains the mode-specific editors
	InterfaceModeStack modeStack_;

	//! \brief Contains the details of this bus interface
	BusIfGeneralDetails details_;

	//! \brief Contains the parameters of bus interface.
	ParameterGroupBox parameters_;

	//! \brief Pointer to the instance that manages the library
	LibraryInterface* libHandler_;
};

#endif // BUSIFGENERALTAB_H
