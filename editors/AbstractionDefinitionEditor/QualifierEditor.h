//-----------------------------------------------------------------------------
// File: QualifierEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Anton Hagqvist
// Date: 16.06.2023
//
// Description:
// Editor for qualifiers in 2022 standard Abstraction Definition.
//-----------------------------------------------------------------------------

#ifndef QUALIFIEREDITOR_H
#define QUALIFIEREDITOR_H

#include "QualifierData.h"

#include <IPXACTmodels/common/Qualifier.h>

#include <QLineEdit>
#include <QFrame>
#include <QLabel>
#include <QComboBox>
#include <QCheckBox>

//-----------------------------------------------------------------------------
//! Editor for qualifiers in 2022 standard Abstraction Definition.
//-----------------------------------------------------------------------------
class QualifierEditor : public QFrame
{
	Q_OBJECT

public:

	/*!
     *  The constructor.
	 *
     *      @param [in] libraryAccess   Interface to the library.
     *      @param [in] parent          The owner of this instance
     */
	QualifierEditor(QWidget *parent);

	virtual ~QualifierEditor() = default;

    //! No copying. No assignment.
	QualifierEditor(const QualifierEditor& other) = delete;
	QualifierEditor& operator=(const QualifierEditor& other) = delete;

	/*!
	 *	Setup the qualifier editor.
	 *  
	 *      @param [in] allQualifiers		List of all possible qualifiers.
	 *      @param [in] activeQualifiers	List of selected qualifiers.
	 *      @param [in] attributes			Map of the qualifier's attributes.
	 */
	void setupEditor(QStringList const& allQualifiers, QStringList const& activeQualifiers, QMap<QString, QString> const& attributes);
	
	/*!
	 *	Get the qualifier data from the editor.
	 *
	 * 		@return A struct containing the qualifier data.
	 */
	QualifierData getQualifierData() const;

signals:

	/*!
	 *	Signal emitted when the user has pressed the accept button.
	 */
	void finishEditing();

	/*!
	 *	Signal emitted when user cancels editing by pressing cross.
	 */
	void cancelEditing();

private slots:

	/*!
	 *	Update attribute visibility when qualifier checkbox is toggled.
	 *  
	 *      @param [in] isChecked	Flag indicating if checkbox was checked or not.
	 */
	void onItemClicked(bool isChecked);

private:

	/*!
	 *	Get the currently selected qualifiers.
	 *		
	 * 		@return A list of currently selected qualifiers, as strings.
	 */
	QStringList getSelectedItems() const;

	/*!
	 *	Get the currently set qualifier attributes.
	 *		
	 * 		@return A map of attributes that have been set.
	 */
	QMap<QString, QString> getAttributes() const;

	/*!
	 *	Populate the editor with qualifier checkboxes.
	 */
	void populateCheckBoxes();

	/*!
	 *	Setup the editor layout.
	 */
	void setupLayout();

	/*!
	 *	Set a single attribute value to the corresponding editor.
	 *  
	 *      @param [in] attributeType	The attribute to set.
	 *      @param [in] attributeValue	The value to set.
	 */
	void setQualifierAttribute(Qualifier::Attribute attributeType, QString const& attributeValue);

	/*!
	 *	Get the editor for a given attribute.
	 *  
	 *      @param [in] attribute	The attribute whose editor to look for.
	 *		
	 * 		@return A pointer to the corresponding editor, or nullptr if invalid attribute type.
	 */
	QComboBox* getAttributeEditor(Qualifier::Attribute attribute);

	/*!
	 *	Set the visibility for the attributes of a qualifier, based on if qualifier is set.
	 *  
	 *      @param [in] qualifier	The qualifier, whose attributes are shown/hidden.
	 *      @param [in] visible		Flag indicating if attributes should be shown or hidden.
	 */
	void setQualifierAttributesVisible(Qualifier::Type qualifier, bool visible);

    //-----------------------------------------------------------------------------
	// Data.
	//-----------------------------------------------------------------------------

	/*!
	 *	Get the attributes for a given qualifier.
	 *  
	 *      @param [in] qualifier	The qualifier.
	 *		
	 * 		@return A list of attributes for a qualifier.
	 */
	QList<Qualifier::Attribute> getQualifierTypeAttributes(Qualifier::Type qualifier);

	//! The editor for the power domain reference. Not used in absDef.
    QLineEdit* powerDomainLineEdit_;

	//! The editor for a user defined attribute.
    QLineEdit* userDefinedLineEdit_;

	//! Combo boxes for pre-set levels.
	QComboBox* resetLevelSelector_;
	QComboBox* clockEnableLevelSelector_;
	QComboBox* powerEnableLevelSelector_;

	//! Editable combobox for flow control flow type.
	QComboBox* flowTypeSelector_;

	//! List of the qualifier checkboxes.
	QList<QCheckBox*> qualifierBoxes_;

	//! List of pre-set flow types. If flow type is not in this list,
	//  Then it will be set as user.
	const QStringList flowTypes_ = {
		QStringLiteral("creditReturn"),
		QStringLiteral("ready"),
		QStringLiteral("busy")
	};
};

#endif // QUALIFIEREDITOR_H
