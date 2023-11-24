#ifndef OPCUAHELPER_H
#define OPCUAHELPER_H

#include <QString>

#include <QtOpcUa/qopcuatype.h>

class QOpcUaNode;

class QOpcUaHelper
{
public:
    static QString getRawDisplayName(QOpcUaNode *node);
    static QString getRawBrowseName(QOpcUaNode *node);
    static QString getAttributeValue(QOpcUaNode *node, QOpcUa::NodeAttribute attr);
};

#endif // OPCUAHELPER_H
