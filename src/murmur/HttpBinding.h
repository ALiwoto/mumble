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

#include <nlohmann/json.hpp>
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

class QSqlDatabase;
class Meta;

const std::string V1_API_PREFIX = "/api/v1";
#define API_V1_HANDLER(path, handler) \
    V1_API_PREFIX + path, [&](const httplib::Request& req, httplib::Response& res) { \
        try { \
            handler(req, res); \
        } catch (const std::exception& e) { \
            this->send_internal_error(res, e); \
        } \
    }



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

        server->Get(API_V1_HANDLER("/server/ping", handle_serverPing));

        server->Post(API_V1_HANDLER("/user/login", handle_userLogin));

        server->Get(API_V1_HANDLER("/user/me", handle_userMe));
    }

    void set_meta(Meta *meta) {
        this->m_meta = meta;
    }

    void send_json_data(httplib::Response& res, const nlohmann::json& result) {
        res.set_content(result.dump(), "application/json");
    }

    void send_json_result(httplib::Response& res, const nlohmann::json& result) {
        nlohmann::json result_json = {
            {"success", true},
            {"result", result},
            {"error", nullptr},
        };
        res.set_content(result_json.dump(), "application/json");
    }

    void send_internal_error(httplib::Response& res, const std::exception& e) {
        this->send_json_data(res, {
                    {"success", false},
                    {"error", {
                            {"code", 500},
                            {"message", e.what()},
                        },
                    },
                });
    }

    void send_invalid_data_error(httplib::Response& res) {
        this->send_json_data(res, {
                    {"success", false},
                    {"error", {
                            {"code", 400},
                            {"message", "Invalid data"},
                        },
                    },
                });
    }

    void send_invalid_username_pass(httplib::Response& res) {
        this->send_json_data(res, {
                    {"success", false},
                    {"error", {
                            {"code", 400},
                            {"message", "Invalid username or password"},
                        },
                    },
                });
    }

    ~HttpBindingServer() {
        delete server;
    }



    // server/ping
    void handle_serverPing(const httplib::Request& req, httplib::Response& res);

    // user/login
    void handle_userLogin(const httplib::Request& req, httplib::Response& res);

    // user/me
    void handle_userMe(const httplib::Request& req, httplib::Response& res);

private:
    void run_server() {
        server->listen(this->hostAddr, this->hostPort);
    }

private:
    httplib::Server *server;
    std::string hostAddr = "0.0.0.0";
    int hostPort = 8080;
    Meta *m_meta;
};

#endif

