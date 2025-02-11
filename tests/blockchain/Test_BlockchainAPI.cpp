// Copyright (c) 2010-2021 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <gtest/gtest.h>
#include <cstdint>
#include <iostream>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <stdexcept>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

#include "OTTestEnvironment.hpp"  // IWYU pragma: keep
#include "opentxs/OT.hpp"
#include "opentxs/Pimpl.hpp"
#include "opentxs/Types.hpp"
#include "opentxs/Version.hpp"
#include "opentxs/api/Context.hpp"
#include "opentxs/api/Factory.hpp"
#include "opentxs/api/HDSeed.hpp"
#include "opentxs/api/Wallet.hpp"
#include "opentxs/api/client/Blockchain.hpp"
#include "opentxs/api/client/Contacts.hpp"
#include "opentxs/api/client/Manager.hpp"
#include "opentxs/api/client/blockchain/AddressStyle.hpp"
#include "opentxs/api/client/blockchain/BalanceNode.hpp"
#include "opentxs/api/client/blockchain/BalanceTree.hpp"
#include "opentxs/api/client/blockchain/HD.hpp"
#include "opentxs/api/client/blockchain/Subchain.hpp"
#include "opentxs/blockchain/BlockchainType.hpp"
#include "opentxs/client/OTAPI_Exec.hpp"
#include "opentxs/core/Data.hpp"
#include "opentxs/core/Identifier.hpp"
#include "opentxs/core/PasswordPrompt.hpp"
#include "opentxs/core/identifier/Nym.hpp"
#include "opentxs/crypto/Types.hpp"
#include "opentxs/crypto/key/HD.hpp"
#include "opentxs/identity/Nym.hpp"
#include "opentxs/protobuf/ContactEnums.pb.h"
#include "opentxs/protobuf/HDPath.pb.h"

#if OT_CRYPTO_WITH_BIP32
namespace ot = opentxs;

namespace
{
bool init_{false};
const ot::proto::ContactItemType individual_{ot::proto::CITEMTYPE_INDIVIDUAL};
const ot::blockchain::Type btc_chain_{ot::blockchain::Type::Bitcoin};
const ot::blockchain::Type bch_chain_{ot::blockchain::Type::BitcoinCash};
const ot::blockchain::Type ltc_chain_{ot::blockchain::Type::Litecoin};
std::unique_ptr<ot::OTPasswordPrompt> reason_p_{nullptr};
std::unique_ptr<ot::OTNymID> invalid_nym_p_{nullptr};
std::unique_ptr<ot::OTNymID> nym_not_in_wallet_p_{nullptr};
std::unique_ptr<ot::OTNymID> alex_p_{nullptr};
std::unique_ptr<ot::OTNymID> bob_p_{nullptr};
std::unique_ptr<ot::OTNymID> chris_p_{nullptr};
std::unique_ptr<ot::OTNymID> daniel_p_{nullptr};
std::unique_ptr<ot::OTData> address_1_p_{nullptr};
std::unique_ptr<ot::OTIdentifier> empty_p_{nullptr};
std::unique_ptr<ot::OTIdentifier> contact_alex_p_{nullptr};
std::unique_ptr<ot::OTIdentifier> contact_bob_p_{nullptr};
std::unique_ptr<ot::OTIdentifier> contact_chris_p_{nullptr};
std::unique_ptr<ot::OTIdentifier> contact_daniel_p_{nullptr};
std::unique_ptr<ot::OTIdentifier> account_1_id_p_{nullptr};
std::unique_ptr<ot::OTIdentifier> account_2_id_p_{nullptr};
std::unique_ptr<ot::OTIdentifier> account_3_id_p_{nullptr};
std::unique_ptr<ot::OTIdentifier> account_4_id_p_{nullptr};
std::unique_ptr<ot::OTIdentifier> account_5_id_p_{nullptr};
std::unique_ptr<ot::OTIdentifier> account_6_id_p_{nullptr};
std::unique_ptr<ot::OTIdentifier> account_7_id_p_{nullptr};
const std::string fingerprint_a_{};
const std::string fingerprint_b_{};
const std::string fingerprint_c_{};
const std::string dummy_script_{"00000000000000000000000000000000"};
const std::string txid_0_{"00000000000000000000000000000000"};
const std::string txid_1_{"11111111111111111111111111111111"};
const std::string txid_2_{"22222222222222222222222222222222"};
const std::string txid_3_{"33333333333333333333333333333333"};
const std::string txid_4_{"44444444444444444444444444444444"};
const std::string memo_1_{"memo 1"};
const std::string memo_2_{"memo 2"};
const std::string memo_3_{"memo 3"};
const std::string memo_4_{"memo 4"};
const std::string empty_string_{};
const std::vector<std::string> alex_external_{
    "1K9teXNg8iKYwUPregT8QTmMepb376oTuX",
    "1GgpoMuPBfaa4ZT6ZeKaTY8NH9Ldx4Q89t",
    "1FXb97adaza32zYQ5U29nxHZS4FmiCfXAJ",
    "1Dx4k7daUS1VNNeoDtZe1ujpt99YeW7Yz",
    "19KhniSVj1CovZWg1P5JvoM199nQR3gkhp",
    "1CBnxZdo58Vu3upwEt96uTMZLAxVx4Xeg9",
    "12vm2SqQ7RhhYPi6bJqqQzyJomV6H3j4AX",
    "1D2fNJYjyWL1jn5qRhJZL6EbGzeyBjHuP3",
    "19w4gVEse89JjE7TroavXZ9pyfJ78h4arG",
    "1DVYvYAmTNtvML7vBrhBBhyePaEDVCCNaw",
};
const std::vector<std::string> alex_internal_{
    "179XLYWcaHiPMnPUsSdrPiAwNcybx2vpaa",
    "1FPoX1BUe9a6ugobnQkzFyn1Uycyns4Ejp",
    "17jfyBx8ZHJ3DT9G2WehYEPKwT7Zv3kcLs",
    "15zErgibP264JkEMqihXQDp4Kb7vpvDpd5",
    "1KvRA5nngc4aA8y57A6TuS83Gud4xR5oPK",
    "14wC1Ph9z6S82QJA6yTaDaSZQjng9kDihT",
    "1FjW1pENbM6g5PAUpCdjQQykBYH6bzs5hU",
    "1Bt6BP3bXfRJbKUEFS15BrWa6Hca8G9W1L",
    "197TU7ptMMnhufMLFrY1o2Sgi5zcw2e3qv",
    "176aRLv3W94vyWPZDPY9csUrLNrqDFrzCs",
};
const std::vector<std::string> bob_external_{
    "1AngXb5xQoQ4nT8Bn6dDdr6AFS4yMZU2y",
    "1FQMy3HkD5C3gGZZHeeH9rjHgyqurxC44q",
    "1APXZ5bCTbj2ZRV3ZHyAa59CmsXRP4HkTh",
    "1M966pvtChYbceTsou73eB2hutwoZ7QtVv",
    "1HcN6BWFZKLNEdBo15oUPQGXpDJ26SVKQE",
    "1NcaLRLFr4edY4hUcR81aNMpveHaRqzxPR",
    "1CT86ZmqRFZW57aztRscjWuzkhJjgHjiMS",
    "1CXT6sU5s4mxP4UattFA6fGN7yW4dkkARn",
    "12hwhKpxTyfiSGDdQw63SWVzefRuRxrFqb",
    "18SRAzD6bZ2GsTK4J4RohhYneEyZAUvyqp",
};
const std::vector<std::string> bob_internal_{
    "1GXj4LrpYKugu4ps7BvYHkUgJLErjBcZc",
    "18yFFsUUe7ATjku2NfKizdnNfZGx99LmLJ",
    "19hDov3sMJdXkgrinhfD2seaKhcb6FiDKL",
    "1W9fEcakg5ZshPuAt5j2vTYkV6txNoiwq",
    "1EPTv3qdCJTbgqUZw83nUbjoKBmy4sHbhd",
    "17mcj9bmcuBfSZqc2mQnjLiT1mtPxGD1yu",
    "1LT2ZEnj1kmpgDbBQodiXVrAj6nRBmWUcH",
    "1HZmwsMWU87WFJxYDNQbnCW52KqUoLiCqZ",
    "16SdtUXrRey55j49Ae84YwVVNZXwGL2tLU",
    "1N2Y3mM828N4JQGLzDfxNjU2WK9CMMekVg",
};
const std::vector<std::string> chris_btc_external_{
    "1MWZN5PtYjfHA7WC1czB43HK9NjTKig1rA",
    "16Ach28pUQbWDpVhe75AjwoCJws144Nd25",
};
const std::vector<std::string> chris_btc_internal_{
    "1PsjtCRUQ32t5F18W2K8Zzpn1aVmuRmTdB",
    "15xi7Z3kVPg88ZYA82V8zPyodnQnamSZvN",
};
const std::vector<std::string> chris_bch_external_{
    "14Et9A6QnwpnUH2Ym9kZ4Zz1FN2GixG9qS",
    "17u11yKTfr13Xkm4k7h4bx3o3ssz4HSwGJ",
};
const std::vector<std::string> chris_bch_internal_{
    "1FkAAgJWW1YWSqa5ByvHFe8dQvfNLT2rQN",
    "1HyweNdaw2QoRU1YfuJQWcZKUAVqMXyJsj",
};
const std::vector<std::string> chris_ltc_external_{
    "LWDn8duKKwbP9hhCWpmX9o8BxywgCSTg41",
    "LSyrWGpCUm457F9TaXWAhvZs7Vu5g7a4Do",
};
const std::vector<std::string> chris_ltc_internal_{
    "LX3FAVopX2moW5h2ZwAKcrCKTChTyWqWze",
    "LMoZuWNnoTEJ1FjxQ4NXTcNbMK3croGpaF",
};

class Test_BlockchainAPI : public ::testing::Test
{
public:
    using AddressStyle = ot::api::client::blockchain::AddressStyle;
    using Subchain = ot::api::client::blockchain::Subchain;
    using ThreadData = std::tuple<std::string, std::uint64_t, std::string>;
    using ThreadVectors = std::map<int, std::vector<ThreadData>>;

    const ot::api::client::Manager& api_;
    ot::PasswordPrompt& reason_;
    const ot::identifier::Nym& invalid_nym_;
    const ot::identifier::Nym& nym_not_in_wallet_;
    const ot::identifier::Nym& alex_;
    const ot::identifier::Nym& bob_;
    const ot::identifier::Nym& chris_;
    const ot::identifier::Nym& daniel_;
    const ot::Data& address_1_;
    const ot::Identifier& empty_id_;
    const ot::Identifier& contact_alex_;
    const ot::Identifier& contact_bob_;
    const ot::Identifier& contact_chris_;
    const ot::Identifier& contact_daniel_;
    ot::Identifier& account_1_id_;  // alex, btc, bip32 *
    ot::Identifier& account_2_id_;  // daniel, btc, bip32
    ot::Identifier& account_3_id_;  // chris, btc, bip32
    ot::Identifier& account_4_id_;  // chris, btc, bip44
    ot::Identifier& account_5_id_;  // chris, bch, bip44 *
    ot::Identifier& account_6_id_;  // bob, btc, bip32 *
    ot::Identifier& account_7_id_;  // chris, ltc, bip44
    const ThreadVectors threads_;

    static const ot::api::client::Manager& init()
    {
        const auto& api = ot::Context().StartClient({}, 0);

        if (false == init_) {
            reason_p_.reset(new ot::OTPasswordPrompt{
                api.Factory().PasswordPrompt(__FUNCTION__)});
            invalid_nym_p_.reset(new ot::OTNymID{api.Factory().NymID("junk")});
            nym_not_in_wallet_p_.reset(new ot::OTNymID{
                api.Factory().NymID("ottaRUsttUuJZj738f9AE6kJJMBp6iedFYQ")});
            const_cast<std::string&>(fingerprint_a_) =
                api.Exec().Wallet_ImportSeed(
                    "response seminar brave tip suit recall often sound stick "
                    "owner lottery motion",
                    "");
            const_cast<std::string&>(fingerprint_b_) =
                api.Exec().Wallet_ImportSeed(
                    "reward upper indicate eight swift arch injury crystal "
                    "super wrestle already dentist",
                    "");
            const_cast<std::string&>(fingerprint_c_) =
                api.Exec().Wallet_ImportSeed(
                    "predict cinnamon gauge spoon media food nurse improve "
                    "employ similar own kid genius seed ghost",
                    "");
            alex_p_.reset(new ot::OTNymID{
                api.Wallet()
                    .Nym(*reason_p_, "Alex", {fingerprint_a_, 0}, individual_)
                    ->ID()});
            bob_p_.reset(new ot::OTNymID{
                api.Wallet()
                    .Nym(*reason_p_, "Bob", {fingerprint_b_, 0}, individual_)
                    ->ID()});
            chris_p_.reset(new ot::OTNymID{
                api.Wallet()
                    .Nym(*reason_p_, "Chris", {fingerprint_c_, 0}, individual_)
                    ->ID()});
            daniel_p_.reset(new ot::OTNymID{
                api.Wallet()
                    .Nym(*reason_p_, "Daniel", {fingerprint_a_, 1}, individual_)
                    ->ID()});

            address_1_p_.reset(new ot::OTData{api.Factory().Data(
                "0xf54a5851e9372b87810a8e60cdd2e7cfd80b6e31",
                ot::StringStyle::Hex)});
            empty_p_.reset(new ot::OTIdentifier{api.Factory().Identifier()});
            contact_alex_p_.reset(
                new ot::OTIdentifier{api.Contacts().ContactID(*alex_p_)});
            contact_bob_p_.reset(
                new ot::OTIdentifier{api.Contacts().ContactID(*bob_p_)});
            contact_chris_p_.reset(
                new ot::OTIdentifier{api.Contacts().ContactID(*chris_p_)});
            contact_daniel_p_.reset(
                new ot::OTIdentifier{api.Contacts().ContactID(*daniel_p_)});
            account_1_id_p_.reset(
                new ot::OTIdentifier{api.Factory().Identifier()});
            account_2_id_p_.reset(
                new ot::OTIdentifier{api.Factory().Identifier()});
            account_3_id_p_.reset(
                new ot::OTIdentifier{api.Factory().Identifier()});
            account_4_id_p_.reset(
                new ot::OTIdentifier{api.Factory().Identifier()});
            account_5_id_p_.reset(
                new ot::OTIdentifier{api.Factory().Identifier()});
            account_6_id_p_.reset(
                new ot::OTIdentifier{api.Factory().Identifier()});
            account_7_id_p_.reset(
                new ot::OTIdentifier{api.Factory().Identifier()});
            init_ = true;
        }

        return api;
    }

    Test_BlockchainAPI()
        : api_(init())
        , reason_(reason_p_->get())
        , invalid_nym_(invalid_nym_p_->get())
        , nym_not_in_wallet_(nym_not_in_wallet_p_->get())
        , alex_(alex_p_->get())
        , bob_(bob_p_->get())
        , chris_(chris_p_->get())
        , daniel_(daniel_p_->get())
        , address_1_(address_1_p_->get())
        , empty_id_(empty_p_->get())
        , contact_alex_(contact_alex_p_->get())
        , contact_bob_(contact_bob_p_->get())
        , contact_chris_(contact_chris_p_->get())
        , contact_daniel_(contact_daniel_p_->get())
        , account_1_id_(account_1_id_p_->get())
        , account_2_id_(account_2_id_p_->get())
        , account_3_id_(account_3_id_p_->get())
        , account_4_id_(account_4_id_p_->get())
        , account_5_id_(account_5_id_p_->get())
        , account_6_id_(account_6_id_p_->get())
        , account_7_id_(account_7_id_p_->get())
        , threads_({
              {0,
               {
                   {txid_3_, 0, ""},
               }},
              {1,
               {
                   {txid_2_, 0, ""},
               }},
              {2,
               {
                   {txid_1_, 0, ""},
               }},
              {3,
               {
                   {txid_2_, 0, ""},
               }},
              {4,
               {
                   {txid_4_, 1, ""},
               }},
              {5,
               {
                   {txid_4_, 0, ""},
               }},
              {6,
               {
                   {txid_2_, 0, ""},
                   {txid_4_, 1, ""},
               }},
          })
    {
    }
};

TEST_F(Test_BlockchainAPI, init)
{
    EXPECT_TRUE(invalid_nym_.empty());
    EXPECT_FALSE(nym_not_in_wallet_.empty());
    EXPECT_FALSE(alex_.empty());
    EXPECT_FALSE(bob_.empty());
    EXPECT_FALSE(chris_.empty());
    EXPECT_FALSE(daniel_.empty());
    EXPECT_TRUE(empty_id_.empty());
    EXPECT_FALSE(contact_alex_.empty());
    EXPECT_FALSE(contact_bob_.empty());
    EXPECT_FALSE(contact_chris_.empty());
    EXPECT_FALSE(contact_daniel_.empty());
    EXPECT_TRUE(account_1_id_.empty());
    EXPECT_TRUE(account_2_id_.empty());
    EXPECT_TRUE(account_3_id_.empty());
    EXPECT_TRUE(account_4_id_.empty());
    EXPECT_TRUE(account_5_id_.empty());
    EXPECT_TRUE(account_6_id_.empty());
    EXPECT_TRUE(account_7_id_.empty());
    EXPECT_FALSE(fingerprint_a_.empty());
    EXPECT_FALSE(fingerprint_b_.empty());
    EXPECT_FALSE(fingerprint_c_.empty());
}

TEST_F(Test_BlockchainAPI, invalid_nym)
{
    bool loaded(false);

    try {
        api_.Blockchain().Account(invalid_nym_, btc_chain_);
        loaded = true;
    } catch (...) {
    }

    EXPECT_FALSE(loaded);

    auto accountID = api_.Blockchain().NewHDSubaccount(
        invalid_nym_, ot::BlockchainAccountType::BIP44, btc_chain_, reason_);

    EXPECT_TRUE(accountID->empty());

    auto list = api_.Blockchain().AccountList(invalid_nym_, btc_chain_);

    EXPECT_EQ(list.size(), 0);
    EXPECT_EQ(list.count(accountID), 0);

    loaded = false;

    try {
        api_.Blockchain().Account(nym_not_in_wallet_, btc_chain_);
        loaded = true;
    } catch (...) {
    }

    EXPECT_FALSE(loaded);

    accountID = api_.Blockchain().NewHDSubaccount(
        nym_not_in_wallet_,
        ot::BlockchainAccountType::BIP44,
        btc_chain_,
        reason_);

    EXPECT_TRUE(accountID->empty());

    list = api_.Blockchain().AccountList(nym_not_in_wallet_, btc_chain_);

    EXPECT_EQ(list.size(), 0);
    EXPECT_EQ(list.count(accountID), 0);
}

// Test: when you create a nym with seed A, then the root of every HDPath for a
// blockchain account associated with that nym should also be A.
TEST_F(Test_BlockchainAPI, TestSeedRoot)
{
    account_1_id_.Assign(api_.Blockchain().NewHDSubaccount(
        alex_, ot::BlockchainAccountType::BIP32, btc_chain_, reason_));
    account_2_id_.Assign(api_.Blockchain().NewHDSubaccount(
        daniel_, ot::BlockchainAccountType::BIP32, btc_chain_, reason_));

    EXPECT_FALSE(account_1_id_.empty());
    EXPECT_FALSE(account_2_id_.empty());

    auto list = api_.Blockchain().AccountList(alex_, btc_chain_);

    EXPECT_EQ(list.size(), 1);
    EXPECT_EQ(list.count(account_1_id_), 1);

    list = api_.Blockchain().AccountList(daniel_, btc_chain_);

    EXPECT_EQ(list.size(), 1);
    EXPECT_EQ(list.count(account_2_id_), 1);

    // Test difference in index on BIP32 implies a different account
    EXPECT_NE(account_1_id_, account_2_id_);

    try {
        const auto& account1 =
            api_.Blockchain().HDSubaccount(alex_, account_1_id_);

        EXPECT_EQ(account1.Path().root(), fingerprint_a_);
    } catch (const std::exception& e) {
        std::cout << __LINE__ << ": " << e.what() << '\n';
        EXPECT_TRUE(false);
    }

    try {
        const auto& account2 =
            api_.Blockchain().HDSubaccount(daniel_, account_2_id_);

        EXPECT_EQ(account2.Path().root(), fingerprint_a_);
    } catch (const std::exception& e) {
        std::cout << __LINE__ << ": " << e.what() << '\n';
        EXPECT_TRUE(false);
    }

    EXPECT_EQ(alex_, api_.Blockchain().Owner(account_1_id_));
    EXPECT_EQ(daniel_, api_.Blockchain().Owner(account_2_id_));
}

// Test that one nym creates the same account for the same chain (BIP32 or
// BIP44).
TEST_F(Test_BlockchainAPI, TestNym_AccountIdempotence)
{
    account_3_id_.Assign(api_.Blockchain().NewHDSubaccount(
        chris_, ot::BlockchainAccountType::BIP32, btc_chain_, reason_));
    account_4_id_.Assign(api_.Blockchain().NewHDSubaccount(
        chris_, ot::BlockchainAccountType::BIP44, btc_chain_, reason_));

    EXPECT_FALSE(account_3_id_.empty());
    EXPECT_FALSE(account_4_id_.empty());
    EXPECT_NE(account_3_id_, account_4_id_);

    const auto& before =
        api_.Blockchain().Account(chris_, btc_chain_).GetHD().at(account_4_id_);

    EXPECT_EQ(before.ID(), account_4_id_);

    const auto duplicate = api_.Blockchain().NewHDSubaccount(
        chris_, ot::BlockchainAccountType::BIP44, btc_chain_, reason_);

    EXPECT_EQ(account_4_id_, duplicate);

    const auto& after =
        api_.Blockchain().Account(chris_, btc_chain_).GetHD().at(account_4_id_);

    EXPECT_EQ(after.ID(), account_4_id_);

    auto list = api_.Blockchain().AccountList(chris_, btc_chain_);

    EXPECT_EQ(list.size(), 2);
    EXPECT_EQ(list.count(account_3_id_), 1);
    EXPECT_EQ(list.count(account_4_id_), 1);
}

// Test that the same nym creates different accounts for two chains
TEST_F(Test_BlockchainAPI, TestChainDiff)
{
    account_5_id_.Assign(api_.Blockchain().NewHDSubaccount(
        chris_, ot::BlockchainAccountType::BIP44, bch_chain_, reason_));

    EXPECT_NE(account_5_id_, account_4_id_);

    auto list = api_.Blockchain().AccountList(chris_, bch_chain_);

    EXPECT_EQ(list.size(), 1);
    EXPECT_EQ(list.count(account_5_id_), 1);
}

// https://github.com/bitcoin/bips/blob/master/bip-0032.mediawiki#test-vector-1
TEST_F(Test_BlockchainAPI, TestBip32_standard_1)
{
    const std::string empty{};
    auto bytes = api_.Factory().Data(
        "0x000102030405060708090a0b0c0d0e0f", ot::StringStyle::Hex);
    auto seed = api_.Factory().SecretFromBytes(bytes->Bytes());
    const auto fingerprint = api_.Seeds().ImportRaw(seed, reason_);

    ASSERT_FALSE(fingerprint.empty());

    const auto& nymID =
        api_.Wallet()
            .Nym(reason_, "John Doe", {fingerprint, 0}, individual_)
            ->ID();

    ASSERT_FALSE(nymID.empty());

    const auto accountID = api_.Blockchain().NewHDSubaccount(
        nymID, ot::BlockchainAccountType::BIP32, btc_chain_, reason_);

    ASSERT_FALSE(accountID->empty());

    const auto& account =
        api_.Blockchain().Account(nymID, btc_chain_).GetHD().at(0);

    EXPECT_EQ(account.ID(), accountID);

    const auto pRoot = account.RootNode(reason_);

    ASSERT_TRUE(pRoot);

    const auto& root = *pRoot;
    const std::string xpub{
        "xpub68Gmy5EdvgibQVfPdqkBBCHxA5htiqg55crXYuXoQRKfDBFA1WEjWgP6LHhwBZeNK1"
        "VTsfTFUHCdrfp1bgwQ9xv5ski8PX9rL2dZXvgGDnw"};
    const std::string xprv{
        "xprv9uHRZZhk6KAJC1avXpDAp4MDc3sQKNxDiPvvkX8Br5ngLNv1TxvUxt4cV1rGL5hj6K"
        "CesnDYUhd7oWgT11eZG7XnxHrnYeSvkzY7d2bhkJ7"};

    EXPECT_EQ(xpub, root.Xpub(reason_));
    EXPECT_EQ(xprv, root.Xprv(reason_));
}

// https://github.com/bitcoin/bips/blob/master/bip-0032.mediawiki#test-vector-3
TEST_F(Test_BlockchainAPI, TestBip32_standard_3)
{
    const std::string empty{};
    auto bytes = api_.Factory().Data(
        "0x4b381541583be4423346c643850da4b320e46a87ae3d2a4e6da11eba819cd4acba45"
        "d239319ac14f863b8d5ab5a0d0c64d2e8a1e7d1457df2e5a3c51c73235be",
        ot::StringStyle::Hex);
    auto seed = api_.Factory().SecretFromBytes(bytes->Bytes());
    const auto fingerprint = api_.Seeds().ImportRaw(seed, reason_);

    ASSERT_FALSE(fingerprint.empty());

    const auto& nymID =
        api_.Wallet()
            .Nym(reason_, "John Doe", {fingerprint, 0}, individual_)
            ->ID();

    ASSERT_FALSE(nymID.empty());

    const auto accountID = api_.Blockchain().NewHDSubaccount(
        nymID, ot::BlockchainAccountType::BIP32, btc_chain_, reason_);

    ASSERT_FALSE(accountID->empty());

    const auto& account =
        api_.Blockchain().Account(nymID, btc_chain_).GetHD().at(0);

    EXPECT_EQ(account.ID(), accountID);

    const auto pRoot = account.RootNode(reason_);

    ASSERT_TRUE(pRoot);

    const auto& root = *pRoot;
    const std::string xpub{
        "xpub68NZiKmJWnxxS6aaHmn81bvJeTESw724CRDs6HbuccFQN9Ku14VQrADWgqbhhTHBao"
        "hPX4CjNLf9fq9MYo6oDaPPLPxSb7gwQN3ih19Zm4Y"};
    const std::string xprv{
        "xprv9uPDJpEQgRQfDcW7BkF7eTya6RPxXeJCqCJGHuCJ4GiRVLzkTXBAJMu2qaMWPrS7AA"
        "NYqdq6vcBcBUdJCVVFceUvJFjaPdGZ2y9WACViL4L"};

    EXPECT_EQ(xpub, root.Xpub(reason_));
    EXPECT_EQ(xprv, root.Xprv(reason_));
}

TEST_F(Test_BlockchainAPI, testBip32_SeedA)
{
    const auto& account =
        api_.Blockchain().Account(alex_, btc_chain_).GetHD().at(0);

    EXPECT_EQ(account.ID(), account_1_id_);
    EXPECT_FALSE(account.LastUsed(Subchain::External));
    EXPECT_FALSE(account.LastUsed(Subchain::Internal));
    EXPECT_TRUE(account.LastGenerated(Subchain::External));
    EXPECT_TRUE(account.LastGenerated(Subchain::Internal));

    std::optional<ot::Bip32Index> index{};
    std::optional<ot::Bip32Index> last{};
    std::optional<ot::Bip32Index> generated{};

    for (ot::Bip32Index i{0}; i < 10; ++i) {
        const auto label{std::string{"receive "} + std::to_string(i)};
        index =
            account.UseNext(Subchain::External, reason_, contact_bob_, label);
        last = account.LastUsed(Subchain::External);
        generated = account.LastGenerated(Subchain::External);

        ASSERT_TRUE(index);
        EXPECT_EQ(i, index.value());
        ASSERT_TRUE(last);
        EXPECT_EQ(i, last.value());
        ASSERT_TRUE(generated);
        EXPECT_TRUE(generated.value() > last.value());

        const auto& element = account.BalanceElement(Subchain::External, i);
        const auto& target = alex_external_.at(i);

        EXPECT_EQ(element.Address(AddressStyle::P2PKH), target);

        const auto [bytes, style, chains, supported] =
            api_.Blockchain().DecodeAddress(target);

        ASSERT_GT(chains.size(), 0);

        const auto& chain = *chains.cbegin();
        const auto encoded =
            api_.Blockchain().EncodeAddress(style, chain, bytes);

        EXPECT_EQ(target, encoded);
        EXPECT_EQ(element.Contact()->str(), contact_bob_.str());
        EXPECT_EQ(element.Label(), label);
    }

    for (ot::Bip32Index i{0}; i < 10; ++i) {
        const auto label{std::string{"change "} + std::to_string(i)};
        index =
            account.UseNext(Subchain::Internal, reason_, contact_bob_, label);
        last = account.LastUsed(Subchain::Internal);
        generated = account.LastGenerated(Subchain::Internal);

        ASSERT_TRUE(index);
        EXPECT_EQ(i, index.value());
        ASSERT_TRUE(last);
        EXPECT_EQ(i, last.value());
        ASSERT_TRUE(generated);
        EXPECT_TRUE(generated.value() > last.value());

        const auto& element = account.BalanceElement(Subchain::Internal, i);

        EXPECT_EQ(element.Address(AddressStyle::P2PKH), alex_internal_.at(i));
        EXPECT_EQ(element.Contact()->str(), empty_id_.str());
        EXPECT_EQ(element.Label(), label);
        EXPECT_TRUE(element.PrivateKey(reason_));
    }
}

TEST_F(Test_BlockchainAPI, testBip32_SeedB)
{
    account_6_id_.Assign(api_.Blockchain().NewHDSubaccount(
        bob_, ot::BlockchainAccountType::BIP32, btc_chain_, reason_));

    ASSERT_FALSE(account_6_id_.empty());

    auto list = api_.Blockchain().AccountList(bob_, btc_chain_);

    EXPECT_EQ(list.size(), 1);
    EXPECT_EQ(list.count(account_6_id_), 1);

    const auto& account =
        api_.Blockchain().Account(bob_, btc_chain_).GetHD().at(0);

    EXPECT_EQ(account.ID(), account_6_id_);
    EXPECT_FALSE(account.LastUsed(Subchain::External));
    EXPECT_FALSE(account.LastUsed(Subchain::Internal));
    EXPECT_TRUE(account.LastGenerated(Subchain::External));
    EXPECT_TRUE(account.LastGenerated(Subchain::Internal));

    std::optional<ot::Bip32Index> index{};
    std::optional<ot::Bip32Index> last{};
    std::optional<ot::Bip32Index> generated{};

    for (ot::Bip32Index i{0}; i < 10; ++i) {
        const auto label{std::string{"receive "} + std::to_string(i)};
        index =
            account.UseNext(Subchain::External, reason_, contact_alex_, label);
        last = account.LastUsed(Subchain::External);
        generated = account.LastGenerated(Subchain::External);

        ASSERT_TRUE(index);
        EXPECT_EQ(i, index.value());
        ASSERT_TRUE(last);
        EXPECT_EQ(i, last.value());
        ASSERT_TRUE(generated);
        EXPECT_TRUE(generated.value() > last.value());

        const auto& element = account.BalanceElement(Subchain::External, i);

        EXPECT_EQ(element.Address(AddressStyle::P2PKH), bob_external_.at(i));
        EXPECT_EQ(element.Contact()->str(), contact_alex_.str());
        EXPECT_EQ(element.Label(), label);
        EXPECT_TRUE(element.PrivateKey(reason_));
    }

    for (ot::Bip32Index i{0}; i < 10; ++i) {
        const auto label{std::string{"change "} + std::to_string(i)};
        index =
            account.UseNext(Subchain::Internal, reason_, contact_alex_, label);
        last = account.LastUsed(Subchain::Internal);
        generated = account.LastGenerated(Subchain::Internal);

        ASSERT_TRUE(index);
        EXPECT_EQ(i, index.value());
        ASSERT_TRUE(last);
        EXPECT_EQ(i, last.value());
        ASSERT_TRUE(generated);
        EXPECT_TRUE(generated.value() > last.value());

        const auto& element = account.BalanceElement(Subchain::Internal, i);

        EXPECT_EQ(element.Address(AddressStyle::P2PKH), bob_internal_.at(i));
        EXPECT_EQ(element.Contact()->str(), empty_id_.str());
        EXPECT_EQ(element.Label(), label);
        EXPECT_TRUE(element.PrivateKey(reason_));
    }
}

TEST_F(Test_BlockchainAPI, testBip44_btc)
{
    const auto& account =
        api_.Blockchain().Account(chris_, btc_chain_).GetHD().at(account_4_id_);

    EXPECT_EQ(account.ID(), account_4_id_);
    EXPECT_FALSE(account.LastUsed(Subchain::External));
    EXPECT_FALSE(account.LastUsed(Subchain::Internal));
    EXPECT_TRUE(account.LastGenerated(Subchain::External));
    EXPECT_TRUE(account.LastGenerated(Subchain::Internal));

    std::optional<ot::Bip32Index> index{};
    std::optional<ot::Bip32Index> last{};
    std::optional<ot::Bip32Index> generated{};

    for (ot::Bip32Index i{0}; i < 2; ++i) {
        const auto label{std::string{"receive "} + std::to_string(i)};
        index = account.UseNext(
            Subchain::External, reason_, contact_daniel_, label);
        last = account.LastUsed(Subchain::External);
        generated = account.LastGenerated(Subchain::External);

        ASSERT_TRUE(index);
        EXPECT_EQ(i, index.value());
        ASSERT_TRUE(last);
        EXPECT_EQ(i, last.value());
        ASSERT_TRUE(generated);
        EXPECT_TRUE(generated.value() > last.value());

        const auto& element = account.BalanceElement(Subchain::External, i);

        EXPECT_EQ(
            element.Address(AddressStyle::P2PKH), chris_btc_external_.at(i));
        EXPECT_EQ(element.Contact()->str(), contact_daniel_.str());
        EXPECT_EQ(element.Label(), label);
        EXPECT_TRUE(element.PrivateKey(reason_));
    }

    for (ot::Bip32Index i{0}; i < 2; ++i) {
        const auto label{std::string{"change "} + std::to_string(i)};
        index = account.UseNext(
            Subchain::Internal, reason_, contact_daniel_, label);
        last = account.LastUsed(Subchain::Internal);
        generated = account.LastGenerated(Subchain::Internal);

        ASSERT_TRUE(index);
        EXPECT_EQ(i, index.value());
        ASSERT_TRUE(last);
        EXPECT_EQ(i, last.value());
        ASSERT_TRUE(generated);
        EXPECT_TRUE(generated.value() > last.value());

        const auto& element = account.BalanceElement(Subchain::Internal, i);

        EXPECT_EQ(
            element.Address(AddressStyle::P2PKH), chris_btc_internal_.at(i));
        EXPECT_EQ(element.Contact()->str(), empty_id_.str());
        EXPECT_EQ(element.Label(), label);
        EXPECT_TRUE(element.PrivateKey(reason_));
    }
}

TEST_F(Test_BlockchainAPI, testBip44_bch)
{
    const auto& account =
        api_.Blockchain().Account(chris_, bch_chain_).GetHD().at(account_5_id_);

    EXPECT_EQ(account.ID(), account_5_id_);
    EXPECT_FALSE(account.LastUsed(Subchain::External));
    EXPECT_FALSE(account.LastUsed(Subchain::Internal));
    EXPECT_TRUE(account.LastGenerated(Subchain::External));
    EXPECT_TRUE(account.LastGenerated(Subchain::Internal));

    std::optional<ot::Bip32Index> index{};
    std::optional<ot::Bip32Index> last{};
    std::optional<ot::Bip32Index> generated{};

    for (ot::Bip32Index i{0}; i < 2; ++i) {
        const auto label{std::string{"receive "} + std::to_string(i)};
        index = account.UseNext(Subchain::External, reason_, empty_id_, label);
        last = account.LastUsed(Subchain::External);
        generated = account.LastGenerated(Subchain::External);

        ASSERT_TRUE(index);
        EXPECT_EQ(i, index.value());
        ASSERT_TRUE(last);
        EXPECT_EQ(i, last.value());
        ASSERT_TRUE(generated);
        EXPECT_TRUE(generated.value() > last.value());

        const auto& element = account.BalanceElement(Subchain::External, i);

        EXPECT_EQ(
            element.Address(AddressStyle::P2PKH), chris_bch_external_.at(i));
        EXPECT_EQ(element.Contact()->str(), empty_id_.str());
        EXPECT_EQ(element.Label(), label);
        EXPECT_TRUE(element.PrivateKey(reason_));
    }

    for (ot::Bip32Index i{0}; i < 2; ++i) {
        const auto label{std::string{"change "} + std::to_string(i)};
        index = account.UseNext(Subchain::Internal, reason_, empty_id_, label);
        last = account.LastUsed(Subchain::Internal);
        generated = account.LastGenerated(Subchain::Internal);

        ASSERT_TRUE(index);
        EXPECT_EQ(i, index.value());
        ASSERT_TRUE(last);
        EXPECT_EQ(i, last.value());
        ASSERT_TRUE(generated);
        EXPECT_TRUE(generated.value() > last.value());

        const auto& element = account.BalanceElement(Subchain::Internal, i);

        EXPECT_EQ(
            element.Address(AddressStyle::P2PKH), chris_bch_internal_.at(i));
        EXPECT_EQ(element.Contact()->str(), empty_id_.str());
        EXPECT_EQ(element.Label(), label);
        EXPECT_TRUE(element.PrivateKey(reason_));
    }
}

TEST_F(Test_BlockchainAPI, testBip44_ltc)
{
    account_7_id_.Assign(api_.Blockchain().NewHDSubaccount(
        chris_, ot::BlockchainAccountType::BIP44, ltc_chain_, reason_));

    ASSERT_FALSE(account_7_id_.empty());

    auto list = api_.Blockchain().AccountList(chris_, ltc_chain_);

    EXPECT_EQ(list.size(), 1);
    EXPECT_EQ(list.count(account_7_id_), 1);

    const auto& account =
        api_.Blockchain().Account(chris_, ltc_chain_).GetHD().at(account_7_id_);

    EXPECT_EQ(account.ID(), account_7_id_);
    EXPECT_FALSE(account.LastUsed(Subchain::External));
    EXPECT_FALSE(account.LastUsed(Subchain::Internal));
    EXPECT_TRUE(account.LastGenerated(Subchain::External));
    EXPECT_TRUE(account.LastGenerated(Subchain::Internal));

    std::optional<ot::Bip32Index> index{};
    std::optional<ot::Bip32Index> last{};
    std::optional<ot::Bip32Index> generated{};

    for (ot::Bip32Index i{0}; i < 2; ++i) {
        const auto label{empty_string_};
        index =
            account.UseNext(Subchain::External, reason_, contact_alex_, label);
        last = account.LastUsed(Subchain::External);
        generated = account.LastGenerated(Subchain::External);

        ASSERT_TRUE(index);
        EXPECT_EQ(i, index.value());
        ASSERT_TRUE(last);
        EXPECT_EQ(i, last.value());
        ASSERT_TRUE(generated);
        EXPECT_TRUE(generated.value() > last.value());

        const auto& element = account.BalanceElement(Subchain::External, i);
        const auto& target = chris_ltc_external_.at(i);

        EXPECT_EQ(element.Address(AddressStyle::P2PKH), target);

        const auto [bytes, style, chains, supported] =
            api_.Blockchain().DecodeAddress(target);

        ASSERT_GT(chains.size(), 0);

        const auto& chain = *chains.cbegin();
        const auto encoded =
            api_.Blockchain().EncodeAddress(style, chain, bytes);

        EXPECT_EQ(target, encoded);
        EXPECT_EQ(element.Contact()->str(), contact_alex_.str());
        EXPECT_EQ(element.Label(), label);
        EXPECT_TRUE(element.PrivateKey(reason_));
    }

    for (ot::Bip32Index i{0}; i < 2; ++i) {
        const auto label{std::string{"change "} + std::to_string(i)};
        index =
            account.UseNext(Subchain::Internal, reason_, contact_alex_, label);
        last = account.LastUsed(Subchain::Internal);
        generated = account.LastGenerated(Subchain::Internal);

        ASSERT_TRUE(index);
        EXPECT_EQ(i, index.value());
        ASSERT_TRUE(last);
        EXPECT_EQ(i, last.value());
        ASSERT_TRUE(generated);
        EXPECT_TRUE(generated.value() > last.value());

        const auto& element = account.BalanceElement(Subchain::Internal, i);

        EXPECT_EQ(
            element.Address(AddressStyle::P2PKH), chris_ltc_internal_.at(i));
        EXPECT_EQ(element.Contact()->str(), empty_id_.str());
        EXPECT_EQ(element.Label(), label);
        EXPECT_TRUE(element.PrivateKey(reason_));
    }
}

TEST_F(Test_BlockchainAPI, AccountList)
{
    auto list = api_.Blockchain().AccountList(alex_, bch_chain_);

    EXPECT_EQ(list.size(), 0);

    list = api_.Blockchain().AccountList(alex_, ltc_chain_);

    EXPECT_EQ(list.size(), 0);

    list = api_.Blockchain().AccountList(bob_, bch_chain_);

    EXPECT_EQ(list.size(), 0);

    list = api_.Blockchain().AccountList(bob_, ltc_chain_);

    EXPECT_EQ(list.size(), 0);

    list = api_.Blockchain().AccountList(daniel_, bch_chain_);

    EXPECT_EQ(list.size(), 0);

    list = api_.Blockchain().AccountList(daniel_, ltc_chain_);

    EXPECT_EQ(list.size(), 0);
}
}  // namespace
#endif  // OT_CRYPTO_WITH_BIP32
