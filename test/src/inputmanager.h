//
// Created by lasse on 28.09.15.
//

#ifndef ICARUS_INPUTMANAGER_H
#define ICARUS_INPUTMANAGER_H

#include <QObject>
#include <QTimer>
#include <QCoreApplication>

#include <gamepad.h>

#include <radio/i_radio.h>
#include <radio/commands/armcommand.h>
#include <radio/commands/disarmcommand.h>
#include <radio/commands/controlcommand.h>
#include <radio/commands/suspendcoptercommand.h>
#include <radio/commands/bindtransmittercommand.h>
#include <radio/commands/togglesuspensioncommand.h>
#include <radio/commands/toggletransmittercommand.h>

class InputManager: public QObject
{
Q_OBJECT
public:

    InputManager(int source, int copterId, IRadio *radio);

    void start();

    void stop();

private slots:

    void update();

private:

    int source;
    int copterId;

    QTimer timer;

    Gamepad gamepad;

    IRadio *radio;

};


#endif //ICARUS_INPUTMANAGER_H
