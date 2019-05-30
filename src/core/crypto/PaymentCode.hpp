// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#if OT_CRYPTO_SUPPORTED_SOURCE_BIP47
namespace opentxs::implementation
{
class PaymentCode : virtual public opentxs::PaymentCode
{
public:
    bool operator==(const proto::PaymentCode& rhs) const override;
    operator const opentxs::crypto::key::Asymmetric&() const override;

    const OTNymID ID() const override;
    const std::string asBase58() const override;
    SerializedPaymentCode Serialize() const override;
    bool Sign(
        const identity::credential::Base& credential,
        proto::Signature& sig,
        const PasswordPrompt& reason) const override;
    bool Sign(const Data& data, Data& output, const PasswordPrompt& reason)
        const override;
    bool VerifyInternally() const override;
    bool Verify(
        const proto::Credential& master,
        const proto::Signature& sourceSignature,
        const PasswordPrompt& reason) const override;

    bool AddPrivateKeys(
        const std::string& seed,
        const Bip32Index index,
        const PasswordPrompt& reason) override;

    ~PaymentCode() = default;

private:
    friend opentxs::Factory;

    const std::uint8_t BIP47_VERSION_BYTE{0x47};

    const api::Core& api_;
    std::uint8_t version_{1};
    std::string seed_{""};
    std::int32_t index_{-1};
    OTAsymmetricKey asymmetric_key_;
    const crypto::key::Secp256k1* pubkey_{nullptr};
    std::unique_ptr<OTPassword> chain_code_{nullptr};
    bool hasBitmessage_{false};
    std::uint8_t bitmessage_version_{0};
    std::uint8_t bitmessage_stream_{0};

    static std::tuple<bool, std::unique_ptr<OTPassword>, OTData> make_key(
        const api::Core& api,
        const std::string& seed,
        const Bip32Index index,
        const PasswordPrompt& reason);

    PaymentCode* clone() const override;
    const OTData Pubkey() const;
    void ConstructKey(const Data& pubkey, const PasswordPrompt& reason);
    OTAsymmetricKey signing_key(const PasswordPrompt& reason) const;

    PaymentCode(
        const api::Core& api,
        const PasswordPrompt& reason,
        const std::string& base58);
    PaymentCode(
        const api::Core& api,
        const PasswordPrompt& reason,
        const proto::PaymentCode& paycode);
    PaymentCode(
        const api::Core& api,
        const PasswordPrompt& reason,
        const std::string& seed,
        const Bip32Index nym,
        const std::uint8_t version,
        const bool bitmessage,
        const std::uint8_t bitmessageVersion,
        const std::uint8_t bitmessageStream);
    PaymentCode() = delete;
    PaymentCode(const PaymentCode&);
    PaymentCode(PaymentCode&&) = delete;
    PaymentCode& operator=(const PaymentCode&);
    PaymentCode& operator=(PaymentCode&&);
};
}  // namespace opentxs::implementation
#endif  // OT_CRYPTO_SUPPORTED_SOURCE_BIP47
