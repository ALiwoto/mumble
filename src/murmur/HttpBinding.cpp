
#include "httplib.h"
#include "HttpBinding.h"

#include "ACL.h"
#include "Channel.h"
#include "ClientType.h"
#include "Connection.h"
#include "EnvUtils.h"
#include "Group.h"
#include "HTMLFilter.h"
#include "HostAddress.h"
#include "Meta.h"
#include "MumbleProtocol.h"
#include "ProtoUtils.h"
#include "QtUtils.h"
#include "ServerDB.h"
#include "ServerUser.h"
#include "User.h"
#include "Version.h"

#ifdef USE_ZEROCONF
#	include "Zeroconf.h"
#endif

#include "Utils.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QSet>
#include <QtCore/QXmlStreamAttributes>
#include <QtCore/QtEndian>
#include <QtNetwork/QHostInfo>
#include <QtNetwork/QSslConfiguration>

#include <boost/bind/bind.hpp>

#include "TracyConstants.h"
#include <tracy/Tracy.hpp>
#include <tracy/TracyC.h>

#include <algorithm>
#include <cassert>
#include <vector>
#include <nlohmann/json.hpp>

#ifdef Q_OS_WIN
#	include <qos2.h>
#	include <ws2tcpip.h>
#else
#	include <netinet/in.h>
#	include <poll.h>
#endif

#include "ServerDB.h"
#include "Server.h"
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>


void HttpBindingServer::handle_serverPing(const httplib::Request &req, httplib::Response &res) {
    if (req.get_header_value("X-Full-Ping") == "1") {
        this->send_json_result(res, {
            {"ping", "pong"},
            {"version", "1.2.3"},
            {"uptime", 123456}
        });
        return;
    }
    this->send_json_result(res, {
        {"ping", "pong"}
    });
}

void HttpBindingServer::handle_userLogin(const httplib::Request &req, httplib::Response &res) {
    nlohmann::json body = nlohmann::json::parse(req.body);
    if (body.is_null()) {
        this->send_invalid_data_error(res);
        return;
    }

    if (!body.contains("username") || !body.contains("password")) {
        this->send_invalid_username_pass(res);
        return;
    }

    QString username = QString::fromStdString(body["username"].get<std::string>());
    QString password = QString::fromStdString(body["password"].get<std::string>());
    std::cout << "Username: " << username.toStdString() << std::endl;
    std::cout << "Password: " << password.toStdString() << std::endl;

    int serverId = 1;
    if (body.contains("server_id"))
        serverId = body["server_id"].get<int>();
    
    Server *server = this->m_meta->qhServers[serverId];
    server->authenticate(username, password);
    std::cout << "Username: " << username.toStdString() << std::endl;
    std::cout << "Password: " << password.toStdString() << std::endl;
}

void HttpBindingServer::handle_userMe(const httplib::Request &req, httplib::Response &res) {
    this->send_json_result(res, {
        {"test", req.path}
    });
}
