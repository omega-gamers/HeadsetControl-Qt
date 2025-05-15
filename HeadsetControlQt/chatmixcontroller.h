#include "chatmixcontroller.h"
#include <QProcess>
#include <QStringList>
#include <QDebug>

ChatMixController::ChatMixController(QObject *parent) : QObject(parent) {}

int ChatMixController::getChatMixValue()
{
    QProcess process;
    process.start("headsetcontrol", QStringList() << "-m" << "-o" << "ENV");
    process.waitForFinished();

    QString output = process.readAllStandardOutput();
    QStringList lines = output.split('\n');

    for (const QString &line : lines) {
        if (line.contains("Chatmix")) {
            QStringList parts = line.split(":");
            if (parts.size() == 2) {
                return parts[1].trimmed().toInt();
            }
        }
    }

    return -1; // Error or not found
}

bool ChatMixController::setChatMixValue(int value)
{
    if (value < 0 || value > 128)
        return false;

    QProcess process;
    process.start("headsetcontrol", QStringList() << "-m" << "-s" << QString::number(value));
    bool success = process.waitForFinished();

    if (success) {
        adjustSinkVolumes(value);
    }

    return success;
}

void ChatMixController::adjustSinkVolumes(int chatmixValue)
{
    // Normalize to 0.0 - 1.0 range
    double chatRatio = 1.0;
    double gameRatio = 1.0;

    if (chatmixValue < 64) {
        chatRatio = 1.0;
        gameRatio = static_cast<double>(chatmixValue) / 64.0;
    } else if (chatmixValue > 64) {
        gameRatio = 1.0;
        chatRatio = static_cast<double>(128 - chatmixValue) / 64.0;
    }

    int chatVolume = static_cast<int>(chatRatio * 65536); // PulseAudio volume scale
    int gameVolume = static_cast<int>(gameRatio * 65536);

    QString chatCommand = QString("pactl set-sink-volume ChatSink %1").arg(chatVolume);
    QString gameCommand = QString("pactl set-sink-volume GameSink %1").arg(gameVolume);

    QProcess::execute(chatCommand);
    QProcess::execute(gameCommand);
}
