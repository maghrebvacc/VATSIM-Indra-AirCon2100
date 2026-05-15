#include "VacsManager.h"
#include <sstream>
#include <vector>
#include <random>
#include <cstring>

namespace Indra
{

const char* VacsManager::kHost = "127.0.0.1";

// ---------------------------------------------------------------------------
// Base64 (needed for the Sec-WebSocket-Key header in the HTTP upgrade request)
// ---------------------------------------------------------------------------

static const char kB64Chars[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

std::string VacsManager::Base64Encode(const unsigned char* data, size_t len)
{
    std::string out;
    out.reserve(((len + 2) / 3) * 4);
    for (size_t i = 0; i < len; i += 3)
    {
        unsigned int b =  (static_cast<unsigned int>(data[i]) << 16)
                        | (i + 1 < len ? static_cast<unsigned int>(data[i + 1]) << 8 : 0u)
                        | (i + 2 < len ? static_cast<unsigned int>(data[i + 2])      : 0u);
        out += kB64Chars[(b >> 18) & 0x3F];
        out += kB64Chars[(b >> 12) & 0x3F];
        out += (i + 1 < len) ? kB64Chars[(b >> 6) & 0x3F] : '=';
        out += (i + 2 < len) ? kB64Chars[b        & 0x3F] : '=';
    }
    return out;
}

std::string VacsManager::MakeWsKey()
{
    std::mt19937 rng(static_cast<unsigned>(GetTickCount()));
    std::uniform_int_distribution<int> dist(0, 255);
    unsigned char bytes[16];
    for (auto& b : bytes)
        b = static_cast<unsigned char>(dist(rng));
    return Base64Encode(bytes, 16);
}

// ---------------------------------------------------------------------------
// Lifecycle
// ---------------------------------------------------------------------------

VacsManager::VacsManager()
{
    WSADATA wsa = {};
    WSAStartup(MAKEWORD(2, 2), &wsa);
}

VacsManager::~VacsManager()
{
    WSACleanup();
}

// ---------------------------------------------------------------------------
// Connection helpers
// ---------------------------------------------------------------------------

SOCKET VacsManager::ConnectSocket()
{
    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET)
        return INVALID_SOCKET;

    // Set a short connect timeout so a missing VACS doesn't hang EuroScope
    DWORD timeout = 1000; // ms
    setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO,
               reinterpret_cast<const char*>(&timeout), sizeof(timeout));
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO,
               reinterpret_cast<const char*>(&timeout), sizeof(timeout));

    sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port   = htons(static_cast<u_short>(kPort));
    inet_pton(AF_INET, kHost, &addr.sin_addr);

    if (connect(sock, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) != 0)
    {
        closesocket(sock);
        return INVALID_SOCKET;
    }
    return sock;
}

bool VacsManager::PerformHandshake(SOCKET sock)
{
    std::string key = MakeWsKey();

    std::ostringstream req;
    req << "GET / HTTP/1.1\r\n"
        << "Host: " << kHost << ":" << kPort << "\r\n"
        << "Upgrade: websocket\r\n"
        << "Connection: Upgrade\r\n"
        << "Sec-WebSocket-Key: " << key << "\r\n"
        << "Sec-WebSocket-Version: 13\r\n"
        << "\r\n";

    std::string reqStr = req.str();
    if (send(sock, reqStr.c_str(), static_cast<int>(reqStr.size()), 0) == SOCKET_ERROR)
        return false;

    // Read just enough to confirm "101 Switching Protocols"
    char buf[1024] = {};
    recv(sock, buf, sizeof(buf) - 1, 0);
    return strstr(buf, "101") != nullptr;
}

// Builds a masked RFC-6455 text frame (opcode 0x1) with a 4-byte mask.
bool VacsManager::SendFrame(SOCKET sock, const std::string& payload)
{
    std::vector<unsigned char> frame;
    frame.reserve(payload.size() + 10);

    // FIN=1, RSV=0, opcode=1 (text)
    frame.push_back(0x81);

    size_t len = payload.size();
    if (len > 65535)
        return false; // We never send a payload this large

    // MASK bit set (required for client->server), payload length
    if (len <= 125)
    {
        frame.push_back(0x80 | static_cast<unsigned char>(len));
    }
    else
    {
        frame.push_back(0x80 | 126);
        frame.push_back(static_cast<unsigned char>((len >> 8) & 0xFF));
        frame.push_back(static_cast<unsigned char>(len & 0xFF));
    }

    // 4-byte masking key
    std::mt19937 rng(static_cast<unsigned>(GetTickCount()));
    std::uniform_int_distribution<int> dist(0, 255);
    unsigned char mask[4];
    for (auto& m : mask)
    {
        m = static_cast<unsigned char>(dist(rng));
        frame.push_back(m);
    }

    // Masked payload
    for (size_t i = 0; i < len; ++i)
        frame.push_back(static_cast<unsigned char>(payload[i]) ^ mask[i % 4]);

    return send(sock, reinterpret_cast<const char*>(frame.data()),
                static_cast<int>(frame.size()), 0) != SOCKET_ERROR;
}

// ---------------------------------------------------------------------------
// Public API
// ---------------------------------------------------------------------------

void VacsManager::StartVacsCall(const std::string& station)
{
    // Build the VACS Remote API JSON payload
    std::string json = "{\"type\":\"call\",\"callsign\":\"" + station + "\"}";

    SOCKET sock = ConnectSocket();
    if (sock == INVALID_SOCKET)
        return;

    if (!PerformHandshake(sock))
    {
        closesocket(sock);
        return;
    }

    SendFrame(sock, json);

    // Brief read to let VACS acknowledge; then close (fire-and-forget)
    char ack[256] = {};
    recv(sock, ack, sizeof(ack) - 1, 0);
    closesocket(sock);
}

} // namespace Indra
