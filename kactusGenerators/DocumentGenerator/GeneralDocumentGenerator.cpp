//-----------------------------------------------------------------------------
// File: GeneralDocumentGenerator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 27.10.2017
//
// Description:
// Contains general functions for document generation.
//-----------------------------------------------------------------------------

#include "GeneralDocumentGenerator.h"

#include <kactusGenerators/DocumentGenerator/DocumentGeneratorHTML.h>

#include <editors/ComponentEditor/common/ExpressionFormatterFactory.h>

//-----------------------------------------------------------------------------
// Function: GeneralDocumentGenerator::GeneralDocumentGenerator()
//-----------------------------------------------------------------------------
GeneralDocumentGenerator::GeneralDocumentGenerator(LibraryInterface* handler,
    ExpressionFormatterFactory* formatterFactory, QObject* parent):
QObject(parent),
libraryHandler_(handler),
component_(),
componentNumber_(0),
targetPath_(),
expressionFormatterFactory_(formatterFactory)
{

}

//-----------------------------------------------------------------------------
// Function: GeneralDocumentGenerator::~GeneralDocumentGenerator()
//-----------------------------------------------------------------------------
GeneralDocumentGenerator::~GeneralDocumentGenerator()
{

}

//-----------------------------------------------------------------------------
// Function: GeneralDocumentGenerator::writeTableElement()
//-----------------------------------------------------------------------------
void GeneralDocumentGenerator::writeTableElement(const QStringList& headers, const QString& title,
    QTextStream& stream, QString const& tableTabs)
{
    QString headerTabs = tableTabs + "\t";
    QString headerRowTabs = headerTabs + "\t";

    stream << tableTabs << DocumentGeneratorHTML::TABLE << title << "\">" << endl;
    stream << headerTabs << "<tr>" << endl;
    foreach (QString header, headers)
    {
        stream << headerRowTabs << "<th>" << header << "</th>" << endl;
    }
    stream << headerTabs << "</tr>\n";
}

//-----------------------------------------------------------------------------
// Function: GeneralDocumentGenerator::setComponent()
//-----------------------------------------------------------------------------
void GeneralDocumentGenerator::setComponent(QSharedPointer<Component> newComponent)
{
    component_ = newComponent;
}

//-----------------------------------------------------------------------------
// Function: GeneralDocumentGenerator::getComponent()
//-----------------------------------------------------------------------------
QSharedPointer<Component> GeneralDocumentGenerator::getComponent() const
{
    return component_;
}

//-----------------------------------------------------------------------------
// Function: GeneralDocumentGenerator::setNumber()
//-----------------------------------------------------------------------------
void GeneralDocumentGenerator::setNumber(int const& newNumber)
{
    componentNumber_ = newNumber;
}

//-----------------------------------------------------------------------------
// Function: GeneralDocumentGenerator::myNumber()
//-----------------------------------------------------------------------------
int GeneralDocumentGenerator::myNumber() const
{
    return componentNumber_;
}

//-----------------------------------------------------------------------------
// Function: GeneralDocumentGenerator::setTargetPath()
//-----------------------------------------------------------------------------
void GeneralDocumentGenerator::setTargetPath(QString const& newPath)
{
    targetPath_ = newPath;
}

//-----------------------------------------------------------------------------
// Function: GeneralDocumentGenerator::getTargetPath()
//-----------------------------------------------------------------------------
QString GeneralDocumentGenerator::getTargetPath() const
{
    return targetPath_;
}

//-----------------------------------------------------------------------------
// Function: GeneralDocumentGenerator::getLibraryHandler()
//-----------------------------------------------------------------------------
LibraryInterface* GeneralDocumentGenerator::getLibraryHandler() const
{
    return libraryHandler_;
}

//-----------------------------------------------------------------------------
// Function: GeneralDocumentGenerator::getExpressionFormatterFactory()
//-----------------------------------------------------------------------------
ExpressionFormatterFactory* GeneralDocumentGenerator::getExpressionFormatterFactory() const
{
    return expressionFormatterFactory_;
}

//-----------------------------------------------------------------------------
// Function: GeneralDocumentGenerator::createExpressionFormatter()
//-----------------------------------------------------------------------------
ExpressionFormatter* GeneralDocumentGenerator::createExpressionFormatter() const
{
    return expressionFormatterFactory_->makeExpressionFormatter(component_);
}

//-----------------------------------------------------------------------------
// Function: GeneralDocumentGenerator::createDesignInstanceFormatter()
//-----------------------------------------------------------------------------
ExpressionFormatter* GeneralDocumentGenerator::createDesignInstanceFormatter(QSharedPointer<Design> design,
    QSharedPointer<Component> instancedComponent) const
{
    return expressionFormatterFactory_->createDesignInstanceFormatter(instancedComponent, design);
}

//-----------------------------------------------------------------------------
// Function: GeneralDocumentGenerator::writeParameters()
//-----------------------------------------------------------------------------
void GeneralDocumentGenerator::writeParameters(QTextStream& stream, QString const& tableTooltip,
    QString const& parameterTabs, QSharedPointer<QList<QSharedPointer<Parameter> > > parameters,
    ExpressionFormatter* formatter)
{
    QStringList paramHeaders;
    paramHeaders.append(QStringLiteral("Name"));
    paramHeaders.append(QStringLiteral("Type"));
    paramHeaders.append(QStringLiteral("Value"));
    paramHeaders.append(QStringLiteral("Resolve"));
    paramHeaders.append(QStringLiteral("Bit vector left"));
    paramHeaders.append(QStringLiteral("Bit vector right"));
    paramHeaders.append(QStringLiteral("Array left"));
    paramHeaders.append(QStringLiteral("Array right"));
    paramHeaders.append(QStringLiteral("Description"));
    writeTableElement(paramHeaders, tableTooltip, stream, parameterTabs);

    QString tableTabs = parameterTabs + QStringLiteral("\t");
    QString tableRowTabs = tableTabs + QStringLiteral("\t");
    foreach (QSharedPointer<Parameter> singleParameter, *parameters)
    {
        stream << tableTabs << "<tr>" << endl;
        stream << tableRowTabs << "<td>" << singleParameter->name() << "</td>" << endl;
        stream << tableRowTabs << "<td>" << singleParameter->getType() << "</td>" << endl;
        stream << tableRowTabs << "<td>" << formatter->formatReferringExpression(singleParameter->getValue()) <<
            "</td>" << endl;
        stream << tableRowTabs << "<td>" << singleParameter->getValueResolve() << "</td>" << endl;
        stream << tableRowTabs << "<td>" <<
            formatter->formatReferringExpression(singleParameter->getVectorLeft()) << "</td>" << endl;
        stream << tableRowTabs << "<td>" <<
            formatter->formatReferringExpression(singleParameter->getVectorRight()) << "</td>" << endl;
        stream << tableRowTabs << "<td>" <<
            formatter->formatReferringExpression(singleParameter->getArrayLeft()) << "</td>" << endl;
        stream << tableRowTabs << "<td>" <<
            formatter->formatReferringExpression(singleParameter->getArrayRight()) << "</td>" << endl;
        stream << tableRowTabs << "<td>" << singleParameter->description() << "</td>" << endl;

        stream << tableTabs << "</tr>" << endl;
    }

    stream << parameterTabs << "</table>" << endl;
}

//-----------------------------------------------------------------------------
// Function: GeneralDocumentGenerator::writeFileBuildCommands()
//-----------------------------------------------------------------------------
void GeneralDocumentGenerator::writeFileBuildCommands(QTextStream& stream, QString const& buildCommandTabs,
    QSharedPointer<QList<QSharedPointer<FileBuilder> > > buildCommands,
    ExpressionFormatter* buildExpressionFormatter)
{
    if (!buildCommands || !buildCommands->isEmpty())
    {
        QStringList buildCommandHeaders;
        buildCommandHeaders.append("File type");
        buildCommandHeaders.append("Command");
        buildCommandHeaders.append("Flags");
        buildCommandHeaders.append("Replace default flags");
        writeTableElement(buildCommandHeaders, QString("Default file build commands"), stream,
            buildCommandTabs);

        QString builderTableTabs = buildCommandTabs + "\t";
        QString builderRowTabs = builderTableTabs + "\t";

        foreach (QSharedPointer<FileBuilder> builder, *buildCommands)
        {
            stream << builderTableTabs << "<tr>" << endl;
            stream << builderRowTabs << "<td>" << builder->getFileType() << "</td>" << endl;
            stream << builderRowTabs << "<td>" << builder->getCommand() << "</td>" << endl;
            stream << builderRowTabs << "<td>" << builder->getFlags() << "</td>" << endl;
            stream << builderRowTabs << "<td>" <<
                buildExpressionFormatter->formatReferringExpression(builder->getReplaceDefaultFlags()) <<
                "</td>" << endl;
            stream << builderTableTabs << "</tr>" << endl;
        }

        stream << buildCommandTabs << "</table>" << endl;
    }
}
