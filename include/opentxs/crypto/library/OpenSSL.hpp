// Copyright (c) 2019 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef OPENTXS_CRYPTO_LIBRARY_OPENSSL_HPP
#define OPENTXS_CRYPTO_LIBRARY_OPENSSL_HPP

#include "Internal.hpp"

#if OT_CRYPTO_USING_OPENSSL
#if OT_CRYPTO_SUPPORTED_KEY_RSA
#include "opentxs/core/crypto/OTEnvelope.hpp"
#include "opentxs/crypto/library/AsymmetricProvider.hpp"
#endif
#include "opentxs/crypto/library/HashingProvider.hpp"
#include "opentxs/crypto/library/Ripemd160.hpp"

namespace opentxs::crypto
{
class OpenSSL : virtual public HashingProvider,
                virtual public Ripemd160
#if OT_CRYPTO_SUPPORTED_KEY_RSA
    ,
                virtual public AsymmetricProvider
#endif
{
public:
#if OT_CRYPTO_SUPPORTED_KEY_RSA
    virtual bool EncryptSessionKey(
        const mapOfAsymmetricKeys& RecipPubKeys,
        Data& plaintext,
        Data& dataOutput,
        const PasswordPrompt& reason) const = 0;
    virtual bool DecryptSessionKey(
        Data& dataInput,
        const identity::Nym& theRecipient,
        Data& plaintext,
        const PasswordPrompt& reason) const = 0;
#endif  // OT_CRYPTO_SUPPORTED_KEY_RSA

    virtual void Cleanup() = 0;
    virtual void Init() = 0;

    virtual ~OpenSSL() = default;

protected:
    OpenSSL() = default;

private:
    OpenSSL(const OpenSSL&) = delete;
    OpenSSL(OpenSSL&&) = delete;
    OpenSSL& operator=(const OpenSSL&) = delete;
    OpenSSL& operator=(OpenSSL&&) = delete;
};
}  // namespace opentxs::crypto

#endif  // OT_CRYPTO_USING_OPENSSL
#endif
