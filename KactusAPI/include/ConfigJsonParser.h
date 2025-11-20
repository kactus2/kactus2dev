#ifndef CONFIGJSONPARSER_H
#define CONFIGJSONPARSER_H

#include <IPXACTmodels/common/Parameter.h>

#include <kactusAPI/KactusAPIGlobal.h>

#include <QtCore>
#include <QDir>

namespace InterconnectGeneration
{
    struct AddressPair
    {

        QString start;
        QString end;

    };

    struct InitStruct
    {

        int index;
        QString name;

    };

    struct TargetStruct
    {

        int index;
        QString name;
        QList<AddressPair> addressRegions;

    };

    enum class BusType {
        AXI4,
        AXI4LITE,
        OBI,
        UNKNOWN
    };

    // Currently used for CLI version
    struct ConfigStruct
    {

        QString interconVLNV;
        QString designVLNV;
        QString busVLNV;
        QString clkVLNV;
        QString rstVLNV;
        BusType busType;
        int addressWidth;
        int dataWidth;
        int idWidth;
        int userWidth;
        bool isChannel;
        QString addressWidthParamName;
        QString dataWidthParamName;
        QList<QSharedPointer<Parameter>> interconnectParams;
        QList<InitStruct> initList;
        QList<TargetStruct> targetList;

    };

    class KACTUS2_API ConfigJsonParser
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
        
        static BusType strToBusType(QString const& busTypeStr);
        static QString busTypeToStr(BusType busType);

    private:

        ConfigStruct config_;
        QString path_ = QDir::currentPath() + "/KactusAPI/include/config.json";
    };

}

#endif // CONFIGJSONPARSER_H
