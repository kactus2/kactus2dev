#ifndef INTERCONNECTRTLWRITER_H
#define INTERCONNECTRTLWRITER_H

#include "ConfigJsonParser.h"

#include <QtCore>

#include <IPXACTmodels/Component/Component.h>

#include <KactusAPI/include/LibraryInterface.h>
#include <KactusAPI/include/MessageMediator.h>

class InterconnectRTLWriter
{
public:
    InterconnectRTLWriter(QSharedPointer<Component> component, LibraryInterface* library,
                          MessageMediator* messager, QString directory, ConfigJsonParser::ConfigStruct* config);

    ~InterconnectRTLWriter() = default;

    void generateRTL();

    void writeBus(QTextStream& stream, QString type);

    //! No copying. No assignment.
    InterconnectRTLWriter(const InterconnectRTLWriter& other) = delete;
    InterconnectRTLWriter& operator=(const InterconnectRTLWriter& other) = delete;

private:

    QSharedPointer<Component> component_{nullptr};

    MessageMediator* messager_{ nullptr };

    LibraryInterface* library_{ nullptr };

    ConfigJsonParser::ConfigStruct* config_;

    QString directory_;
};

#endif // INTERCONNECTRTLWRITER_H
