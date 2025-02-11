// Copyright (c) 2010-2021 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

// IWYU pragma: private
// IWYU pragma: friend ".*src/api/client/blockchain/PaymentCode.cpp"

#pragma once

#include <atomic>
#include <cstdint>
#include <functional>
#include <list>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "api/client/blockchain/BalanceNode.hpp"
#include "api/client/blockchain/Deterministic.hpp"
#include "internal/api/client/blockchain/Blockchain.hpp"
#include "opentxs/Proto.hpp"
#include "opentxs/Types.hpp"
#include "opentxs/Version.hpp"
#include "opentxs/api/client/blockchain/PaymentCode.hpp"
#include "opentxs/api/client/blockchain/Subchain.hpp"
#include "opentxs/api/client/blockchain/Types.hpp"
#include "opentxs/blockchain/Blockchain.hpp"
#include "opentxs/blockchain/BlockchainType.hpp"
#include "opentxs/blockchain/Types.hpp"
#include "opentxs/core/Data.hpp"
#include "opentxs/core/Identifier.hpp"
#include "opentxs/core/crypto/PaymentCode.hpp"
#include "opentxs/core/identifier/Nym.hpp"
#include "opentxs/crypto/Types.hpp"
#include "opentxs/protobuf/Bip47Channel.pb.h"
#include "opentxs/protobuf/HDPath.pb.h"
#include "util/Latest.hpp"

namespace opentxs
{
namespace api
{
namespace client
{
namespace internal
{
struct Blockchain;
}  // namespace internal
}  // namespace client

namespace internal
{
struct Core;
}  // namespace internal
}  // namespace api

namespace proto
{
class Bip47Channel;
class HDPath;
}  // namespace proto

class PasswordPrompt;
}  // namespace opentxs

namespace opentxs::api::client::blockchain::implementation
{
class PaymentCode final : public internal::PaymentCode, public Deterministic
{
public:
    using Element = implementation::BalanceNode::Element;
    using SerializedType = proto::Bip47Channel;

    auto AddNotification(const Txid& tx) const noexcept -> bool final;
    auto IsNotified() const noexcept -> bool final;
    auto Local() const noexcept -> const opentxs::PaymentCode& final
    {
        return local_;
    }
    auto ReorgNotification(const Txid& tx) const noexcept -> bool final;
    auto Remote() const noexcept -> const opentxs::PaymentCode& final
    {
        return remote_;
    }
    auto PrivateKey(
        const Subchain type,
        const Bip32Index index,
        const PasswordPrompt& reason) const noexcept -> ECKey final;
#if OT_CRYPTO_WITH_BIP32
    auto RootNode(const PasswordPrompt& reason) const noexcept -> HDKey final
    {
        return local_.get().Key();
    }
#endif  // OT_CRYPTO_WITH_BIP32

    PaymentCode(
        const api::internal::Core& api,
        const internal::BalanceTree& parent,
        const opentxs::PaymentCode& local,
        const opentxs::PaymentCode& remote,
        const proto::HDPath& path,
        const opentxs::blockchain::block::Txid& txid,
        const PasswordPrompt& reason,
        Identifier& id) noexcept(false);
    PaymentCode(
        const api::internal::Core& api,
        const internal::BalanceTree& parent,
        const SerializedType& serialized,
        Identifier& id) noexcept(false);

    ~PaymentCode() final = default;

private:
    static constexpr auto DefaultVersion = VersionNumber{1};
    static constexpr auto Bip47DirectionVersion = VersionNumber{1};
    static constexpr auto compare_ = [](const opentxs::PaymentCode& lhs,
                                        const opentxs::PaymentCode& rhs) {
        return lhs.ID() == rhs.ID();
    };

    using Compare = std::function<
        void(const opentxs::PaymentCode&, const opentxs::PaymentCode&)>;
    using Latest = LatestVersion<OTPaymentCode, opentxs::PaymentCode, Compare>;

    VersionNumber version_;
    mutable std::set<opentxs::blockchain::block::pTxid> outgoing_notifications_;
    mutable std::set<opentxs::blockchain::block::pTxid> incoming_notifications_;
    mutable Latest local_;
    Latest remote_;

    auto account_already_exists(const Lock& lock) const noexcept -> bool final;
    auto get_contact() const noexcept -> OTIdentifier;
    auto has_private(const PasswordPrompt& reason) const noexcept -> bool;
    auto save(const Lock& lock) const noexcept -> bool final;
    auto set_deterministic_contact(Element& element) const noexcept
        -> void final
    {
        element.SetContact(get_contact());
    }
    auto set_deterministic_contact(
        std::set<OTIdentifier>& contacts) const noexcept -> void final
    {
        contacts.emplace(get_contact());
    }

    PaymentCode(const PaymentCode&) = delete;
    PaymentCode(PaymentCode&&) = delete;
    auto operator=(const PaymentCode&) -> PaymentCode& = delete;
    auto operator=(PaymentCode&&) -> PaymentCode& = delete;
};
}  // namespace opentxs::api::client::blockchain::implementation
