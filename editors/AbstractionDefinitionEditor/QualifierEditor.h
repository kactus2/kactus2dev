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

#include <common/widgets/EnumCollectionEditor/EnumCollectionEditor.h>

#include <IPXACTmodels/common/Qualifier.h>

#include <QLineEdit>
#include <QFrame>
#include <QLabel>

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

	void setResetEnableLevel(QString const& level);

	void setClockEnableLevel(QString const& level);

	void setPowerEnableLevel(QString const& level);

	void setPowerDomainRef(QString const& ref);

	void setFlowType(QString const& flowType);

	void setUserFlowType(QString const& userFlowType);

	void setUserDefined(QString const& userDefined);

	void setupEditor(QStringList const& allQualifiers, QStringList const& activeQualifiers, QMap<QString, QString> attributes);
	
	QStringList getSelectedItems() const;

	QMap<QString, QString> getAttributes() const;

public slots:

	void onItemCheckChanged(Qt::CheckState newState);

private:

	void setupLayout();

	void setQualifierAttribute(Qualifier::Attribute attributeType, QString const& attributeValue);

	QLineEdit* getAttributeEditor(Qualifier::Attribute attribute);

	QLabel* getAttributeLabel(Qualifier::Attribute attribute);

	void updateAttributeList(QStringList const& selectedItems);

	void setQualifierAttributesVisible(Qualifier::Type qualifier, bool visible);

	QList<Qualifier::Attribute> getQualifierTypeAttributes(Qualifier::Type qualifier);

	EnumCollectionEditor* enumerationEditor_;

    QLineEdit resetLevelLineEdit_;
    QLineEdit clockEnableLevelLineEdit_;
    QLineEdit powerEnableLevelLineEdit_;
    QLineEdit powerDomainLineEdit_;
    QLineEdit flowTypeLineEdit_;
    QLineEdit userFlowTypeLineEdit_;
    QLineEdit userDefinedLineEdit_;

	QLabel resetLevelLabel_ = QLabel(QStringLiteral("Reset level:"));
	QLabel clockEnableLevelLabel_ = QLabel(QStringLiteral("Clock enable level:"));
	QLabel powerEnableLevelLabel_ = QLabel(QStringLiteral("Power enable level:"));
	QLabel powerDomainRefLabel_ = QLabel(QStringLiteral("Power domain:"));
	QLabel flowTypeLabel_ = QLabel(QStringLiteral("Flow type:"));
	QLabel userFlowTypeLabel_ = QLabel(QStringLiteral("User flow type:"));
	QLabel userDefinedLabel_ = QLabel(QStringLiteral("User defined:"));
	
};

#endif // QUALIFIEREDITOR_H
