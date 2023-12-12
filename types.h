#ifndef TYPES_H
#define TYPES_H

#include <QObject>

namespace Types {

Q_NAMESPACE
enum class DashboardType { Unknown = -1, Variables, Events, Add };
Q_ENUM_NS(DashboardType)

} // namespace Types

Q_DECLARE_METATYPE(Types::DashboardType)

#endif // TYPES_H
