#ifndef BATCHEVENT_H
#define BATCHEVENT_H

#include <QVariant>

struct BatchEvent
{
    uint32_t type;
    QVariant data;
};

struct DBusForwardKey
{
    uint32_t keysym;
    uint32_t keystate;
    bool isRelease;
};

struct PreeditKeyData
{
    QString text;
    int32_t format;
};

struct PreeditKey
{
    QList<PreeditKeyData> info;
    int32_t cursor;
};

Q_DECLARE_METATYPE(BatchEvent)
Q_DECLARE_METATYPE(DBusForwardKey)
Q_DECLARE_METATYPE(PreeditKeyData)
Q_DECLARE_METATYPE(PreeditKey)

#endif // !BATCHEVENT_H
