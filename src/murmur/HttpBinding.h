#ifndef MUMBLE_MURMUR_HTTP_BINDING_H_
#define MUMBLE_MURMUR_HTTP_BINDING_H_

#include <QtCore/QtGlobal>

#ifdef Q_OS_WIN
#	include "win.h"
#endif

#include "ACL.h"
#include "AudioReceiverBuffer.h"
#include "Ban.h"
#include "ChannelListenerManager.h"
#include "HostAddress.h"
#include "Mumble.pb.h"
#include "MumbleProtocol.h"
#include "Timer.h"
#include "User.h"
#include "Version.h"
#include "VolumeAdjustment.h"

#ifndef Q_MOC_RUN
#	include <boost/function.hpp>
#endif

#include <QtCore/QEvent>
#include <QtCore/QMutex>
#include <QtCore/QQueue>
#include <QtCore/QReadWriteLock>
#include <QtCore/QSocketNotifier>
#include <QtCore/QStringList>
#include <QtCore/QThread>
#include <QtCore/QTimer>
#include <QtCore/QUrl>
#include <QtNetwork/QSslCertificate>
#include <QtNetwork/QSslKey>
#include <QtNetwork/QSslSocket>
#include <QtNetwork/QTcpServer>
#if defined(USE_QSSLDIFFIEHELLMANPARAMETERS)
#	include <QtNetwork/QSslDiffieHellmanParameters>
#endif

#ifdef Q_OS_WIN
#	include <winsock2.h>
#endif
#include "httplib.h"

const std::string V1_API_PREFIX = "/api/v1";


class HttpBindingServer : public QThread {
    Q_OBJECT

protected:
    void run() override {
        run_server();
    }

public:
    HttpBindingServer(std::string host, int port) {
        server = new httplib::Server();
        this->hostAddr = host;
        this->hostPort = port;

        server->Get(V1_API_PREFIX + "/ping", [&](const httplib::Request& req, httplib::Response& res) {
            handle_PingRequest(req, res);
        });


        server->Get("/hi", [](const httplib::Request& req, httplib::Response& res) {
            // print reqest's path
            std::cout << req.path << std::endl;
            res.set_content("Hello World!", "text/plain");
        });
    }

    ~HttpBindingServer() {
        delete server;
    }



    // Handles a request to the /ping endpoint.
    void handle_PingRequest(const httplib::Request& req, httplib::Response& res);

private:
    void run_server() {
        server->listen(this->hostAddr, this->hostPort);
    }

private:
    httplib::Server *server;
    std::string hostAddr = "0.0.0.0";
    int hostPort = 8080;
};

#endif

