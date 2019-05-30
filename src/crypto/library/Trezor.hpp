// Copyright (c) 2018 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#define OPENTXS_TREZOR_PROVIDES_ECDSA                                          \
    OT_CRYPTO_SUPPORTED_KEY_SECP256K1 || OT_CRYPTO_SUPPORTED_KEY_ED25519

namespace opentxs::crypto::implementation
{
class Trezor final : virtual public crypto::Trezor,
                     virtual public EncodingProvider
#if OT_CRYPTO_WITH_BIP39
    ,
                     virtual public crypto::Bip39
#endif
#if OT_CRYPTO_WITH_BIP32
    ,
                     public Bip32
#endif
#if OPENTXS_TREZOR_PROVIDES_ECDSA
    ,
                     public AsymmetricProvider,
                     public EcdsaProvider
#endif
{
public:
    std::string Base58CheckEncode(
        const std::uint8_t* inputStart,
        const std::size_t& inputSize) const override;
    bool Base58CheckDecode(const std::string&& input, RawData& output)
        const override;
    bool RIPEMD160(
        const std::uint8_t* input,
        const size_t inputSize,
        std::uint8_t* output) const override;

#if OT_CRYPTO_WITH_BIP32
    Key DeriveKey(
        const api::crypto::Hash& hash,
        const EcdsaCurve& curve,
        const OTPassword& seed,
        const Path& path) const override;
    bool RandomKeypair(OTPassword& privateKey, Data& publicKey) const override;
    std::string SeedToFingerprint(
        const EcdsaCurve& curve,
        const OTPassword& seed) const override;
#endif

#if OT_CRYPTO_WITH_BIP39
    bool SeedToWords(const OTPassword& seed, OTPassword& words) const override;
    void WordsToSeed(
        const OTPassword& words,
        OTPassword& seed,
        const OTPassword& passphrase) const override;
#endif

#if OPENTXS_TREZOR_PROVIDES_ECDSA
    bool ECDH(
        const Data& publicKey,
        const OTPassword& privateKey,
        OTPassword& secret) const override;
    bool ScalarBaseMultiply(const OTPassword& privateKey, Data& publicKey)
        const override;
    bool Sign(
        const api::Core& api,
        const Data& plaintext,
        const key::Asymmetric& theKey,
        const proto::HashType hashType,
        Data& signature,  // output
        const PasswordPrompt& reason,
        const OTPassword* exportPassword = nullptr) const override;
    bool Verify(
        const Data& plaintext,
        const key::Asymmetric& theKey,
        const Data& signature,
        const proto::HashType hashType,
        const PasswordPrompt& reason) const override;
#endif  // OPENTXS_TREZOR_PROVIDES_ECDSA

    ~Trezor() = default;

private:
    friend opentxs::Factory;

    typedef bool DerivationMode;
    const DerivationMode DERIVE_PRIVATE = true;
    const DerivationMode DERIVE_PUBLIC = false;

    const std::uint8_t KeyMax[32]{
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xFF, 0xFF, 0xFC, 0x2F};

    static std::string curve_name(const EcdsaCurve& curve);

#if OPENTXS_TREZOR_PROVIDES_ECDSA
    const curve_info* secp256k1_{nullptr};
    const curve_info* ed25519_{nullptr};

    const curve_info* get_curve(const EcdsaCurve& curve) const;
    const curve_info* get_curve(const proto::AsymmetricKeyType& curve) const;
#endif

#if OT_CRYPTO_WITH_BIP32
    static std::unique_ptr<HDNode> derive_child(
        const HDNode& parent,
        const Bip32Index index,
        const DerivationMode privateVersion);
    static std::unique_ptr<HDNode> instantiate_node(
        const EcdsaCurve& curve,
        const OTPassword& seed);

    std::unique_ptr<HDNode> derive_child(
        const api::crypto::Hash& hash,
        const EcdsaCurve& curve,
        const OTPassword& seed,
        const Path& path,
        Bip32Fingerprint& parentID) const;
    bool is_valid(const OTPassword& key) const;
#endif

    Trezor(const api::Crypto& crypto);
    Trezor() = delete;
    Trezor(const Trezor&) = delete;
    Trezor(Trezor&&) = delete;
    Trezor& operator=(const Trezor&) = delete;
    Trezor& operator=(Trezor&&) = delete;
};
}  // namespace opentxs::crypto::implementation
