// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "Internal.hpp"

#include "crypto/library/AsymmetricProviderNull.hpp"

#include <memory>

namespace opentxs::crypto::key::implementation
{
class Null : virtual public key::Asymmetric
{
public:
    OTData CalculateHash(const proto::HashType, const PasswordPrompt&)
        const override
    {
        return Data::Factory();
    }
    bool CalculateID(Identifier&) const override { return false; }
    const opentxs::crypto::AsymmetricProvider& engine() const override
    {
        throw;
    }
    const OTSignatureMetadata* GetMetadata() const override { return nullptr; }
    bool hasCapability(const NymCapability&) const override { return false; }
    bool HasPrivate() const override { return false; }
    bool HasPublic() const override { return false; }
    proto::AsymmetricKeyType keyType() const override
    {
        return proto::AKEYTYPE_NULL;
    }
    bool Open(
        crypto::key::Asymmetric&,
        crypto::key::Symmetric&,
        PasswordPrompt&,
        const PasswordPrompt&) const override
    {
        return false;
    }
    const std::string Path() const override { return {}; }
    bool Path(proto::HDPath&) const override { return false; }
    const proto::KeyRole& Role() const override { throw; }
    std::shared_ptr<proto::AsymmetricKey> Serialize() const override
    {
        return nullptr;
    }
    OTData SerializeKeyToData(const proto::AsymmetricKey&) const override
    {
        return Data::Factory();
    }
    proto::HashType SigHashType() const override
    {
        return proto::HASHTYPE_NONE;
    }
    bool Sign(
        const Data&,
        proto::Signature&,
        const PasswordPrompt&,
        const OTPassword* = nullptr,
        const String& = String::Factory(""),
        const proto::SignatureRole = proto::SIGROLE_ERROR) const override
    {
        return false;
    }
    bool Sign(
        const GetPreimage,
        const proto::SignatureRole,
        proto::Signature&,
        const Identifier&,
        const PasswordPrompt&,
        proto::KeyRole,
        const proto::HashType) const override
    {
        return false;
    }
    bool TransportKey(Data&, OTPassword&, const PasswordPrompt& reason)
        const override
    {
        return false;
    }
    bool Verify(const Data&, const proto::Signature&, const PasswordPrompt&)
        const override
    {
        return false;
    }

    void Release() override {}
    void ReleaseKey() override {}
    bool Seal(
        const opentxs::api::Core&,
        OTAsymmetricKey&,
        crypto::key::Symmetric&,
        const PasswordPrompt&,
        PasswordPrompt&) const override
    {
        return false;
    }
    void SetAsPublic() override {}
    void SetAsPrivate() override {}

    operator bool() const override { return false; }
    bool operator==(const proto::AsymmetricKey&) const override
    {
        return false;
    }

    Null() = default;
    ~Null() = default;

private:
    Null* clone() const override { return new Null; }
};

#if OT_CRYPTO_SUPPORTED_KEY_HD
class NullEC : virtual public key::EllipticCurve, public Null
{
public:
    bool CheckCapability(const NymCapability&) const override { return {}; }
    const crypto::EcdsaProvider& ECDSA() const override { throw; }
    bool GetKey(Data&) const override { return {}; }
    bool GetKey(proto::Ciphertext&) const override { return {}; }
    const Asymmetric& GetPrivateKey() const override { throw; }
    const Asymmetric& GetPublicKey() const override { throw; }
    std::int32_t GetPublicKeyBySignature(Keys&, const Signature&, bool)
        const override
    {
        return {};
    }
    std::shared_ptr<proto::AsymmetricKey> GetSerialized(bool) const override
    {
        return {};
    }
    bool GetTransportKey(Data&, OTPassword&, const PasswordPrompt&)
        const override
    {
        return false;
    }
    OTData PrivateKey(const PasswordPrompt&) const override
    {
        return Data::Factory();
    }
    OTData PublicKey(const PasswordPrompt&) const override
    {
        return Data::Factory();
    }

    bool SetKey(const Data&) override { return {}; }
    bool SetKey(std::unique_ptr<proto::Ciphertext>&) override { return {}; }

    NullEC() = default;
    ~NullEC() = default;
};

class NullHD : virtual public key::HD, public NullEC
{
public:
    OTData Chaincode(const PasswordPrompt& reason) const override
    {
        return Data::Factory();
    }
    int Depth() const override { return {}; }
    Bip32Fingerprint Fingerprint(const PasswordPrompt& reason) const override
    {
        return {};
    }
    std::string Xprv(const PasswordPrompt& reason) const override { return {}; }
    std::string Xpub(const PasswordPrompt& reason) const override { return {}; }

    NullHD() = default;
    ~NullHD() = default;

private:
    NullHD* clone() const override { return new NullHD; }
};
#endif  // OT_CRYPTO_SUPPORTED_KEY_HD
}  // namespace opentxs::crypto::key::implementation
