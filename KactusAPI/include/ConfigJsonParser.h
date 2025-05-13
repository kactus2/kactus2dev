#ifndef CONFIGJSONPARSER_H
#define CONFIGJSONPARSER_H

#include <IPXACTmodels/common/Parameter.h>

#include <QtCore>
#include <QDir>

struct AddressPair {

    QString Start;
    QString End;

};

struct InitStruct {

    int Index;
    QString Name;
    int DataWidth;

};

struct TargetStruct {

    int Index;
    QString Name;
    int DataWidth;
    QList<AddressPair> AddressRegions;

};

// Currently used for CLI version
struct ConfigStruct {

    QString InterconVLNV;
    QString DesignVLNV;
    QString BusVLNV;
    QString ClkVLNV;
    QString RstVLNV;
    QString BusType;
    int AddressWidth;
    int IDWidth;
    int UserWidth;
    bool isChannel;
    QList<QSharedPointer<Parameter>> interconnectParams;
    QList<InitStruct> InitList;
    QList<TargetStruct> TargetList;

};

class ConfigJsonParser
{
public:

    //! The constructor
    ConfigJsonParser();

    //! The destructor
    ~ConfigJsonParser() = default;

    ConfigStruct* readFile();

    //! No copying. No assignment.
    ConfigJsonParser(const ConfigJsonParser& other) = delete;
    ConfigJsonParser& operator=(const ConfigJsonParser& other) = delete;

private:

    ConfigStruct config_;
    QString path_ = QDir::currentPath() + "/editors/InterconnectGenerator/config.json";
};

#endif // CONFIGJSONPARSER_H
