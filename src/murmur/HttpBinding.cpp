
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

#ifdef Q_OS_WIN
#	include <qos2.h>
#	include <ws2tcpip.h>
#else
#	include <netinet/in.h>
#	include <poll.h>
#endif

void HttpBindingServer::handle_PingRequest(const httplib::Request &req, httplib::Response &res) {
    std:: cout << req.path << std::endl;
    res.set_content("Pong", "text/plain");
}