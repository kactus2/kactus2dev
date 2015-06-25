//-----------------------------------------------------------------------------
// File: VHDLPackageImport.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Sillanpää
// Date: 25.06.2015
//
// Description:
// Import plugin for VHDL package.
//-----------------------------------------------------------------------------

#include "VHDLPackageImport.h"
#include "VHDLSyntax.h"

#include <Plugins/PluginSystem/ImportPlugin/ImportColors.h>
#include <Plugins/PluginSystem/ImportPlugin/Highlighter.h>
#include <Plugins/PluginSystem/ImportPlugin/ModelParameterVisualizer.h>


#include <IPXACTmodels/component.h>
#include <IPXACTmodels/model.h>
#include <IPXACTmodels/modelparameter.h>
#include <IPXACTmodels/parameter.h>
#include <IPXACTmodels/view.h>

#include <stdio.h>
#include <QRegularExpression>
#include <QtGui>

namespace
{
    //QRegularExpression const PACKAGE("PACKAGE\\s+(\\w+)\\s+IS\\s+.*END\\s+(\\w+)\\s*[;]",QRegularExpression::CaseInsensitiveOption);
    QRegularExpression const PACKAGE_START("PACKAGE\\s+(\\w+)\\s+IS\\s+",QRegularExpression::CaseInsensitiveOption);
    QRegularExpression const PACKAGE_END("END\\s*(\\w+)\\s*[;]",QRegularExpression::CaseInsensitiveOption);
    //QRegularExpression const CONSTANT("\\bconstant\\s+",QRegularExpression::CaseInsensitiveOption);
    //QRegularExpression const CONSTANT_par("\\bconstant\\s*(\\w+)\\s*[:]\\s*(\\w+)(?:\\s*"+ VHDLSyntax::DEFAULT +")\\s*[;]",QRegularExpression::CaseInsensitiveOption);
    QRegularExpression const CONSTANT_par("\\bconstant\\s*(\\w+)\\s*[:]\\s*(\\w+)\\s*[:][=]\\s*(\\d*)\\s*[;]",QRegularExpression::CaseInsensitiveOption);
    //QRegularExpression const VARIABLE("variable\\s+",QRegularExpression::CaseInsensitiveOption);
    QRegularExpression const FUNCTION("^function\\s+\\w+.*return\\w+\\s*[;]$",QRegularExpression::CaseInsensitiveOption);
    QRegularExpression const PROCEDURE("procedure\\s*\\w+\\s*[(].*[)]\\s*[;]",QRegularExpression::CaseInsensitiveOption);
}

//-----------------------------------------------------------------------------
// Function: VHDLPackageImport::VHDLPackageImport()
//-----------------------------------------------------------------------------
VHDLPackageImport::VHDLPackageImport() : QObject(0), highlighter_(0)
{

}

//-----------------------------------------------------------------------------
// Function: VHDLPackageImport::~VHDLPackageImport()
//-----------------------------------------------------------------------------
VHDLPackageImport::~VHDLPackageImport()
{

}

//-----------------------------------------------------------------------------
// Function: VHDLPackageImport::getName()
//-----------------------------------------------------------------------------
QString VHDLPackageImport::getName() const
{
    return "VHDL package import";
}

//-----------------------------------------------------------------------------
// Function: VHDLPackageImport::getVersion()
//-----------------------------------------------------------------------------
QString VHDLPackageImport::getVersion() const
{
    return "1.0";
}

//-----------------------------------------------------------------------------
// Function: VHDLPackageImport::getDescription()
//-----------------------------------------------------------------------------
QString VHDLPackageImport::getDescription() const
{
    return "Import constants from vhdl package file.";
}

//-----------------------------------------------------------------------------
// Function: VHDLPackageImport::getVendor()
//-----------------------------------------------------------------------------
QString VHDLPackageImport::getVendor() const
{
    return tr("TUT");
}

//-----------------------------------------------------------------------------
// Function: VHDLPackageImport::getLicence()
//-----------------------------------------------------------------------------
QString VHDLPackageImport::getLicence() const
{
    return tr("GPL2");
}

//-----------------------------------------------------------------------------
// Function: VHDLPackageImport::getLicenceHolder()
//-----------------------------------------------------------------------------
QString VHDLPackageImport::getLicenceHolder() const
{
    return tr("Public");
}

//-----------------------------------------------------------------------------
// Function: VHDLPackageImport::getSettingsWidget()
//-----------------------------------------------------------------------------
PluginSettingsWidget* VHDLPackageImport::getSettingsWidget()
{
    return new PluginSettingsWidget();
}

//-----------------------------------------------------------------------------
// Function: VHDLPackageImport::getProgramRequirements()
//-----------------------------------------------------------------------------
QList<IPlugin::ExternalProgramRequirement> VHDLPackageImport::getProgramRequirements()
{
    return QList<IPlugin::ExternalProgramRequirement>();
}

//-----------------------------------------------------------------------------
// Function: VHDLPackageImport::getSupportedFileTypes()
//-----------------------------------------------------------------------------
QStringList VHDLPackageImport::getSupportedFileTypes() const
{
    QStringList supportedTypes;
    supportedTypes << "vhdlSource" << "vhdlSource-87" << "vhdlSource-93";
    return supportedTypes;
}

//-----------------------------------------------------------------------------
// Function: VHDLPackageImport::getCompatibilityWarnings()
//-----------------------------------------------------------------------------
QString VHDLPackageImport::getCompatibilityWarnings() const
{
    return QString();
}

//-----------------------------------------------------------------------------
// Function: VHDLPackageImport::import()
//-----------------------------------------------------------------------------
void VHDLPackageImport::import(QString const& input, QSharedPointer<Component> targetComponent)
{
    if (hasPackageDeclaration(input))
    {
        QStringList constants;

        int startIndex = PACKAGE_START.match(input).capturedEnd();
        int length = input.indexOf(PACKAGE_END) - startIndex;

        QString package = input.mid(startIndex, length);

        QRegExp commentLine("^" + VHDLSyntax::SPACE + VHDLSyntax::COMMENT_LINE_EXP + "|" +
            VHDLSyntax::ENDLINE + VHDLSyntax::SPACE + VHDLSyntax::COMMENT_LINE_EXP);
        package.remove(commentLine);
        package.remove(FUNCTION);
        package.remove(PROCEDURE);

        //constants.append(findDeclarations(CONSTANT_par,package));
        int nextGeneric = package.indexOf(CONSTANT_par, 0);
        while (nextGeneric != -1)
        {
            QRegularExpressionMatch match = CONSTANT_par.match(package, nextGeneric);
            constants.append(match.captured());
            nextGeneric = package.indexOf(CONSTANT_par, nextGeneric + match.capturedLength());
        }

//        qDebug() << constants.size();
//        qDebug() << package;

//        for(int i = 0; i<constants.size();i++)
//        {
//            qDebug() << constants[i];
//        }
        foreach (QString constant, constants)
        {

            createModelParameterFromDefinition(constant,targetComponent);
            highlightDefinition(constant);
        }


    }

}

//-----------------------------------------------------------------------------
// Function: VHDLPackageImport::setHighlighter()
//-----------------------------------------------------------------------------
void VHDLPackageImport::setHighlighter(Highlighter* highlighter)
{
    highlighter_ = highlighter;
}

//-----------------------------------------------------------------------------
// Function: VerilogImporter::hasModuleDeclaration()
//-----------------------------------------------------------------------------
bool VHDLPackageImport::hasPackageDeclaration(QString const& input)
{
    int packageBegin = input.indexOf(PACKAGE_START);
    int packageEnd = input.indexOf(PACKAGE_END,packageBegin);

    return packageBegin != -1 && packageEnd != -1;
}
//-----------------------------------------------------------------------------
// Function: VerilogParameterParser::findDeclarations()
//-----------------------------------------------------------------------------
QStringList VHDLPackageImport::findDeclarations(QRegularExpression const& declarationRule,
    QString const& inspect)
{
    QStringList declarations;

    int previousStart = 0;
    int declarationStart = inspect.indexOf(declarationRule);

    // Repeat the parsing until no more matches are found.
    while (declarationStart != -1)
    {
        int declarationLength = declarationRule.match(inspect, declarationStart).capturedLength();

        // Seek for the next match beginning from the end of the previous match.
        previousStart = declarationStart;
        declarationStart = inspect.indexOf(declarationRule, declarationStart  + declarationLength + 1);

        // Take the matching part and append to the list.
        QString declaration = inspect.mid(previousStart, declarationStart - previousStart);
        declaration = declaration.trimmed();

        // Highlight the selection if applicable.

        //highlightDefinition(declaration);
        declarations.append(declaration);

    }
    return declarations;
}

//-----------------------------------------------------------------------------
// Function: VHDLPackageImport::highlightDefinition()
//-----------------------------------------------------------------------------
void VHDLPackageImport::highlightDefinition(QString const& definition)
{
    if (highlighter_)
    {
        highlighter_->applyFontColor(definition, Qt::black);
        highlighter_->applyHighlight(definition, ImportColors::MODELPARAMETER);
    }
}

//-----------------------------------------------------------------------------
// Function: VHDLPackageImport::createParameterFromDefinition()
//-----------------------------------------------------------------------------
void VHDLPackageImport::createModelParameterFromDefinition(QString const& definition,
    QSharedPointer<Component> targetComponent)
{
   // QRegularExpressionMatch match = CONSTANT_par.match(definition);
    QString name = CONSTANT_par.match(definition).captured(1);
    qDebug() << name;

    QString dataType = CONSTANT_par.match(definition).captured(2);
    QString value = CONSTANT_par.match(definition).captured(3);
        qDebug() << dataType;
        qDebug() << value;
    QSharedPointer<ModelParameter> parameter = findModelParameterByName(targetComponent, name);
    if (parameter.isNull())
    {
        parameter = QSharedPointer<ModelParameter>(new ModelParameter());
        parameter->setName(name);
        targetComponent->getModel()->getModelParameters()->append(parameter);
    }

    if (value.isEmpty())
    {
        value = "0";
    }

    parameter->setDataType(dataType);
    parameter->setValue(value);
    //parameter->setDescription(description);
    parameter->setAttribute("imported", "true");
}

//-----------------------------------------------------------------------------
// Function: VHDLPackageImport::findParameterByName()
//-----------------------------------------------------------------------------
QSharedPointer<ModelParameter> VHDLPackageImport::findModelParameterByName(QSharedPointer<Component> targetComponent,
    QString const& name) const
{
    QSharedPointer<ModelParameter> modelparameter;
    foreach (QSharedPointer<ModelParameter> existingModelParameter, *targetComponent->getModelParameters())
    {
        if (existingModelParameter->getName() == name)
        {
            return existingModelParameter;
        }
    }

    return modelparameter;
}
