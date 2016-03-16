//-----------------------------------------------------------------------------
// File: tst_ComDefinitionValidator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 28.01.2016
//
// Description:
// Unit test for class ComDefinitionValidator.
//-----------------------------------------------------------------------------


#include <IPXACTmodels/kactusExtensions/ComDefinition.h>
#include <IPXACTmodels/kactusExtensions/validators/ComDefinitionValidator.h>

#include <QtTest>

class tst_ComDefinitionValidator : public QObject
{
	Q_OBJECT

public:
    tst_ComDefinitionValidator();

private slots:
	void baseCase();
};

//-----------------------------------------------------------------------------
// Function: tst_ComDefinitionValidator::tst_ComDefinitionValidator()
//-----------------------------------------------------------------------------
tst_ComDefinitionValidator::tst_ComDefinitionValidator()
{

}

//-----------------------------------------------------------------------------
// Function: tst_ComDefinitionValidator::baseCase()
//-----------------------------------------------------------------------------
void tst_ComDefinitionValidator::baseCase()
{
	QSharedPointer<ComDefinition> ComDefinition(new ComDefinition);
	ComDefinition->setVlnv(VLNV(VLNV::COMDEFINITION,"me","kurjasto","def","0.11"));

	QVector<QString> errorList;
    ComDefinitionValidator validator;
	validator.findErrorsIn(errorList, ComDefinition);

	QVERIFY(validator.validate(ComDefinition));
    QCOMPARE(errorList.size(), 0);
}

QTEST_APPLESS_MAIN(tst_ComDefinitionValidator)

#include "tst_ComDefinitionValidator.moc"
