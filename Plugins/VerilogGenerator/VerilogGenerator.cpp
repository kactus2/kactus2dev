//-----------------------------------------------------------------------------
// File: VerilogGenerator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 28.7.2014
//
// Description:
// Verilog file generator.
//-----------------------------------------------------------------------------

#include "VerilogGenerator.h"

#include <kactusGenerators/HDLGenerator/vhdlportsorter.h>
#include <kactusGenerators/HDLGenerator/HDLUtils.h>

#include <QDate>

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::VerilogGenerator()
//-----------------------------------------------------------------------------
VerilogGenerator::VerilogGenerator(): headerWriter_(0), topWriter_(0), topComponent_()
{

}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::~VerilogGenerator()
//-----------------------------------------------------------------------------
VerilogGenerator::~VerilogGenerator()
{
    delete headerWriter_;
    headerWriter_ = 0;

    delete topWriter_;
    topWriter_ = 0;
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::generate()
//-----------------------------------------------------------------------------
void VerilogGenerator::generate(QString const& outputPath) const
{
    if (nothingToWrite())
    {
        return;
    }

    QFile outputFile(outputPath); 
    if (!outputFile.open(QIODevice::WriteOnly))
    {
        return;
    }

    QTextStream outputStream(&outputFile);

    QString fileName = outputPath.mid(outputPath.lastIndexOf("/") + 1);

    headerWriter_->write(outputStream, fileName, topComponent_->getDescription(), QDateTime::currentDateTime());
    topWriter_->write(outputStream);

    outputFile.close();
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::parse()
//-----------------------------------------------------------------------------
void VerilogGenerator::parse(QSharedPointer<const Component> component)
{
    topComponent_ = component;

    QString mockUsername = "user";
    QString mockPath = "C:/Test/TestLibrary/TestComponent/1.0/TestComponent.1.0.xml";

    delete headerWriter_;
    headerWriter_ = new VerilogHeaderWriter(*topComponent_->getVlnv(), mockPath, mockUsername);

    delete topWriter_;
    topWriter_ = new ComponentVerilogWriter(topComponent_);
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::nothingToWrite()
//-----------------------------------------------------------------------------
bool VerilogGenerator::nothingToWrite() const
{
    return topWriter_ == 0;
}
