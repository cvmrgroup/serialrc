
#include <QTimer>
#include <QCoreApplication>

#include <radiomanager.h>

#include "inputmanager.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);


    RadioConfiguration conf;
    conf.txId = 1;
    conf.copterId = 0;
    conf.sender = "crazy";

    std::vector<RadioConfiguration> confs;
    confs.push_back(conf);

    RadioManager manager;
    manager.initialize(confs);

    try {
        manager.start();
    } catch (std::exception &e){
        BOOST_LOG_TRIVIAL(error) << e.what();
    }

    InputManager inputManager(0, conf.copterId, &manager);
    inputManager.start();

    int exitCode = app.exec();

    inputManager.stop();
    manager.stop();

    BOOST_LOG_TRIVIAL(info) << "goodbye";

    return exitCode;
}