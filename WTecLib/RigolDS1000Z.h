#ifndef WT_RIGOL_DS1000Z_H
#define WT_RIGOL_DS1000Z_H

#include <string>
#include <vector>

#include "wt_visaDevice.h"

class RigolDS1000Z : public wt_visaDevice {
public:
    RigolDS1000Z(std::string _devName);
    ~RigolDS1000Z();

    /* * * * * * * * * * * * */
    /*    General Commands   */
    /* * * * * * * * * * * * */

    // Auto setting function
    void autoscale() { write(":AUT"); }

    // Clear screen
    void clearScreen() { write(":CLE"); }

    // Run/Stop data capture
    void run() { write(":RUN"); }
    void stop() { write(":STOP"); }

    // Single trigger mode
    void single() { write(":SING"); }

    // Force trigger signal
    void force() { write(":TFOR"); }

    /* * * * * * * * * * */
    /* Channel commands  */
    /* * * * * * * * * * */

    void setBWLimit(int _nCh, bool _lim);

    void setCoupling(int _nCh, std::string _coup);

    void display(int _nCh, bool _disp);

    void invert(int _nCh, bool _inv);

    void setOffset(int _nCh, double _offs);

    void setScale(int _nCh, double _scale);

    void setProbe(int _nCh, int _prob);

    /* * * * * * * * * * * */
    /*  Measure commands   */
    /* * * * * * * * * * * */

    void clearMeasurements() { write(":MEAS:CLE ALL"); }

    void setMeasurement(std::string _item, int _ch1, int _ch2 = 0);

    double getMeasurement(std::string _item, int _ch1, int _ch2 = 0);

    /* * * * * * * * * * * */
    /*  Timebase commands  */
    /* * * * * * * * * * * */

    void setDelay(double _delay);

    void setTimebase(double _timeb);

    /* * * * * * * * * * * */
    /*  Trigger commands   */
    /* * * * * * * * * * * */

    bool triggered();

    /* * * * * * * * * * * */
    /*  Waveform commands  */
    /* * * * * * * * * * * */

    void setSource(int _nCh);

    void setMode(std::string _mode);

    void setFormat(std::string _form);

    // Read waveform preamble
    void getPreamble();

    // Read waveform data
    int getData(std::vector<double> &_time, std::vector<double> &_volt);

    // Read waveform data from channel (setSource + getPreamble + getData)
    int readChannel(int _nCh, std::vector<double> &_time, std::vector<double> &_volt);

private:
    double __xinc, __yinc, __xorig, __yorig, __xref, __yref;

    void checkCh(int _ch);
    bool checkValueInList(double _val, double* _list, int _len);
};


#endif  // !WT_RIGOL_DS1000Z_H
