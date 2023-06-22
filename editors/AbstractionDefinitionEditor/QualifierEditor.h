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

	void setupEditor(QStringList const& allQualifiers, QStringList const& activeQualifiers, QMap<QString, QString> attributes);
	
	QStringList getSelectedItems() const;

	QMap<QString, QString> getAttributes() const;

public slots:

	void onItemClicked(bool isChecked);

	void onItemCheckChanged(Qt::CheckState newState);

private:

	void populateCheckBoxes();

	void setupLayout();

	void setQualifierAttribute(Qualifier::Attribute attributeType, QString const& attributeValue);

	QComboBox* getAttributeEditor(Qualifier::Attribute attribute);

	QLabel* getAttributeLabel(Qualifier::Attribute attribute);

	void updateAttributeList(QStringList const& selectedItems);

	void setQualifierAttributesVisible(Qualifier::Type qualifier, bool visible);

	QList<Qualifier::Attribute> getQualifierTypeAttributes(Qualifier::Type qualifier);

    QLineEdit powerDomainLineEdit_;
    QLineEdit userDefinedLineEdit_;

	QComboBox resetLevelSelector_;
	QComboBox clockEnableLevelSelector_;
	QComboBox powerEnableLevelSelector_;
	QComboBox flowTypeSelector_;

	QList<QCheckBox*> qualifierBoxes_;

	QList<QComboBox*> levelSelectors_;

	const QStringList flowTypes_ = {
		QStringLiteral("credit-return"),
		QStringLiteral("ready"),
		QStringLiteral("busy")
	};

	QLabel resetLevelLabel_ = QLabel(QStringLiteral("Reset level:"));
	QLabel clockEnableLevelLabel_ = QLabel(QStringLiteral("Clock enable level:"));
	QLabel powerEnableLevelLabel_ = QLabel(QStringLiteral("Power enable level:"));
	QLabel powerDomainRefLabel_ = QLabel(QStringLiteral("Power domain:"));
	QLabel flowTypeLabel_ = QLabel(QStringLiteral("Flow type:"));
	QLabel userFlowTypeLabel_ = QLabel(QStringLiteral("User flow type:"));
	QLabel userDefinedLabel_ = QLabel(QStringLiteral("User defined:"));
	
};

#endif // QUALIFIEREDITOR_H
