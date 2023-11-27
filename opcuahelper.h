#ifndef OPCUAHELPER_H
#define OPCUAHELPER_H

#include <QString>

#include <QtOpcUa/qopcuatype.h>

class QOpcUaNode;

class QOpcUaHelper
{
public:
    static QString getRawAttributeValue(QOpcUaNode *node, QOpcUa::NodeAttribute attr);
    static QString getFormattedAttributeValue(QOpcUaNode *node, QOpcUa::NodeAttribute attr);
};

#endif // OPCUAHELPER_H
