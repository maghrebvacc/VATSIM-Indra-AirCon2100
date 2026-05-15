#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#include "Storage.h"

namespace Indra
{

std::string moduleDirectory()
{
    HMODULE module = nullptr;
    char path[MAX_PATH] = {};
    GetModuleHandleExA(
        GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
        GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        reinterpret_cast<LPCSTR>(&moduleDirectory), &module);
    GetModuleFileNameA(module, path, MAX_PATH);
    std::string result(path);
    std::size_t slash = result.find_last_of("\\/");
    return slash == std::string::npos ? "." : result.substr(0, slash);
}

std::string dataDirectory()
{
    std::string dir = moduleDirectory();
    std::size_t slash = dir.find_last_of("\\/");
    std::string leaf = slash == std::string::npos ? dir : dir.substr(slash + 1);
    if (_stricmp(leaf.c_str(), "build") == 0 && slash != std::string::npos)
        return dir.substr(0, slash);
    return dir;
}

std::string readFile(const std::string &path)
{
    std::ifstream file(path.c_str(), std::ios::in | std::ios::binary);
    if (!file) return "";
    std::ostringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

bool jsonString(const std::string &obj, const char *key, std::string &out)
{
    std::string token = std::string("\"") + key + "\"";
    std::size_t p = obj.find(token);
    if (p == std::string::npos) return false;
    p = obj.find(':', p);
    if (p == std::string::npos) return false;
    p = obj.find('"', p);
    if (p == std::string::npos) return false;
    std::size_t e = obj.find('"', p + 1);
    if (e == std::string::npos) return false;
    out = obj.substr(p + 1, e - p - 1);
    return true;
}

bool jsonNumber(const std::string &obj, const char *key, double &out)
{
    std::string token = std::string("\"") + key + "\"";
    std::size_t p = obj.find(token);
    if (p == std::string::npos) return false;
    p = obj.find(':', p);
    if (p == std::string::npos) return false;
    ++p;
    while (p < obj.size() && isspace(static_cast<unsigned char>(obj[p]))) ++p;
    char *end = nullptr;
    out = strtod(obj.c_str() + p, &end);
    return end && end != obj.c_str() + p;
}

std::map<std::string, std::string>              g_metars;
std::vector<std::string>                        g_messages;
std::map<std::string, std::vector<ChatMessage>> g_chatHistory;

bool g_newMessageArrived = false;

void rememberMessage(const std::string &value)
{
    g_messages.push_back(value);
    if (g_messages.size() > 30)
        g_messages.erase(g_messages.begin());
}

void addChatMessage(const std::string &sender,
                    const std::string &receiver,
                    const std::string &msg,
                    bool fromMe)
{
    ChatMessage cm;
    cm.sender   = sender;
    cm.receiver = receiver;
    cm.message  = msg;
    cm.fromMe   = fromMe;
    char timeBuf[20];
    SYSTEMTIME st;
    GetLocalTime(&st);
    snprintf(timeBuf, sizeof(timeBuf), "%02d:%02d", st.wHour, st.wMinute);
    cm.time = timeBuf;
    std::string contactKey = fromMe ? receiver : sender;
    g_chatHistory[contactKey].push_back(cm);
    if (!fromMe) g_newMessageArrived = true;
}

std::vector<ViewDef> loadViewsJson(const std::string &filename)
{
    std::vector<ViewDef> views;
    std::string json = readFile(dataDirectory() + "\\" + filename);
    std::size_t p = 0;
    while ((p = json.find('{', p)) != std::string::npos)
    {
        std::size_t e = json.find('}', p + 1);
        if (e == std::string::npos) break;
        std::string obj = json.substr(p, e - p + 1);
        ViewDef v;
        double  n = 0.0;
        jsonString(obj, "button", v.button);
        jsonString(obj, "name",   v.name);
        if (jsonNumber(obj, "centerLat", n)) { v.centerLat = n; v.hasCenter = true; }
        if (jsonNumber(obj, "centerLon", n))   v.centerLon = n;
        if (jsonNumber(obj, "zoomNm",    n))   v.zoomNm    = static_cast<int>(n);
        if (!v.button.empty()) views.push_back(v);
        p = e + 1;
    }
    return views;
}

void saveViewToJson(const std::string &button, const ViewDef &view)
{
    std::string          path  = dataDirectory() + "\\indra_saved_views.json";
    std::vector<ViewDef> views = loadViewsJson();
    bool found = false;
    for (auto &v : views)
        if (v.button == button) { v = view; found = true; break; }
    if (!found) views.push_back(view);
    std::ofstream file(path.c_str());
    if (!file) return;
    file << "[\n";
    for (std::size_t i = 0; i < views.size(); ++i)
    {
        const auto &v = views[i];
        file << "{\"button\":\"" << v.button << "\",\"name\":\"" << v.name
             << "\",\"zoomNm\":" << v.zoomNm;
        if (v.hasCenter)
            file << ",\"centerLat\":" << v.centerLat
                 << ",\"centerLon\":" << v.centerLon;
        file << "}";
        if (i + 1 < views.size()) file << ",";
        file << "\n";
    }
    file << "]\n";
}


std::vector<Contact> loadContactsJson(const std::string &filename)
{
    std::vector<Contact> result;

    std::string json = readFile(dataDirectory() + "\\" + filename);
    if (json.empty()) return result;

    std::string arrayToken = "\"contacts\"";
    std::size_t arrayPos = json.find(arrayToken);
    if (arrayPos == std::string::npos) return result;

    std::size_t bracketOpen = json.find('[', arrayPos);
    if (bracketOpen == std::string::npos) return result;

    std::size_t bracketClose = json.find(']', bracketOpen);
    if (bracketClose == std::string::npos) return result;

    std::string arrayBody = json.substr(bracketOpen, bracketClose - bracketOpen + 1);

    std::size_t p = 0;
    while ((p = arrayBody.find('{', p)) != std::string::npos)
    {
        std::size_t e = arrayBody.find('}', p + 1);
        if (e == std::string::npos) break;

        std::string obj = arrayBody.substr(p, e - p + 1);
        Contact c;
        if (jsonString(obj, "name",    c.name)   &&
            jsonString(obj, "station", c.station) &&
            !c.name.empty() && !c.station.empty())
        {
            result.push_back(c);
        }
        p = e + 1;
    }
    return result;
}


void vacsCall(const std::string &station)
{
    static const char kHost[] = "127.0.0.1";
    static const int  kPort   = 6809;

    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) return;

    DWORD timeout = 1000;
    setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, reinterpret_cast<const char*>(&timeout), sizeof(timeout));
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<const char*>(&timeout), sizeof(timeout));

    sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port   = htons(static_cast<u_short>(kPort));
    inet_pton(AF_INET, kHost, &addr.sin_addr);

    if (connect(sock, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) != 0)
    {
        closesocket(sock);
        return;
    }

    // WebSocket upgrade
    char key[25];
    snprintf(key, sizeof(key), "%08X%08X==", GetTickCount(), GetTickCount() ^ 0xDEADBEEF);
    char req[512];
    snprintf(req, sizeof(req),
        "GET / HTTP/1.1\r\nHost: %s:%d\r\n"
        "Upgrade: websocket\r\nConnection: Upgrade\r\n"
        "Sec-WebSocket-Key: %s\r\nSec-WebSocket-Version: 13\r\n\r\n",
        kHost, kPort, key);
    send(sock, req, static_cast<int>(strlen(req)), 0);

    char resp[512] = {};
    recv(sock, resp, sizeof(resp) - 1, 0);
    if (!strstr(resp, "101")) { closesocket(sock); return; }

    // Masked text frame: {"type":"call","callsign":"<station>"}
    std::string payload = "{\"type\":\"call\",\"callsign\":\"" + station + "\"}";
    size_t len = payload.size();

    std::vector<unsigned char> frame;
    frame.push_back(0x81);
    frame.push_back(static_cast<unsigned char>(0x80 | (len <= 125 ? len : 126)));
    if (len > 125) { frame.push_back((len >> 8) & 0xFF); frame.push_back(len & 0xFF); }

    unsigned char mask[4] = { 0xAB, 0xCD, 0xEF, 0x01 };
    for (auto m : mask) frame.push_back(m);
    for (size_t i = 0; i < len; ++i)
        frame.push_back(static_cast<unsigned char>(payload[i]) ^ mask[i % 4]);

    send(sock, reinterpret_cast<const char*>(frame.data()), static_cast<int>(frame.size()), 0);
    recv(sock, resp, sizeof(resp) - 1, 0);
    closesocket(sock);
}

} // namespace Indra
