//
// Created by Rambo.Liu on 2024/9/13.
// ip 地址管理类  通过此类  用来管理是ipv4 还是ip v6

#ifndef AIRCONTROL_ADDRESS_H
#define AIRCONTROL_ADDRESS_H

#include <string>
#include <cstdint>

enum AddressType {
    AT_IPV4 = 0x1,
    AT_IPV6 = 0x2,
};

class Address {

public:
    Address();

    Address(AddressType at);

    Address(AddressType at, const std::string &ip, uint16_t port);

    Address(const Address &addr);

    ~Address();

    void SetType(AddressType at) { _address_type = at; }

    AddressType GetType() { return _address_type; }

    void SetIp(const std::string &ip);

    const std::string &GetIp() { return _ip; }

    void SetAddrPort(uint16_t port) { _port = port; }

    uint16_t GetAddrPort() { return _port; }

    const std::string AsString();

    friend std::ostream &operator<<(std::ostream &out, Address &addr);

    friend bool operator==(const Address &addr1, const Address &addr2);

    static bool IsIpv4(const std::string &ip);

private:
    std::string ToIpv6(const std::string &ip);

    std::string ToIpv4(const std::string &ip);

protected:
    AddressType _address_type;
    std::string _ip;
    uint16_t _port;
};


#endif //AIRCONTROL_ADDRESS_H
