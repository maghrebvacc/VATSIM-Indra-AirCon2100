#pragma once
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

namespace Indra
{

// VacsManager handles fire-and-forget WebSocket calls to the VACS Remote API.
// VACS listens on ws://127.0.0.1:6809 and accepts JSON:
//   {"type":"call","callsign":"EGLL_TWR"}
class VacsManager
{
public:
    VacsManager();
    ~VacsManager();

    // Initiates a VACS audio call to the given station callsign.
    // Connects, performs WebSocket handshake, sends the call message, and closes.
    void StartVacsCall(const std::string& station);

private:
    static const char*  kHost;
    static const int    kPort = 6809;

    SOCKET ConnectSocket();
    bool   PerformHandshake(SOCKET sock);
    bool   SendFrame(SOCKET sock, const std::string& payload);

    static std::string  MakeWsKey();
    static std::string  Base64Encode(const unsigned char* data, size_t len);
};

} // namespace Indra
