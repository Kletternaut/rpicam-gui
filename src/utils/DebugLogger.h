#ifndef DEBUGLOGGER_H
#define DEBUGLOGGER_H

#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QtGlobal>

class DebugLogger {
public:
    static void initialize(const QString &filePath);
    static void shutdown();
    static void customMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);

private:
    static QFile debugLogFile;
};

#endif // DEBUGLOGGER_H