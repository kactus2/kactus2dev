/* 
 *  	Created on: 23.2.2012
 *      Author: Antti Kamppi
 * 		filename: parametergroupbox.h
 *		Project: Kactus 2
 */

#ifndef PARAMETERGROUPBOX_H
#define PARAMETERGROUPBOX_H

#include <IPXACTmodels/parameter.h>

#include <editors/ComponentEditor/parameters/parametersmodel.h>
#include <common/views/EditableTableView/editabletableview.h>

#include <QGroupBox>
#include <QList>
#include <QSharedPointer>
#include <QSortFilterProxyModel>

class Choice;

/*! \brief Group box that can be used to edit list of parameters.
 *
 */
class ParameterGroupBox : public QGroupBox {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param parameters    The list containing the parameters.
     * \param component     The containing component whose parameters are being edited.
	 * \param parent        Pointer to the owner of this editor.
	 *
	*/
	ParameterGroupBox(QList<QSharedPointer<Parameter> >& parameters,
        QSharedPointer<Component> component,
		QWidget *parent);
	
	//! \brief The destructor
	virtual ~ParameterGroupBox();

	/*! \brief Check for the validity of the edited parameters
	*
	* \return True if all parameters are in valid state.
	*/
	virtual bool isValid() const;

	/*! \brief Check for the validity of the edited parameters.
	*
	* \param errorList          The list to add the possible error messages to.
	* \param parentIdentifier   String from parent to help to identify the location of the error.
	*
	* \return bool True if the state is valid and writing is possible.
	*/
	bool isValid(QStringList& errorList, const QString& parentIdentifier) const;

	/*! \brief Restore the changes made in the editor back to ones in parameter models.
	*
	*/
	virtual void refresh();

signals:

	//! \brief Emitted when contents of the editor change.
	void contentChanged();

	//! \brief Prints an error message to the user.
	void errorMessage(const QString& msg) const;

	//! \brief Prints a notification to user.
	void noticeMessage(const QString& msg) const;

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

private:
	//! \brief No copying
	ParameterGroupBox(const ParameterGroupBox& other);

	//! \brief No assignment
	ParameterGroupBox& operator=(const ParameterGroupBox& other);

	//! \brief The view that displays the parameters.
	EditableTableView view_;

	//! \brief The model that holds the data to be displayed to the user
	ParametersModel* model_;

	//! \brief The proxy that is used to do the sorting of parameters.
	QSortFilterProxyModel proxy_;
};

#endif // PARAMETERGROUPBOX_H
