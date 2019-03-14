#ifndef WT_VISA_RESOURCE_MANAGER_H
#define WT_VISA_RESOURCE_MANAGER_H

#include <string>

#include <visa.h>

class wt_visaResourceManager {
public:
    wt_visaResourceManager();
    ~wt_visaResourceManager();

    // Returns unique id string for device
    std::string findDevice(std::string _expr = "?*INSTR");

    // Opens device using a unique id string
    ViSession openDevice(std::string _dev);

private:
    ViSession __visaRM;

    static void checkStatus(ViStatus _stat, std::string _msg = "");
};

extern wt_visaResourceManager gVisaRM;  // Global visa manager

#endif  // !WT_VISA_RESOURCE_MANAGER_H
