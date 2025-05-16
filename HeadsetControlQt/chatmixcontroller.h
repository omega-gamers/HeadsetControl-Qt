#ifndef CHATMIXCONTROLLER_H
#define CHATMIXCONTROLLER_H

#include <QObject>

class ChatMixController : public QObject
{
    Q_OBJECT

public:
    explicit ChatMixController(QObject *parent = nullptr);
    int getChatMixValue();
    bool setChatMixValue(int value);
    void adjustSinkVolumes(int chatmixValue);
};

#endif // CHATMIXCONTROLLER_H
