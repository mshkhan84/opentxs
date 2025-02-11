// Copyright (c) 2010-2021 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "0_stdafx.hpp"    // IWYU pragma: associated
#include "1_Internal.hpp"  // IWYU pragma: associated
#include "blockchain/client/wallet/DeterministicStateData.hpp"  // IWYU pragma: associated

#include <cstdint>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "internal/blockchain/client/Client.hpp"
#include "opentxs/Bytes.hpp"
#include "opentxs/Pimpl.hpp"
#include "opentxs/api/client/blockchain/BalanceNode.hpp"
#include "opentxs/api/client/blockchain/Deterministic.hpp"
#include "opentxs/api/client/blockchain/Subchain.hpp"  // IWYU pragma: keep
#include "opentxs/blockchain/FilterType.hpp"
#include "opentxs/blockchain/block/bitcoin/Block.hpp"
#include "opentxs/blockchain/block/bitcoin/Input.hpp"
#include "opentxs/blockchain/block/bitcoin/Inputs.hpp"
#include "opentxs/blockchain/block/bitcoin/Output.hpp"
#include "opentxs/blockchain/block/bitcoin/Outputs.hpp"
#include "opentxs/blockchain/block/bitcoin/Script.hpp"
#include "opentxs/blockchain/block/bitcoin/Transaction.hpp"
#include "opentxs/core/Data.hpp"
#include "opentxs/core/Identifier.hpp"
#include "opentxs/core/Log.hpp"
#include "opentxs/core/LogSource.hpp"
#include "opentxs/crypto/Types.hpp"
#include "opentxs/crypto/key/EllipticCurve.hpp"
#include "opentxs/protobuf/BlockchainTransactionOutput.pb.h"  // IWYU pragma: keep
#include "util/ScopeGuard.hpp"

#define OT_METHOD                                                              \
    "opentxs::blockchain::client::wallet::DeterministicStateData::"

namespace opentxs::blockchain::client::wallet
{
DeterministicStateData::DeterministicStateData(
    const api::Core& api,
    const api::client::Blockchain& blockchain,
    const internal::Network& network,
    const WalletDatabase& db,
    const api::client::blockchain::Deterministic& node,
    const SimpleCallback& taskFinished,
    Outstanding& jobCounter,
    const zmq::socket::Push& threadPool,
    const filter::Type filter,
    const Subchain subchain) noexcept
    : SubchainStateData(
          api,
          blockchain,
          network,
          db,
          OTIdentifier{node.ID()},
          taskFinished,
          jobCounter,
          threadPool,
          filter,
          subchain)
    , node_(node)
{
}

auto DeterministicStateData::check_index() noexcept -> bool
{
    last_indexed_ = db_.SubchainLastIndexed(id_, subchain_, filter_type_);
    const auto generated = node_.LastGenerated(subchain_);

    if (generated.has_value()) {
        if ((false == last_indexed_.has_value()) ||
            (last_indexed_.value() != generated.value())) {
            LogVerbose(OT_METHOD)(__FUNCTION__)(": ")(id_)(" has ")(
                generated.value() + 1)(" keys generated, but only ")(
                last_indexed_.value_or(0))(" have been indexed.")
                .Flush();
            static constexpr auto job{"index"};

            return queue_work(Task::index, job);
        } else {
            LogTrace(OT_METHOD)(__FUNCTION__)(": ")(id_)(" all ")(
                generated.value() + 1)(" generated keys have been indexed.")
                .Flush();
        }
    } else {
        LogVerbose(OT_METHOD)(__FUNCTION__)(": ")(id_)(
            " no generated keys present")
            .Flush();
    }

    return false;
}

auto DeterministicStateData::handle_confirmed_matches(
    const block::bitcoin::Block& block,
    const block::Position& position,
    const block::Block::Matches& confirmed) noexcept -> void
{
    auto transactions = std::map<
        block::pTxid,
        std::pair<
            std::vector<Bip32Index>,
            const block::bitcoin::Transaction*>>{};

    for (const auto& match : confirmed) {
        const auto& [txid, elementID] = match;
        const auto& [index, subchainID] = elementID;
        const auto& [subchain, accountID] = subchainID;
        const auto& element = node_.BalanceElement(subchain, index);
        const auto& pTransaction = block.at(txid->Bytes());

        OT_ASSERT(pTransaction);

        auto& arg = transactions[txid];
        auto& [outputs, pTX] = arg;
        auto postcondition = ScopeGuard{[&] {
            if (nullptr == arg.second) { transactions.erase(match.first); }
        }};
        const auto& transaction = *pTransaction;
        auto i = Bip32Index{0};

        for (const auto& output : transaction.Outputs()) {
            if (Subchain::Outgoing == subchain_) { continue; }

            const auto& script = output.Script();

            switch (script.Type()) {
                case block::bitcoin::Script::Pattern::PayToPubkey: {
                    const auto pKey = element.Key();

                    OT_ASSERT(pKey);
                    OT_ASSERT(script.Pubkey().has_value());

                    const auto& key = *pKey;

                    if (key.PublicKey() == script.Pubkey().value()) {
                        outputs.emplace_back(i);

                        if (nullptr == pTX) { pTX = pTransaction.get(); }
                    }

                    // TODO mark key as used
                } break;
                case block::bitcoin::Script::Pattern::PayToPubkeyHash: {
                    const auto hash = element.PubkeyHash();

                    OT_ASSERT(script.PubkeyHash().has_value());

                    if (hash->Bytes() == script.PubkeyHash().value()) {
                        outputs.emplace_back(i);

                        if (nullptr == pTX) { pTX = pTransaction.get(); }
                    }

                    // TODO mark key as used
                } break;
                case block::bitcoin::Script::Pattern::PayToMultisig: {
                    const auto m = script.M();
                    const auto n = script.N();

                    OT_ASSERT(m.has_value());
                    OT_ASSERT(n.has_value());

                    if (1u != m.value() || (3u != n.value())) {
                        // TODO handle non-payment code multisig eventually

                        continue;
                    }

                    const auto pKey = element.Key();

                    OT_ASSERT(pKey);

                    const auto& key = *pKey;

                    if (key.PublicKey() == script.MultisigPubkey(0).value()) {
                        outputs.emplace_back(i);

                        if (nullptr == pTX) { pTX = pTransaction.get(); }
                    }

                    // TODO mark key as used
                } break;
                case block::bitcoin::Script::Pattern::PayToScriptHash:
                default: {
                }
            };

            ++i;
        }
    }

    auto unspent = std::set<blockchain::block::bitcoin::Outpoint>{};

    for (const auto& utxo : db_.GetUnspentOutputs()) {
        unspent.insert(utxo.first);
    }

    for (const auto& transaction : block) {
        OT_ASSERT(transaction);

        for (const auto& input : transaction->Inputs()) {
            if (0 < unspent.count(input.PreviousOutput())) {
                auto& pTx = transactions[transaction->ID()].second;

                if (nullptr == pTx) { pTx = transaction.get(); }
            }
        }
    }

    for (const auto& [txid, data] : transactions) {
        auto& [outputs, pTX] = data;

        OT_ASSERT(nullptr != pTX);

        auto updated = db_.AddConfirmedTransaction(
            network_.Chain(),
            id_,
            subchain_,
            filter_type_,
            position,
            outputs,
            *pTX);

        OT_ASSERT(updated);  // TODO handle database errors
    }
}

auto DeterministicStateData::index() noexcept -> void
{
    const auto first =
        last_indexed_.has_value() ? last_indexed_.value() + 1u : Bip32Index{0u};
    const auto last = node_.LastGenerated(subchain_).value_or(0u);
    auto elements = WalletDatabase::ElementMap{};

    if (last > first) {
        LogVerbose(OT_METHOD)(__FUNCTION__)(": ")(id_)(
            " indexing elements from ")(first)(" to ")(last)
            .Flush();
    } else {
        LogVerbose(OT_METHOD)(__FUNCTION__)(": ")(id_)(
            " subchain is fully indexed to item ")(last)
            .Flush();
    }

    for (auto i{first}; i <= last; ++i) {
        const auto& element = node_.BalanceElement(subchain_, i);
        index_element(filter_type_, element, i, elements);
    }

    db_.SubchainAddElements(id_, subchain_, filter_type_, elements);
}
}  // namespace opentxs::blockchain::client::wallet
