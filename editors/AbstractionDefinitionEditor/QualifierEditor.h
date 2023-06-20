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

#include <QLineEdit>

#include <QFrame>

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

	void setupEditor(QStringList const& allQualifiers, QStringList const& activeQualifiers);

private:

	void setupLayout();

	EnumCollectionEditor* enumerationEditor_;

	QLineEdit resetLevelLineEdit_;
	QLineEdit clockEnableLevelLineEdit_;
	QLineEdit powerEnableLevelLineEdit_;
	QLineEdit powerDomainLineEdit_;
	QLineEdit flowTypeLineEdit_;
	QLineEdit userFlowTypeLineEdit_;
	QLineEdit userDefinedLineEdit_;
	
};

#endif // QUALIFIEREDITOR_H
