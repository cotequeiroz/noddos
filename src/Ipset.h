/*
   Copyright 2017 Steven Hessing

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

 * Ipset.h
 *
 *  Created on: Aug 9, 2017
 *      Author: Steven Hessing
 */

#ifndef IPSET_H_
#define IPSET_H_

#include <iostream>
#include <stdexcept>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <syslog.h>

#include <libipset/linux_ip_set.h>
#include <libipset/types.h>
#include <libipset/session.h>

#include "boost/asio.hpp"

#include "MacAddress.h"


std::string getIpsetUuid (const std::string inUuid);
std::string getIpsetName (const std::string inUuid, bool inSrc, bool inIpv4 = true);


class Ipset {
private:
    struct ipset_session *session ;
    std::string ipsetType;
    std::string ipsetName;
    bool isIpsetv4;
    bool Debug;

    bool ipset_exec(enum ipset_cmd cmd, const boost::asio::ip::address &inIpAddress, uint32_t timeout);
    bool ipset_exec(enum ipset_cmd cmd, const std::string Mac, uint32_t timeout);
    bool ipset_exec(enum ipset_cmd cmd);
    // bool ipset_exec(enum ipset_cmd cmd, const MacAddress &Mac, uint32_t timeout = 0);
    // bool ipsec_exec(enum ipset_cmd cmd, const boost::asio::ip::address &inIpAddress, uint32_t timeout);

public:
    Ipset (const bool inDebug = false): Debug{inDebug}, ipsetType{""}, ipsetName{""}, isIpsetv4{false} {
        session = nullptr;
        if (Debug == true) {
            syslog (LOG_DEBUG, "Ipset: new instance");
        }
    };

    Ipset(const std::string inIpsetName, std::string inIpsetType, bool inisIpsetv4, bool inDebug = false):
            ipsetName{inIpsetName}, ipsetType{inIpsetType}, isIpsetv4{inisIpsetv4}, Debug{inDebug} {
        if (Debug == true) {
            syslog (LOG_DEBUG, "Ipset: new instance");
        }
        Open(inIpsetName, inIpsetType, inisIpsetv4);
    }

    ~Ipset(void) {
        if (Debug == true) {
            syslog (LOG_DEBUG, "Ipset: deleting instance");
        }
        if (session != nullptr) {
            ipset_session_fini(session);
            session = nullptr;
        }
    }
    void Open (const std::string inIpsetName, std::string inIpsetType, bool inisIpsetv4, bool inDebug = false);

    bool Destroy() {
        return ipset_exec(IPSET_CMD_DESTROY);
    }
    bool Exists() {
         ipset_session_data_set(session, IPSET_SETNAME, ipsetName.c_str());
         return ipset_cmd(session, IPSET_CMD_HEADER, 0) == 0;
     }

     bool Add(const boost::asio::ip::address &inIpAddress, time_t timeout = 604800) {
         return ipset_exec(IPSET_CMD_ADD, inIpAddress, timeout);
     }

     bool Add(const MacAddress &inMac, time_t timeout = 7776000) {
         return ipset_exec(IPSET_CMD_ADD, inMac.str(), timeout);
     }

     bool Remove(const boost::asio::ip::address &inIpAddress) {
         return ipset_exec(IPSET_CMD_DEL, inIpAddress, 0);
     }
     bool Remove(const MacAddress &Mac) {
         return ipset_exec(IPSET_CMD_DEL, Mac.str(), 0);
     }
     bool Remove(const std::string &Mac) {
          return ipset_exec(IPSET_CMD_DEL, Mac, 0);
     }

     bool In(const boost::asio::ip::address &inIpAddress) {
         return ipset_exec(IPSET_CMD_TEST, inIpAddress, 0);
     }
     bool In(const MacAddress &Mac) {
         return ipset_exec(IPSET_CMD_TEST, Mac.str(), 0);
     }

};

#endif /* IPSET_H_ */