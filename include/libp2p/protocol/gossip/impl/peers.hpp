/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef LIBP2P_PROTOCOL_GOSSIP_PEERS_HPP
#define LIBP2P_PROTOCOL_GOSSIP_PEERS_HPP

#include <algorithm>
#include <set>

#include <libp2p/multi/multiaddress.hpp>

#include <libp2p/protocol/gossip/common.hpp>
#include <libp2p/protocol/gossip/impl/ptr_key_wrapper.hpp>

namespace libp2p::protocol::gossip {

  /// Builds outbound message for wire protocol
  class MessageBuilder;

  /// Data related to peer needed by pub-sub protocols
  struct PeerContext {
    /// Used by shared ptr only
    using Ptr = std::shared_ptr<PeerContext>;

    /// The key
    using key_type = PeerId;
    const PeerId peer_id;

    /// Set of topics this peer is subscribed to
    std::set<TopicId> subscribed_to;

    /// Builds message to be sent to this peer
    std::shared_ptr<MessageBuilder> message_to_send;

    /// Not null iff this peer can be dialed to
    boost::optional<multi::Multiaddress> dial_to;

    ~PeerContext() = default;
    PeerContext(PeerContext &&) = delete;
    PeerContext(const PeerContext &) = delete;
    PeerContext &operator=(const PeerContext &) = delete;
    PeerContext &operator=(PeerContext &&) = delete;

    explicit PeerContext(PeerId id) : peer_id(std::move(id)) {}

    bool operator<(const PeerContext &other) const {
      return less(peer_id, other.peer_id);
    }

    friend bool operator<(const PeerContext &ctx, const PeerId &peer) {
      return less(ctx.peer_id, peer);
    }

    friend bool operator<(const PeerId &peer, const PeerContext &ctx) {
      return less(peer, ctx.peer_id);
    }
  };

  /// Peer set for pub-sub protocols
  class PeerSet {
   public:
    PeerSet() = default;
    ~PeerSet() = default;
    PeerSet(const PeerSet &) = default;
    PeerSet(PeerSet &&) = default;
    PeerSet &operator=(const PeerSet &) = default;
    PeerSet &operator=(PeerSet &&) = default;

    /// Finds peer context by id. Returns empty ptr if not found
    PeerContext::Ptr find(const PeerId &id) const;

    /// Inserts peer context into set, returns false if already inserted
    bool insert(PeerContext::Ptr ctx);

    /// Erases peer context from set, returns false not found
    bool erase(const PeerId &id);

    /// Clears all data
    void clear();

    /// Returns true iff size() == 0
    bool empty() const;

    /// Returns # of peers in list
    size_t size() const;

    /// Selects up to n random peers
    std::vector<PeerContext::Ptr> selectRandomPeers(
        size_t n, UniformRandomGen &gen) const;

    /// Selects all peers
    template <typename Callback>
    void selectAll(const Callback &callback) const {
      for (const auto &p : peers_) {
        callback(p.ptr);
      }
    }

    /// Selects peers filtered by external criteria
    template <typename Callback, typename Predicate>
    void selectIf(const Callback &callback, const Predicate &filter) const {
      selectAll([&](const PeerContext::Ptr &p) {
        if (filter(p))
          callback(p);
      });
    }

    /// Conditionally erases peers from the set
    template <typename Predicate>
    void eraseIf(const Predicate &filter) {
      std::vector<PeerContext::Ptr> to_erase;
      selectIf(
          [&to_erase](const PeerContext::Ptr &p) { to_erase.emplace_back(p); },
          filter);
      if (to_erase.size() == size()) {
        clear();
      } else {
        for (const auto &p : to_erase) {
          erase(p->peer_id);
        }
      }
    }

   private:
    std::set<SharedPtrKeyWrapper<PeerContext>, std::less<>> peers_;
    mutable std::vector<PeerId> peer_ids_;
  };

}  // namespace libp2p::protocol::gossip

#endif  // LIBP2P_PROTOCOL_GOSSIP_PEERS_HPP