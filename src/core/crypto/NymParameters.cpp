/************************************************************
 *
 *                 OPEN TRANSACTIONS
 *
 *       Financial Cryptography and Digital Cash
 *       Library, Protocol, API, Server, CLI, GUI
 *
 *       -- Anonymous Numbered Accounts.
 *       -- Untraceable Digital Cash.
 *       -- Triple-Signed Receipts.
 *       -- Cheques, Vouchers, Transfers, Inboxes.
 *       -- Basket Currencies, Markets, Payment Plans.
 *       -- Signed, XML, Ricardian-style Contracts.
 *       -- Scripted smart contracts.
 *
 *  EMAIL:
 *  fellowtraveler@opentransactions.org
 *
 *  WEBSITE:
 *  http://www.opentransactions.org/
 *
 *  -----------------------------------------------------
 *
 *   LICENSE:
 *   This Source Code Form is subject to the terms of the
 *   Mozilla Public License, v. 2.0. If a copy of the MPL
 *   was not distributed with this file, You can obtain one
 *   at http://mozilla.org/MPL/2.0/.
 *
 *   DISCLAIMER:
 *   This program is distributed in the hope that it will
 *   be useful, but WITHOUT ANY WARRANTY; without even the
 *   implied warranty of MERCHANTABILITY or FITNESS FOR A
 *   PARTICULAR PURPOSE.  See the Mozilla Public License
 *   for more details.
 *
 ************************************************************/

#include <opentxs/core/stdafx.hpp>

#include <opentxs/core/crypto/NymParameters.hpp>

#ifdef __APPLE__
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

namespace opentxs
{

NymParameters::NymParameters(
    NymParameters::NymParameterType theKeytype,
    Credential::CredentialType theCredentialtype)
{
        setNymParameterType(theKeytype);
        setCredentialType(theCredentialtype);
}

NymParameters::NymParameterType NymParameters::nymParameterType() {
    return nymType_;
}

OTAsymmetricKey::KeyType NymParameters::AsymmetricKeyType() const
{
    OTAsymmetricKey::KeyType newKeyType;

    switch (nymType_) {
        case NymParameters::LEGACY :
            newKeyType = OTAsymmetricKey::LEGACY;
            break;
        case NymParameters::SECP256K1 :
            newKeyType = OTAsymmetricKey::SECP256K1;
            break;
        default :
            newKeyType = OTAsymmetricKey::ERROR_TYPE;
    }
    return newKeyType;
}

void NymParameters::setNymParameterType(NymParameters::NymParameterType theKeytype) {
    nymType_ = theKeytype;
}

Credential::CredentialType NymParameters::credentialType() const {
    return credentialType_;
}

void NymParameters::setCredentialType(Credential::CredentialType theCredentialtype) {
    credentialType_ = theCredentialtype;
}

const std::string& NymParameters::AltLocation() const
{
    return altLocation_;
}

void NymParameters::SetAltLocation(const std::string& location)
{
    altLocation_ = location;
}

#if defined(OT_CRYPTO_SUPPORTED_KEY_RSA)
NymParameters::NymParameters(const int32_t keySize)
    : nymType_(NymParameters::LEGACY),
    credentialType_(Credential::LEGACY),
    nBits_(keySize)
{
}

int32_t NymParameters::keySize() {
    return nBits_;
}

void NymParameters::setKeySize(int32_t keySize) {
    nBits_ = keySize;
}
#endif

} // namespace opentxs
