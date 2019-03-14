#ifndef WT_VISA_DEVICE_H
#define WT_VISA_DEVICE_H

#include <string>

#include "wt_visaResourceManager.h"

class wt_visaDevice {
public:
    wt_visaDevice(std::string _devName, wt_visaResourceManager* __visaRM = &gVisaRM);
    ~wt_visaDevice();

    // Write message to device, returns number of bytes written
    int write(std::string _msg);

    // Read message from device, returns message
    std::string read();

    /* * * * * * * * * * * * * * * * */
    /*   IEEE488.2 Common Commands   */
    /* * * * * * * * * * * * * * * * */

    // Access Event Status Enable Register
    void setESE(uint8_t _reg) { write("*ESE " + std::to_string(_reg) ); }
    uint8_t getESE();

    // Access Event Status Register
    void setESR(uint8_t _reg) { write("*ESR " + std::to_string(_reg) ); }
    uint8_t getESR();

    // Access Service Request Enable Register
    void setSRE(uint8_t _reg) { write("*SRE " + std::to_string(_reg) ); }
    uint8_t getSRE();

    // Access Status Byte Register
    void setSTB(uint8_t _reg) { write("*STB " + std::to_string(_reg) ); }
    uint8_t getSTB();

    // Get ID string
    std::string IDN();

    // Query operation complete bit
    bool OPC();

    // Restore default state
    void RST() { write("*RST"); }

    // Clear event registers and error queue
    void CLS() { write("*CLS"); }

    // Perform self-test
    uint8_t TST();

    // Wait for operation to finish
    void WAI() { write("*WAI"); };

private:
    ViSession __device;
    wt_visaResourceManager* __visaRM;
    std::string __devName;

    static void checkStatus(ViStatus _stat, std::string _msg = "");
};
#endif  // !WT_VISA_DEVICE_H
