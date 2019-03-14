#include "wt_visaResourceManager.h"

//#define DEBUG

wt_visaResourceManager gVisaRM;

wt_visaResourceManager::wt_visaResourceManager() {
    ViStatus status = viOpenDefaultRM(&__visaRM);
    checkStatus(status, "Could not open Default Resource Manager.");
}

wt_visaResourceManager::~wt_visaResourceManager() {
    ViStatus status = viClose(__visaRM);
    checkStatus(status, "Could not close Default Resource Manager.");
}

std::string wt_visaResourceManager::findDevice(std::string _expr) {
    char devName[VI_FIND_BUFLEN];
    // Find device
    ViStatus status = viFindRsrc(__visaRM, _expr.c_str(), VI_NULL, VI_NULL, devName);
    checkStatus(status, "Resource '" + _expr + "' not found.");
    return std::string(devName);
}

ViSession wt_visaResourceManager::openDevice(std::string _dev) {
    ViSession device = 0;
    // Open divice
    ViStatus status = viOpen(__visaRM, _dev.c_str(), VI_NULL, VI_NULL, &device);
    checkStatus(status, "Device '" + _dev + "' could not be opened.");
    return device;
}

void wt_visaResourceManager::checkStatus(ViStatus _stat, std::string _msg) {
    // TODO: Add exceptions
    if (_stat < VI_SUCCESS) {
        printf("\t%s (0x%X)\n", _msg.c_str(), _stat);
    }
    return;
}
