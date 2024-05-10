#!/usr/bin/env python3

import os
import sys
from PySide6.QtCore import QCoreApplication
from PySide6.QtDBus import QDBusConnection, QDBusInterface, QDBusReply


if __name__ == "__main__":
    if len(sys.argv) < 3:
        print(f"Usage: {os.path.basename(sys.argv[0])} KEY VALUE")
        sys.exit(1)

    key = sys.argv[1]
    value = sys.argv[2]

    app = QCoreApplication()
    session_bus = QDBusConnection.sessionBus()
    if not session_bus.isConnected():
        print("Failed to connect to D-Bus session bus")
        sys.exit(2)

    iface = QDBusInterface("org.privact.data", "/", "", session_bus)
    if not iface.isValid():
        print(session_bus.lastError().message())
        sys.exit(3)

    message = iface.call("submit_data_point", key, value)
    reply = QDBusReply(message)
    if not reply.isValid():
        error = reply.error().message()
        print(f"Call failed: {error}")
        sys.exit(-1)

    value = reply.value()
    print(f"Reply: {value}")
