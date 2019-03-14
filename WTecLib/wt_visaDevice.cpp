#include "wt_visaDevice.h"

#include <unistd.h>

//#define DEBUG

wt_visaDevice::wt_visaDevice(std::string _devName, wt_visaResourceManager* _visaRM)
    : __visaRM(_visaRM), __devName(_devName) {
    // Open the device
    __device = __visaRM->openDevice(__devName);
}

wt_visaDevice::~wt_visaDevice() {
    // Close device
    ViStatus status = viClose(__device);
    checkStatus(status, "Device '" + __devName + "' could not be closed.");
}

int wt_visaDevice::write(std::string _msg) {
    ViUInt32 ret;

#ifdef DEBUG
    printf("\twt_visaDevice::write('%s') [%lu]\n", _msg.c_str(), _msg.size());
#endif

    ViStatus status = viWrite(__device, (ViBuf)_msg.c_str(), _msg.size(), &ret);
    checkStatus(status, "viWrite(" + _msg + ") failed.");
    return ret;
}

std::string wt_visaDevice::read() {
    std::string ret("");
    int len, nto = 0;
    char buf[1024];
    ViStatus status = VI_SUCCESS_MAX_CNT;

    // Read in chunks of 1024 bytes (until VI_SUCCESS)
    while (status == VI_SUCCESS_MAX_CNT) {
        status = viRead(__device, (ViBuf)buf, 1024, (ViUInt32*)&len);
        // On timeout wait retry
        if (status == VI_ERROR_TMO) {
            status = VI_SUCCESS_MAX_CNT;
            // Increase timeout counter and wait...
            nto++;
            if (nto < 5)
                usleep(1e3);    // 1ms
            else if(nto < 10)
                usleep(10e3);   // 10ms
            else if(nto < 15)
                usleep(100e3);  // 10ms
            else
                checkStatus(VI_ERROR_TMO, "viRead max timeout!");

            printf("\twt_visaDevice::read() timeout %i...\n", nto);
            continue;
        }
        checkStatus(status, "viRead failed.");
        ret.append(buf, len);
    }

#ifdef DEBUG
    printf("\twt_visaDevice::read() [%lu] -- %s\n", ret.size(), ret.c_str() );
#endif

    return ret;
}

    /* * * * * * * * * * * * * * * * */
    /*   IEEE488.2 Common Commands   */
    /* * * * * * * * * * * * * * * * */

uint8_t wt_visaDevice::getESE() {
    write("*ESE?");
    std::string msg = read();
    return std::stoi(msg);
}

uint8_t wt_visaDevice::getESR() {
    write("*ESR?");
    std::string msg = read();
    return std::stoi(msg);
}

uint8_t wt_visaDevice::getSRE() {
    write("*SRE?");
    std::string msg = read();
    return std::stoi(msg);
}

uint8_t wt_visaDevice::getSTB() {
    write("*STB?");
    std::string msg = read();
    return std::stoi(msg);
}

std::string wt_visaDevice::IDN() {
    write("*IDN?");
    return read();
}

bool wt_visaDevice::OPC() {
    // Reset OPC bit and issue query
    write("*OPC;*OPC?");
    std::string ret = read();
    return std::stoi(ret);
}

uint8_t wt_visaDevice::TST() {
    write("*TST?");
    std::string msg = read();
    return std::stoi(msg);
}

void wt_visaDevice::checkStatus(ViStatus _stat, std::string _msg) {
    // TODO: Add exceptions
    if (_stat < VI_SUCCESS) {
        printf("\t%s (0x%X)\n", _msg.c_str(), _stat);
    }
    return;
}
