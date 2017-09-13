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

#include "opentxs/core/stdafx.hpp"

#include "opentxs/client/NymData.hpp"

#include "opentxs/contact/Contact.hpp"
#include "opentxs/contact/ContactData.hpp"
#include "opentxs/core/crypto/PaymentCode.hpp"
#include "opentxs/core/Identifier.hpp"
#include "opentxs/core/Log.hpp"
#include "opentxs/core/Nym.hpp"
#include "opentxs/core/String.hpp"

#define OT_METHOD "opentxs::NymData::"

namespace opentxs
{
NymData::NymData(const std::shared_ptr<Nym>& nym)
    : nym_(nym)
{
}

bool NymData::AddPaymentCode(
    const std::string& code,
    const std::uint32_t currency,
    const bool primary,
    const bool active)
{
    return AddPaymentCode(
        code,
        static_cast<const proto::ContactItemType>(currency),
        primary,
        active);
}

bool NymData::AddPaymentCode(
    const std::string& code,
    const proto::ContactItemType currency,
    const bool primary,
    const bool active)
{
    class PaymentCode paymentCode(code);

    if (false == paymentCode.VerifyInternally()) {
        otErr << OT_METHOD << __FUNCTION__ << ": Invalid payment code."
              << std::endl;
    }

    return nym().AddPaymentCode(paymentCode, currency, primary, active);
}

bool NymData::AddPreferredOTServer(const std::string& id, const bool primary)
{
    return nym().AddPreferredOTServer(Identifier(id), primary);
}

const ContactData& NymData::data() const
{
    OT_ASSERT(nym_);

    return nym_->Claims();
}

Nym& NymData::nym()
{
    OT_ASSERT(nym_);

    return *nym_;
}

std::string NymData::PaymentCode(const std::uint32_t currency) const
{
    return Contact::PaymentCode(
        data(), static_cast<proto::ContactItemType>(currency));
}

std::string NymData::PaymentCode(const proto::ContactItemType currency) const
{
    return Contact::PaymentCode(data(), currency);
}

std::string NymData::PreferredOTServer() const
{
    return String(data().PreferredOTServer()).Get();
}

std::string NymData::PrintContactData() const
{
    return ContactData::PrintContactData(data().Serialize(true));
}

proto::ContactItemType NymData::Type() const { return data().Type(); }

bool NymData::Valid() const { return bool(nym_); }
}  // namespace opentxs
