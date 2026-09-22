#ifndef SOCK_LIB_SOCK
#define SOCK_LIB_SOCK
#include <SockLib/Helper.hpp>
#include <SockLib/Obj.hpp>
#include <SockLib/Exception.hpp>
#include <string>
#include <string_view>
#include <vector>
#include <chrono>
#include <type_traits>
#include <cassert>

namespace SockLib
{
    class Server;

    class Sock
    {
    public:
        inline static constexpr std::size_t               MAX_SIZE       = 2147483647;
        inline static constexpr std::chrono::milliseconds MAX_TIMEOUT    = std::chrono::milliseconds(2147483647);
        inline static constexpr const char               *LOOPBACK_IPV4_ADDR = "127.0.0.1"; // For SockLib::Server::IPver::IPV4_ONLY
        inline static constexpr const char               *LOOPBACK_IPV6_ADDR = "::1"; // This is for For SockLib::Server::IPver::IPV6_ONLY/BOTH_IPV4N6.
        // "localhost" is defined by the OS configurations. So it's not reliable. For example on Linux Mint you have to manually define it for IPv6.

        static SockLib::Sock connect(const char *address, std::uint16_t port); // Address can be an url, ipv6 and ipv6 and so on.
        void setTimeout(std::chrono::milliseconds duration); // This apply for sending and receiving.
        void setOffTimeout(void); // This apply for sending and receiving.
        void close(void);
        ~Sock(void);

        Sock(const SockLib::Sock &other) = delete;
        SockLib::Sock& operator = (const SockLib::Sock &other) = delete;
        Sock(SockLib::Sock &&other) noexcept;
        SockLib::Sock& operator = (SockLib::Sock &&other) noexcept;   

        template <typename Obj>                              
        void sendObj(const Obj &obj);

        // Can use SockLib::Obj::Str/Bytes.
        template <typename Obj, typename ...ConstructorArgs>
        Obj recvObjDynamic(std::size_t dynamicMaxBytes, ConstructorArgs &&...constructorArgs);

        // Cannot use SockLib::Obj::Str/Bytes.
        template <typename Obj, typename ...ConstructorArgs>
        Obj recvObjStatic(ConstructorArgs &&...constructorArgs);

        void sendInt8    (std::int8_t                            i);
        void sendUint8   (std::uint8_t                           i);
        void sendInt16   (std::int16_t                           i);
        void sendUint16  (std::uint16_t                          i);
        void sendInt32   (std::int32_t                           i);
        void sendUint32  (std::uint32_t                          i);
        void sendInt64   (std::int64_t                           i);
        void sendUint64  (std::uint64_t                          i);
        void sendFloat32 (SockLib::Helper::float32_t             f);
        void sendFloat64 (SockLib::Helper::float64_t             f);
        void sendBytes   (const std::byte *bytes, std::size_t size); // On the network this is specified with a size - [uint32_t: size][byte[] N]
        void sendBytes   (const std::vector<std::byte> &bytes); // On the network this is specified with a size - [uint32_t: size][byte[] N]
        void sendBytes   (std::span<const std::byte> bytes); // On the network this is specified with a size - [uint32_t: size][byte[] N]

        void sendBool  (bool             b); // On the network it's 'std::uint8_t'.
        void sendChar  (char             c); // On the network it's 'std::uint8_t'.
        void sendInt   (int              i); // On the network it's 'std::int32_t'.
        void sendFloat (float            f); // On the network it's 'SockLib::Helper::float32_t'.
        void sendStr   (std::string_view s); // On the network this is specified with a size - [uint32_t: size][byte[] N]

        std::int8_t                recvInt8    (void);
        std::uint8_t               recvUint8   (void);
        std::int16_t               recvInt16   (void);
        std::uint16_t              recvUint16  (void);
        std::int32_t               recvInt32   (void);
        std::uint32_t              recvUint32  (void);
        std::int64_t               recvInt64   (void);
        std::uint64_t              recvUint64  (void);
        SockLib::Helper::float32_t recvFloat32 (void);
        SockLib::Helper::float64_t recvFloat64 (void);
        std::vector<std::byte>     recvBytes   (std::size_t maxBytes); // On the network this is specified with a size - [uint32_t: size][byte[] N]

        bool        recvBool  (void); // On the network it's 'std::uint8_t'.
        char        recvChar  (void); // On the network it's 'std::uint8_t'.
        int         recvInt   (void); // On the network it's 'std::int32_t'.
        float       recvFloat (void); // On the network it's 'SockLib::Helper::float32_t'.
        std::string recvStr   (std::size_t maxBytes); // On the network this is specified with a size - [uint32_t: size][byte[] N]

        void        sendRawAllBytes  (const std::byte *bytes, std::size_t size);
        std::size_t sendRawSomeBytes (const std::byte *bytes, std::size_t size); // Send bytes as the OS can. For size > 0: Sent bytes will be at least one.
        void        recvRawAllBytes  (std::byte       *bytes, std::size_t size);
        std::size_t recvRawSomeBytes (std::byte       *bytes, std::size_t size); // Recieve bytes as the OS can. For size > 0: Recievd bytes will be at least one.

    private:
        Sock(SockLib::Helper::Sock new_socket);
        
        template <typename Obj, typename ...ConstructorArgs>
        Obj recvObjDynamicImpl(std::size_t dynamicBytesLimit, std::size_t &outGainedDynamicBytes, ConstructorArgs &&...constructorArgs);

        SockLib::Helper::Sock m_sock;
        
        friend class SockLib::Server;
    };
}

template <typename Obj>
inline void SockLib::Sock::sendObj(const Obj &obj)
{
    std::apply([this](const auto &...field) {([&] 
    {
        using Type = std::remove_cvref_t<decltype(field)>;

        if constexpr (std::is_same_v<Type, SockLib::Obj::Int8>) {
            this->sendInt8(field);
        }
        else if constexpr (std::is_same_v<Type, SockLib::Obj::Uint8>) {
            this->sendUint8(field);
        }
        else if constexpr (std::is_same_v<Type, SockLib::Obj::Int16>) {
            this->sendInt16(field);
        }
        else if constexpr (std::is_same_v<Type, SockLib::Obj::Uint16>) {
            this->sendUint16(field);
        }
        else if constexpr (std::is_same_v<Type, SockLib::Obj::Int32>) {
            this->sendInt32(field);
        }
        else if constexpr (std::is_same_v<Type, SockLib::Obj::Uint32>) {
            this->sendUint32(field);
        }
        else if constexpr (std::is_same_v<Type, SockLib::Obj::Int64>) {
            this->sendInt64(field);
        }
        else if constexpr (std::is_same_v<Type, SockLib::Obj::Uint64>) {
            this->sendUint64(field);
        }
        else if constexpr (std::is_same_v<Type, SockLib::Obj::Float32>) {
            this->sendFloat32(field);
        }
        else if constexpr (std::is_same_v<Type, SockLib::Obj::Float64>) {
            this->sendFloat64(field);
        }
        else if constexpr (std::is_same_v<Type, SockLib::Obj::Bytes>) {
            this->sendBytes(field);
        }
        else if constexpr (std::is_same_v<Type, SockLib::Obj::BytesSpan>) {
            this->sendBytes(field);
        }
        else if constexpr (std::is_same_v<Type, SockLib::Obj::Bool>) {
            this->sendBool(field);
        }
        else if constexpr (std::is_same_v<Type, SockLib::Obj::Char>) {
            this->sendChar(field);
        }
        else if constexpr (std::is_same_v<Type, SockLib::Obj::Int>) {
            this->sendInt(field);
        }
        else if constexpr (std::is_same_v<Type, SockLib::Obj::Float>) {
            this->sendFloat(field);
        }
        else if constexpr (std::is_same_v<Type, SockLib::Obj::Str>) {
            this->sendStr(field);
        }
        else if constexpr (std::is_same_v<Type, SockLib::Obj::StrView>) {
            this->sendStr(field);
        }
        else {
            this->sendObj(field); // Attempt to send nested object.
        }

    }(),...); }, obj._SockLibObjToTupleConst());
}
template <typename Obj, typename ...ConstructorArgs>
inline Obj SockLib::Sock::recvObjDynamic(std::size_t dynamicMaxBytes, ConstructorArgs &&...constructorArgs)
{ assert(SockLib::Sock::MAX_SIZE >= dynamicMaxBytes);
    
    std::size_t discardMe = 0;
    return this->recvObjDynamicImpl<Obj>(dynamicMaxBytes, discardMe, std::forward<ConstructorArgs>(constructorArgs)...);
}
template<typename Obj, typename ...ConstructorArgs>
inline Obj SockLib::Sock::recvObjStatic(ConstructorArgs &&...constructorArgs)
{
    Obj obj(std::forward<ConstructorArgs>(constructorArgs)...);

    std::apply([&](auto &...field) {([&]
    {
        using Type = std::remove_reference_t<decltype(field)>;

        if constexpr (std::is_same_v<Type, SockLib::Obj::Int8>) {
            field = this->recvInt8();
        }
        else if constexpr (std::is_same_v<Type, SockLib::Obj::Uint8>) {
            field = this->recvUint8();
        }
        else if constexpr (std::is_same_v<Type, SockLib::Obj::Int16>) {
            field = this->recvInt16();
        }
        else if constexpr (std::is_same_v<Type, SockLib::Obj::Uint16>) {
            field = this->recvUint16();
        }
        else if constexpr (std::is_same_v<Type, SockLib::Obj::Int32>) {
            field = this->recvInt32();
        }
        else if constexpr (std::is_same_v<Type, SockLib::Obj::Uint32>) {
            field = this->recvUint32();
        }
        else if constexpr (std::is_same_v<Type, SockLib::Obj::Int64>) {
            field = this->recvInt64();
        }
        else if constexpr (std::is_same_v<Type, SockLib::Obj::Uint64>) {
            field = this->recvUint64();
        }
        else if constexpr (std::is_same_v<Type, SockLib::Obj::Float32>) {
            field = this->recvFloat32();
        }
        else if constexpr (std::is_same_v<Type, SockLib::Obj::Float64>) {
            field = this->recvFloat64();
        }
        else if constexpr (std::is_same_v<Type, SockLib::Obj::Bool>) {
            field = this->recvBool();
        }
        else if constexpr (std::is_same_v<Type, SockLib::Obj::Char>) {
            field = this->recvChar();
        }
        else if constexpr (std::is_same_v<Type, SockLib::Obj::Int>) {
            field = this->recvInt();
        }
        else if constexpr (std::is_same_v<Type, SockLib::Obj::Float>) {
            field = this->recvFloat();
        }
        else {
            field = this->recvObjStatic<Type>(); // Attempt to receive nested object.
        }

    }(), ...); }, obj._SockLibObjToTupleNoneConst());

    return obj;
}

template <typename Obj, typename ...ConstructorArgs>
inline Obj SockLib::Sock::recvObjDynamicImpl(std::size_t dynamicMaxBytes, std::size_t &outGainedDynamicBytes, ConstructorArgs &&...constructorArgs)
{ assert(SockLib::Sock::MAX_SIZE >= dynamicMaxBytes);

    Obj obj(std::forward<ConstructorArgs>(constructorArgs)...);
    std::size_t gainedDynamicBytes = 0;

    std::apply([&](auto &...field) {([&] 
    {
        using Type = std::remove_reference_t<decltype(field)>;
        if (gainedDynamicBytes > dynamicMaxBytes) {
            this->close();
            throw SockLib::Exception(std::format("Gained object size='{}' exceeds SockLib::Sock::MAX_SIZE", gainedDynamicBytes));
        }

        if constexpr (std::is_same_v<Type, SockLib::Obj::Int8>) {
            field = this->recvInt8();
        }
        else if constexpr (std::is_same_v<Type, SockLib::Obj::Uint8>) {
            field = this->recvUint8();
        }
        else if constexpr (std::is_same_v<Type, SockLib::Obj::Int16>) {
            field = this->recvInt16();
        }
        else if constexpr (std::is_same_v<Type, SockLib::Obj::Uint16>) {
            field = this->recvUint16();
        }
        else if constexpr (std::is_same_v<Type, SockLib::Obj::Int32>) {
            field = this->recvInt32();
        }
        else if constexpr (std::is_same_v<Type, SockLib::Obj::Uint32>) {
            field = this->recvUint32();
        }
        else if constexpr (std::is_same_v<Type, SockLib::Obj::Int64>) {
            field = this->recvInt64();
        }
        else if constexpr (std::is_same_v<Type, SockLib::Obj::Uint64>) {
            field = this->recvUint64();
        }
        else if constexpr (std::is_same_v<Type, SockLib::Obj::Float32>) {
            field = this->recvFloat32();
        }
        else if constexpr (std::is_same_v<Type, SockLib::Obj::Float64>) {
            field = this->recvFloat64();
        }
        else if constexpr (std::is_same_v<Type, SockLib::Obj::Bytes>) {
            field = this->recvBytes(dynamicMaxBytes - gainedDynamicBytes);
            gainedDynamicBytes += field.size();
        }
        else if constexpr (std::is_same_v<Type, SockLib::Obj::Bool>) {
            field = this->recvBool();
        }
        else if constexpr (std::is_same_v<Type, SockLib::Obj::Char>) {
            field = this->recvChar();
        }
        else if constexpr (std::is_same_v<Type, SockLib::Obj::Int>) {
            field = this->recvInt();
        }
        else if constexpr (std::is_same_v<Type, SockLib::Obj::Float>) {
            field = this->recvFloat();
        }
        else if constexpr (std::is_same_v<Type, SockLib::Obj::Str>) {
            field = this->recvStr(dynamicMaxBytes - gainedDynamicBytes);
            gainedDynamicBytes += field.size();
        }
        else {
            // This assumes the object is dynamic.
            // But it can be static and still work.
            field = this->recvObjDynamicImpl<Type>(dynamicMaxBytes - gainedDynamicBytes, outGainedDynamicBytes); // Attempt to receive nested object.
            gainedDynamicBytes += outGainedDynamicBytes;
        }

    }(),...); }, obj._SockLibObjToTupleNoneConst());

    outGainedDynamicBytes = gainedDynamicBytes;
    return obj;
}
#endif // SOCK_LIB_SOCK