//-----------------------------------------------------------------------------
// File: tst_ApiDefinitionValidator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 28.01.2016
//
// Description:
// Unit test for class ApiDefinitionValidator.
//-----------------------------------------------------------------------------


#include <IPXACTmodels/kactusExtensions/ApiDefinition.h>
#include <IPXACTmodels/kactusExtensions/validators/ApiDefinitionValidator.h>

#include <QtTest>

class tst_ApiDefinitionValidator : public QObject
{
	Q_OBJECT

public:
    tst_ApiDefinitionValidator();

private slots:
	void baseCase();
};

//-----------------------------------------------------------------------------
// Function: tst_ApiDefinitionValidator::tst_ApiDefinitionValidator()
//-----------------------------------------------------------------------------
tst_ApiDefinitionValidator::tst_ApiDefinitionValidator()
{

}

//-----------------------------------------------------------------------------
// Function: tst_ApiDefinitionValidator::baseCase()
//-----------------------------------------------------------------------------
void tst_ApiDefinitionValidator::baseCase()
{
	QSharedPointer<ApiDefinition> apiDefinition(new ApiDefinition);
	apiDefinition->setVlnv(VLNV(VLNV::APIDEFINITION,"me","kurjasto","def","0.11"));

	QVector<QString> errorList;
    ApiDefinitionValidator validator;
	validator.findErrorsIn(errorList, apiDefinition);

	QVERIFY(validator.validate(apiDefinition));
    QCOMPARE(errorList.size(), 0);
}

QTEST_APPLESS_MAIN(tst_ApiDefinitionValidator)

#include "tst_ApiDefinitionValidator.moc"
