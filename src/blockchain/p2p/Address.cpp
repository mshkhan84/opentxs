// Copyright (c) 2010-2019 The Open-Transactions developers
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "stdafx.hpp"

#include "opentxs/api/crypto/Crypto.hpp"
#include "opentxs/api/crypto/Encode.hpp"
#include "opentxs/api/Core.hpp"
#include "opentxs/core/Data.hpp"
#include "opentxs/core/Identifier.hpp"
#include "opentxs/core/Log.hpp"
#include "opentxs/Bytes.hpp"

#include "internal/api/Api.hpp"
#include "internal/blockchain/p2p/P2P.hpp"

#include "Address.hpp"

// #define OT_METHOD "opentxs::blockchain::p2p::implementation::Address::"

namespace opentxs
{
using ReturnType = blockchain::p2p::implementation::Address;

auto Factory::BlockchainAddress(
    const api::internal::Core& api,
    const blockchain::p2p::Protocol protocol,
    const blockchain::p2p::Network network,
    const Data& bytes,
    const std::uint16_t port,
    const blockchain::Type chain,
    const Time lastConnected,
    const std::set<blockchain::p2p::Service>& services) noexcept
    -> std::unique_ptr<blockchain::p2p::internal::Address>
{
    try {
        return std::make_unique<ReturnType>(
            api,
            ReturnType::DefaultVersion,
            protocol,
            network,
            bytes.Bytes(),
            port,
            chain,
            lastConnected,
            services);
    } catch (const std::exception& e) {
        LogOutput("opentxs::Factory::")(__FUNCTION__)(": ")(e.what()).Flush();

        return {};
    }
}

auto Factory::BlockchainAddress(
    const api::internal::Core& api,
    const proto::BlockchainPeerAddress serialized) noexcept
    -> std::unique_ptr<blockchain::p2p::internal::Address>
{
    try {
        return std::make_unique<ReturnType>(
            api,
            serialized.version(),
            static_cast<blockchain::p2p::Protocol>(serialized.protocol()),
            static_cast<blockchain::p2p::Network>(serialized.network()),
            serialized.address(),
            static_cast<std::uint16_t>(serialized.port()),
            static_cast<blockchain::Type>(serialized.chain()),
            Clock::from_time_t(serialized.time()),
            ReturnType::instantiate_services(serialized));
    } catch (const std::exception& e) {
        LogOutput("opentxs::Factory::")(__FUNCTION__)(": ")(e.what()).Flush();

        return {};
    }
}
}  // namespace opentxs

namespace opentxs::blockchain::p2p::implementation
{
const VersionNumber Address::DefaultVersion{1};

Address::Address(
    const api::internal::Core& api,
    const VersionNumber version,
    const Protocol protocol,
    const Network network,
    const ReadView bytes,
    const std::uint16_t port,
    const blockchain::Type chain,
    const Time lastConnected,
    const std::set<Service>& services) noexcept(false)
    : api_(api)
    , version_(version)
    , id_(calculate_id(api, version, protocol, network, bytes, port, chain))
    , protocol_(protocol)
    , network_(network)
    , bytes_(api.Factory().Data(bytes))
    , port_(port)
    , chain_(chain)
    , previous_last_connected_(lastConnected)
    , previous_services_(services)
    , last_connected_(lastConnected)
    , services_(services)
{
    const auto size = bytes_->size();

    switch (network_) {
        case Network::ipv4: {
            if (sizeof(ip::address_v4::bytes_type) != size) {
                throw std::runtime_error("Incorrect ipv4 bytes");
            }
        } break;
        case Network::ipv6:
        case Network::cjdns: {
            if (sizeof(ip::address_v6::bytes_type) != size) {
                throw std::runtime_error("Incorrect ipv6 bytes");
            }
        } break;
        case Network::onion2: {
            if (10 != size) {
                throw std::runtime_error("Incorrect onion bytes");
            }
        } break;
        case Network::onion3: {
            if (56 != size) {
                throw std::runtime_error("Incorrect onion bytes");
            }
        } break;
        case Network::eep: {
            if (32 != size) {  // TODO replace ths with correct value
                throw std::runtime_error("Incorrect eep bytes");
            }
        } break;
        default: {
            OT_FAIL;
        }
    }
}

Address::Address(const Address& rhs) noexcept
    : api_(rhs.api_)
    , version_(rhs.version_)
    , id_(rhs.id_)
    , protocol_(rhs.protocol_)
    , network_(rhs.network_)
    , bytes_(rhs.bytes_)
    , port_(rhs.port_)
    , chain_(rhs.chain_)
    , previous_last_connected_(rhs.previous_last_connected_)
    , previous_services_(rhs.previous_services_)
    , last_connected_(rhs.last_connected_)
    , services_(rhs.services_)
{
}

auto Address::calculate_id(
    const api::internal::Core& api,
    const VersionNumber version,
    const Protocol protocol,
    const Network network,
    const ReadView bytes,
    const std::uint16_t port,
    const blockchain::Type chain) noexcept -> OTIdentifier
{
    const auto serialized = serialize(
        version,
        protocol,
        network,
        bytes,
        port,
        chain,
        Clock::from_time_t(0),
        {});

    return api.Factory().Identifier(serialized);
}

auto Address::Display() const noexcept -> std::string
{
    std::string output{};

    switch (network_) {
        case Network::ipv4: {
            ip::address_v4::bytes_type bytes{};
            std::memcpy(bytes.data(), bytes_->data(), bytes.size());
            auto address = ip::make_address_v4(bytes);
            output = address.to_string();
        } break;
        case Network::ipv6:
        case Network::cjdns: {
            ip::address_v6::bytes_type bytes{};
            std::memcpy(bytes.data(), bytes_->data(), bytes.size());
            auto address = ip::make_address_v6(bytes);
            output = std::string("[") + address.to_string() + "]";
        } break;
        case Network::onion2:
        case Network::onion3: {
            output =
                std::string(
                    static_cast<const char*>(bytes_->data()), bytes_->size()) +
                ".onion";
        } break;
        case Network::eep: {
            output = api_.Crypto().Encode().DataEncode(bytes_) + ".i2p";
        } break;
        default: {
            OT_FAIL;
        }
    }

    return output + ":" + std::to_string(port_);
}

auto Address::instantiate_services(const SerializedType& serialized) noexcept
    -> std::set<Service>
{
    auto output = std::set<Service>{};

    for (const auto& service : serialized.service()) {
        output.emplace(static_cast<Service>(service));
    }

    return output;
}

auto Address::serialize(
    const VersionNumber version,
    const Protocol protocol,
    const Network network,
    const ReadView bytes,
    const std::uint16_t port,
    const blockchain::Type chain,
    const Time time,
    const std::set<Service>& services) noexcept -> SerializedType
{
    auto output = SerializedType{};
    output.set_version(version);
    output.set_protocol(static_cast<std::uint8_t>(protocol));
    output.set_network(static_cast<std::uint8_t>(network));
    output.set_chain(static_cast<std::uint32_t>(chain));
    output.set_address(bytes.data(), bytes.size());
    output.set_port(port);
    output.set_time(Clock::to_time_t(time));

    for (const auto& service : services) {
        output.add_service(static_cast<std::uint8_t>(service));
    }

    return output;
}

auto Address::Serialize() const noexcept -> SerializedType
{
    auto output = serialize(
        version_,
        protocol_,
        network_,
        bytes_->Bytes(),
        port_,
        chain_,
        last_connected_,
        services_);
    output.set_id(id_->str());

    return output;
}
}  // namespace opentxs::blockchain::p2p::implementation
