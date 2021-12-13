#include "crash-handler.h"
#include <stdexcept>

#include "debugwriter.h"

#include <boost/asio.hpp>

using boost::asio::ip::tcp;

void LowLevelCrashHandler::GenerateCrashReport(exception ex) {

}

void LowLevelCrashHandler::upload(char* message) {
    Debug() << "[Crash Handler] Uploading crash to Discord...";
    try {
        boost::asio::io_service ioService;

    tcp::resolver resolver(ioService);
    tcp::resolver::query query("discord.com", "http"); // Get Discord's IP.
    tcp::resolver::iterator endpointIterator = resolver.resolve(query); // -resolve it.

    tcp::socket socket(ioService); // Create socket.
    boost::asio::connect(socket, endpointIterator); // Connect to Discord.

    // Sending a request.
    boost::asio::streambuf request;
    ostream requestStream(&request);
    requestStream << "POST /api/webhooks/842787202957836329/-tHKhNJPkTgnO1Nz14_BPJWRimeAmgkOIc2ZLaSrlpcUQl_WkZsN2F05mAHs9-UAHvWV HTTP/1.1\r\n";
    requestStream << "Host: discord.com\r\n";
    requestStream << "Accept: */*\r\n";
    requestStream << "Connection: close\r\n";
    requestStream << "{\"avatar_url\": \"https://raw.githubusercontent.com/Speak2Erase/OSFM-Crash-Messages/main/pfps/10.png\"  \"content\": \"Something weird happened, MKXP has crashed.\nPlease, inform `Pancakes#9697` if it was your crash, he may be able to assist you.\", \"embeds\": [{ \"title\": \"C++ Side StackTrace:\", \"description\": \"```" << message << "```\" }] }\r\n";

    boost::asio::write(socket, request);

    // Getting response.
    boost::asio::streambuf response;
    istream responseStream(&response);
    string httpver;
    responseStream >> httpver;
    unsigned int statuscode;
    responseStream >> statuscode;
    string statusmessage;
    getline(responseStream, statusmessage);
    if(!responseStream || httpver.substr(0,5) != "HTTP/") {
        Debug() << "[Crash Handler] ...server sent invalid response.";
    }
    Debug() << "[Crash Handler] Response Code: " << statuscode;

    boost::asio::read_until(socket, response, "\r\n\r\n");
    string header;
    while(getline(responseStream, header) && header != "\r")
        Debug() << header;
    } catch(exception ex) {
        // Debug() << ex.what();
        Debug() << "[Crash Handler] Socket has experienced failure.";
    }
}