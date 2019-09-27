/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef LIBP2P_KAD_ROUTING_TABLE_MOCK_HPP
#define LIBP2P_KAD_ROUTING_TABLE_MOCK_HPP

#include "p2p/protocol/kademlia/routing_table.hpp"

#include <gmock/gmock.h>

namespace libp2p::protocol::kademlia {

  struct RoutingTableMock : public RoutingTable {
    ~RoutingTableMock() override = default;

    MOCK_METHOD1(update, outcome::result<void>(const peer::PeerId &));
    MOCK_METHOD1(remove, void(const peer::PeerId &id));
    MOCK_CONST_METHOD0(getAllPeers, PeerIdVec());
    MOCK_METHOD2(getNearestPeers, PeerIdVec(const NodeId &id, size_t count));
    MOCK_CONST_METHOD0(size, size_t());
  };

}  // namespace libp2p::protocol::kademlia

#endif  // LIBP2P_KAD_ROUTING_TABLE_MOCK_HPP