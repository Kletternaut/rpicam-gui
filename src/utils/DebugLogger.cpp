#include <QCoreApplication> // Füge diesen Include hinzu
#include <QDebug>
#include "DebugLogger.h"

QFile DebugLogger::debugLogFile;

void DebugLogger::initialize(const QString &filePath) {
    // Setze den Pfad auf das Hauptverzeichnis rpicam-gui
    QString fullPath = QCoreApplication::applicationDirPath() + "/../debug.log";
    debugLogFile.setFileName(fullPath);

    if (debugLogFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        debugLogFile.resize(0); // Datei leeren
        qInstallMessageHandler(customMessageHandler); // Debug-Handler installieren
    } else {
        qDebug() << "Fehler beim Öffnen der Datei" << fullPath; // Debug-Ausgabe
    }
}

void DebugLogger::shutdown() {
    if (debugLogFile.isOpen()) {
        debugLogFile.close();
    }
    qInstallMessageHandler(nullptr); // Standard-Handler wiederherstellen
}

void DebugLogger::customMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
    if (debugLogFile.isOpen()) {
        QTextStream out(&debugLogFile);
        QString timeStamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
        QString logType;

        switch (type) {
        case QtDebugMsg:
            logType = "DEBUG";
            break;
        case QtInfoMsg:
            logType = "INFO";
            break;
        case QtWarningMsg:
            logType = "WARNING";
            break;
        case QtCriticalMsg:
            logType = "CRITICAL";
            break;
        case QtFatalMsg:
            logType = "FATAL";
            break;
        }

        out << "[" << timeStamp << "] [" << logType << "] " << msg << "\n";
        out.flush();
    }
}