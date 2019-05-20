// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "Internal.hpp"

#include "opentxs/crypto/key/Keypair.hpp"

#include "internal/identity/credential/Credential.hpp"
#include "Base.hpp"

#include <memory>

namespace opentxs::identity::credential::implementation
{
class Key : virtual public credential::internal::Key,
            public credential::implementation::Base
{
public:
    const crypto::key::Keypair& GetKeypair(
        const proto::AsymmetricKeyType type,
        const proto::KeyRole role) const override;
    std::int32_t GetPublicKeysBySignature(
        crypto::key::Keypair::Keys& listOutput,
        const Signature& theSignature,
        char cKeyType = '0') const override;
    bool hasCapability(const NymCapability& capability) const override;
    using Base::Verify;
    bool Verify(
        const Data& plaintext,
        const proto::Signature& sig,
        const proto::KeyRole key = proto::KEYROLE_SIGN) const override;
    bool Sign(
        const GetPreimage input,
        const proto::SignatureRole role,
        proto::Signature& signature,
        proto::KeyRole key = proto::KEYROLE_SIGN,
        const OTPasswordData* pPWData = nullptr,
        const proto::HashType hash = proto::HASHTYPE_BLAKE2B256) const override;
    bool TransportKey(Data& publicKey, OTPassword& privateKey) const override;

    bool ReEncryptKeys(const OTPassword& theExportPassword, bool bImporting)
        override;
    bool SelfSign(
        const OTPassword* exportPassword = nullptr,
        const OTPasswordData* pPWData = nullptr,
        const bool onlyPrivate = false) override;

    virtual ~Key() = default;

protected:
    OTKeypair signing_key_;
    OTKeypair authentication_key_;
    OTKeypair encryption_key_;

    std::shared_ptr<Base::SerializedType> serialize(
        const Lock& lock,
        const SerializationModeFlag asPrivate,
        const SerializationSignatureFlag asSigned) const override;
    bool verify_internally(const Lock& lock) const override;

    bool New(const NymParameters& nymParameters) override;

    Key(const api::Core& api,
        identity::internal::Authority& owner,
        const NymParameters& nymParameters,
        const VersionNumber version) noexcept;
    Key(const api::Core& api,
        identity::internal::Authority& owner,
        const proto::Credential& serializedCred) noexcept;

private:
    static const VersionConversionMap credential_subversion_;

    const VersionNumber subversion_;

    static OTKeypair deserialize_key(
        const int index,
        const proto::Credential& credential);
#if OT_CRYPTO_SUPPORTED_KEY_HD
    static OTKeypair derive_hd_keypair(
        const api::Crypto& crypto,
        const OTPassword& seed,
        const std::string& fingerprint,
        const std::uint32_t nym,
        const std::uint32_t credset,
        const std::uint32_t credindex,
        const EcdsaCurve& curve,
        const proto::KeyRole role);
#endif
    static OTKeypair new_key(
        const api::Crypto& crypto,
        const proto::KeyRole role,
        const NymParameters& nymParameters);

    bool addKeytoSerializedKeyCredential(
        proto::KeyCredential& credential,
        const bool getPrivate,
        const proto::KeyRole role) const;
    bool addKeyCredentialtoSerializedCredential(
        std::shared_ptr<Base::SerializedType> credential,
        const bool addPrivate) const;
    bool VerifySig(
        const Lock& lock,
        const proto::Signature& sig,
        const CredentialModeFlag asPrivate = PRIVATE_VERSION) const;
    bool VerifySignedBySelf(const Lock& lock) const;

    Key() = delete;
    Key(const Key&) = delete;
    Key(Key&&) = delete;
    Key& operator=(const Key&) = delete;
    Key& operator=(Key&&) = delete;
};
}  // namespace opentxs::identity::credential::implementation
