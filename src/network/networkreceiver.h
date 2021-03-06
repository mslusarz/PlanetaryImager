/*
 * GuLinux Planetary Imager - https://github.com/GuLinux/PlanetaryImager
 * Copyright (C) 2019  Marco Gulino <marco@gulinux.net>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef NETWORKRECEIVER_H
#define NETWORKRECEIVER_H

#include <functional>
#include <QObject>
#include "c++/dptr.h"
#include "commons/fwd.h"
#include "network/protocol/networkpackettype.h"

FWD_PTR(NetworkDispatcher)
FWD_PTR(NetworkReceiver)
FWD_PTR(NetworkPacket)

class NetworkReceiver {
public:
  NetworkReceiver(const NetworkDispatcherPtr &dispatcher);
  void handle(const NetworkPacketPtr &packet);
  virtual ~NetworkReceiver();
protected:
  typedef std::function<void(const NetworkPacketPtr &)> HandlePacket;
  void register_handler(const NetworkPacketType &name, const HandlePacket handler);
  void wait_for_processed(const NetworkPacketType &name) const;
  NetworkDispatcherPtr dispatcher() const;
private:
  DPTR
};

#endif // NETWORKRECEIVER_H
