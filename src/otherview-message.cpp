// Standard libraries.
#include <vector>
#include <stdexcept>
#include <cstdlib>
#include <iostream>
#include <iostream>
#include <fstream>
#include <string>

// ModShot libraries.
#include "otherview-message.h"
#include "config.h"
#include "debugwriter.h"

// Boost Interprocess.
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/process.hpp>

#ifdef __linux
#include <sys/ipc.h>
#include <sys/shm.h>
#endif

#include <stdio.h>


using namespace boost::process;

#define OSFM_NAMEDPIPE_NAME "OSFMOtherViewPipe_DontTouch"

/*io_service ioService;

void ovm_server_thread() {
    tcp::acceptor acceptor(ioService, tcp::endpoint(tcp::v4(), OSFM_OTHERVIEW_PORT));

    while(true) {
        tcp::socket socket(ioService);
        acceptor.accept(socket);

        boost::asio::streambuf buf;
        boost::system::error_code errorcode;
        while(read(socket, buf, errorcode)) {
            cout << &buf << endl;
            if(errorcode) {
                cout << "Error: " << errorcode << endl;
                break;
            }
        }
    }
}

void ovm_client_thread() {
    tcp::socket socket(ioService);
    tcp::endpoint endpoint(ip::address::from_string("127.0.0.1"), OSFM_OTHERVIEW_PORT);
    socket.connect(endpoint);

    boost::system::error_code errorcode;
    boost::asio::streambuf buf;
    socket.send("Echo!");
    while(read(socket, buf, errorcode)) {
        cout << &buf << endl;
        if(errorcode) {
            cout << "Error: " << errorcode << endl;
        }
    }
}*/

void ovh_write_named_pipe(string message) {
    ofstream fifo(OSFM_NAMEDPIPE_NAME);

    fifo << message << endl;
    fifo.close();
}

void clearPipe() {
    ofstream fifo(OSFM_NAMEDPIPE_NAME);
    fifo.clear();
    fifo.close();
}

string ovh_read_named_pipe() {
    ifstream fifo(OSFM_NAMEDPIPE_NAME);

    string str;
    while(fifo >> str) {
        str += str;
    }
    fifo.close();
    clearPipe();

    return str;
}

OtherViewMessager::OtherViewMessager(const Config &c):
    conf(c)
{
    Debug() << "Interprocessing start.";
    try {
        if (conf.isOtherView) { // Server-mode
            ovh_write_named_pipe("Boo!");
            Debug() << ovh_read_named_pipe();

            //OtherViewMessager::OpenOneShot();
        } else {
            //ovh_read_shared_memory();
            //boost::thread c(ovm_client_thread);
        }
    } catch(exception& e) {
        std::cerr << "[EXCEPTION] " << e.what() << endl;
    }
    Debug() << "If you see this, interprocessing has succeeded.";
    // LowLevelCrashHandler::upload("If you see this, this is not a crash, it's just a test.\n   -s");
}

void openoneshot_thread() {
    ipstream pipeStream;
    child c("./lib/oneshot", std_out > pipeStream);

    string line;
    while(pipeStream && getline(pipeStream, line) && !line.empty())
        cerr << "[Child ModShot] " << line << endl;
}

void OtherViewMessager::OpenOneShot() {
    boost::thread childModShot(openoneshot_thread);
}

void OtherViewMessager::sendMsg(string str) {
    ovh_write_named_pipe(str);
}

string OtherViewMessager::getMsg() {
    if(ovh_read_named_pipe() != "")
        return ovh_read_named_pipe();
    else return nullptr;
}
