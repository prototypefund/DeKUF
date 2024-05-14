#include <QtDBus>
#include <core/storage.hpp>

#include "dbus_service.hpp"

detail::Data::Data(QSharedPointer<Storage> storage)
    : storage(storage)
{
}

QString detail::Data::submit_data_point(
    const QString& key, const QString& value)
{
    qDebug() << "Received data point:" << key << "=" << value;
    storage->addDataPoint(key, value);
    return "OK";
}

DBusService::DBusService(QSharedPointer<Storage> storage)
    : data(storage)
{
    auto connection = QDBusConnection::sessionBus();

    if (!connection.isConnected()) {
        qDebug() << "DBusService: Failed to connect to D-Bus session bus";
        return;
    }
    if (!connection.registerService("org.privact.data"))
        throw QString("DBusService: ")
            + qPrintable(connection.lastError().message());

    connection.registerObject("/", &data, QDBusConnection::ExportAllSlots);
}
