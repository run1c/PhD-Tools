#include "RigolDS1000Z.h"

#include <sstream>
#include <unistd.h>

RigolDS1000Z::RigolDS1000Z(std::string _devName) : wt_visaDevice(_devName) {

}

RigolDS1000Z::~RigolDS1000Z() {

}

    /* * * * * * * * * * */
    /* Channel commands  */
    /* * * * * * * * * * */

void RigolDS1000Z::setBWLimit(int _nCh, bool _lim) {
    std::stringstream query;
    query << ":CHAN" << _nCh << ":BWL ";
    if (_lim)
        query << " 20M";
    else
        query << " OFF";
    write(query.str());
    return;
}

void RigolDS1000Z::setCoupling(int _nCh, std::string _coup) {
    std::stringstream query;
    query << ":CHAN" << _nCh << ":COUP " << _coup;
    write(query.str());
    return;
}

void RigolDS1000Z::display(int _nCh, bool _disp) {
    std::stringstream query;
    query << ":CHAN" << _nCh << ":DISP ";
    if (_disp)
        query << " ON";
    else
        query << " OFF";
    write(query.str());
    return;
}

void RigolDS1000Z::invert(int _nCh, bool _inv) {
    std::stringstream query;
    query << ":CHAN" << _nCh << ":INV" ;
    if (_inv)
        query << " ON";
    else
        query << " OFF";
    write(query.str());
    return;
}

void RigolDS1000Z::setOffset(int _nCh, double _offs) {
    std::stringstream query;
    query << ":CHAN" << _nCh << ":OFFS " << _offs;
    write(query.str());
    return;
}

void RigolDS1000Z::setScale(int _nCh, double _scale) {
    std::stringstream query;
    query << ":CHAN" << _nCh << ":SCAL " << _scale;
    write(query.str());
    return;
}

void RigolDS1000Z::setProbe(int _nCh, int _prob) {
    std::stringstream query;
    query << ":CHAN" << _nCh << ":PROB " << _prob;
    write(query.str());
    return;
}

    /* * * * * * * * * * * */
    /*  Measure commands   */
    /* * * * * * * * * * * */

void RigolDS1000Z::setMeasurement(std::string _item, int _ch1, int _ch2) {
    std::stringstream query;
    query << ":MEAS:ITEM " << _item << ",CHAN" << _ch1;
    if (_ch2) query << ",CHAN" << _ch2;
    write(query.str());
    return;
}

double RigolDS1000Z::getMeasurement(std::string _item, int _ch1, int _ch2) {
    std::stringstream query;
    query << ":MEAS:ITEM? " << _item << ",CHAN" << _ch1;
    if (_ch2) query << ",CHAN" << _ch2;
    write(query.str());
    return std::stof( read() );
}

    /* * * * * * * * * * * */
    /*  Timebase commands  */
    /* * * * * * * * * * * */

void RigolDS1000Z::setDelay(double _delay) {
    write( ":TIM:OFFS " + std::to_string(_delay) );
    return;
}

void RigolDS1000Z::setTimebase(double _timeb) {
    write( ":TIM:SCAL " + std::to_string(_timeb) );
    return;
}

    /* * * * * * * * * * * */
    /*  Trigger commands   */
    /* * * * * * * * * * * */

bool RigolDS1000Z::triggered() {
    bool ret = false;
    write(":TRIG:STAT?");
    std::string msg = read();
    if (msg == "TD\n" || msg == "STOP\n")
        ret = true;
    return ret;
}
    /* * * * * * * * * * * */
    /*  Waveform commands  */
    /* * * * * * * * * * * */

void RigolDS1000Z::setSource(int _nCh) {
    write( ":WAV:SOUR CHAN" + std::to_string(_nCh) );
    return;
}

void RigolDS1000Z::setMode(std::string _mode) {
    write( ":WAV:MODE " + _mode );
    return;
}

void RigolDS1000Z::setFormat(std::string _form) {
    write( ":WAV:FORM " + _form );
    return;
}

void RigolDS1000Z::getPreamble() {

    write(":WAV:XINC?");
    __xinc = stof( read() );
    write(":WAV:YINC?");
    __yinc = stof( read() );

    write(":WAV:XOR?");
    __xorig = stof( read() );
    write(":WAV:YOR?");
    __yorig = stof( read() );

    write(":WAV:XREF?");
    __xref = stof( read() );
    write(":WAV:YREF?");
    __yref = stof( read() );

    return;
}

int RigolDS1000Z::getData(std::vector<double> &_time, std::vector<double> &_volt) {
    _time.clear();
    _volt.clear();
    // Send query
    write(":WAV:DATA?");
    std::string data = read();
    // Header contains payload length
    int len = std::stoi( data.substr(2, 9) );

    // Skip header and last character ('\n')
    for (int i = 11; i < (data.size() - 1); i++) {
        double voltage = ( (uint8_t)data.at(i) - __yorig - __yref )*__yinc;
        double time = (i - 11) * __xinc + __xorig;
        _volt.push_back(voltage);
        _time.push_back(time);
    }

    return len;
}

int RigolDS1000Z::readChannel(int _nCh, std::vector<double> &_time, std::vector<double> &_volt) {
    // Set channel and read preamble
    setSource(_nCh);
    getPreamble();
    return getData(_time, _volt);
}

void RigolDS1000Z::checkCh(int _ch) {
    // TODO: Exception
    if (_ch < 1 || _ch > 4) {
        printf("\tInvalid channel number (%i)\n", _ch);
    }
    return;
}

bool RigolDS1000Z::checkValueInList(double _val, double* _list, int _len) {
    bool ret = false;
    // Turn array into vector
    for (int i = 0; i < _len; i++)
        if (_val == _list[i]) ret = true;
    return ret;
}
