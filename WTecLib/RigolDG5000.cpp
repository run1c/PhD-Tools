#include "RigolDG5000.h"

#include <sstream>
#include <unistd.h>

RigolDG5000::RigolDG5000(std::string _devName) : wt_visaDevice(_devName) {

}

RigolDG5000::~RigolDG5000() {

}

    /* * * * * * * * * * * */
    /*   Output Commands   */
    /* * * * * * * * * * * */

void RigolDG5000::enable(int _nCh, bool _en) {
    std::stringstream query;
    query << ":OUTP" << _nCh;
    if (_en)
        query << " ON";
    else
        query << " OFF";
    write(query.str());
    return;
}

void RigolDG5000::setImpedance(int _nCh, std::string _imp) {
    std::stringstream query;
    query << ":OUTP" << _nCh << ":IMP " << _imp;
    write(query.str());
    return;
}

void RigolDG5000::invert(int _nCh, bool _inv) {
    std::stringstream query;
    query << ":OUTP" << _nCh << ":POL ";
    if (_inv)
        query << "INV";
    else
        query << "NORM";
    write(query.str());
    return;
}

void RigolDG5000::setAttenuation(int _nCh, int _att) {
    std::stringstream query;
    query << ":OUTP" << _nCh << ":ATT " << _att << "X";
    write(query.str());
    return;
}

    /* * * * * * * * * * * */
    /*   Source Commands   */
    /* * * * * * * * * * * */

void RigolDG5000::setSine(int _nCh, double _f, double _a, double _offs, double _ph) {
    std::stringstream query;
    query << ":SOUR" << _nCh << ":APPL:SIN " << _f << "," << _a << "," << _offs << "," << _ph;
    write(query.str());
    return;
}

void RigolDG5000::setSquare(int _nCh, double _f, double _a, double _offs, double _ph) {
    std::stringstream query;
    query << ":SOUR" << _nCh << ":APPL:SQU " << _f << "," << _a << "," << _offs << "," << _ph;
    write(query.str());
    return;
}

void RigolDG5000::setRamp(int _nCh, double _f, double _a, double _offs, double _ph) {
    std::stringstream query;
    query << ":SOUR" << _nCh << ":APPL:RAMP " << _f << "," << _a << "," << _offs << "," << _ph;
    write(query.str());
    return;
}

void RigolDG5000::setPulse(int _nCh, double _f, double _a, double _offs, double _delay) {
    std::stringstream query;
    query << ":SOUR" << _nCh << ":APPL:PULS " << _f << "," << _a << "," << _offs << "," << _delay;
    write(query.str());
    return;
}

void RigolDG5000::setNoise(int _nCh, double _a, double _offs) {
    std::stringstream query;
    query << ":SOUR" << _nCh << ":APPL:NOIS " << _a << "," << _offs;
    write(query.str());
    return;
}

void RigolDG5000::enableBurst(int _nCh, bool _en) {
    std::stringstream query;
    query << ":SOUR" << _nCh << ":BURS ";
    if (_en)
        query << "ON";
    else
        query << "OFF";
    write(query.str());
    return;
}

void RigolDG5000::setBurst(int _nCh, int _n, double _delay, double _ph, std::string _trg) {
    write(":SOUR" + std::to_string(_nCh) + ":BURS:TRIG:SOUR " + _trg);
    write(":SOUR" + std::to_string(_nCh) + ":BURS:MODE TRIG");

    write(":SOUR" + std::to_string(_nCh) + ":BURS:NCYC " + std::to_string(_n));
    write(":SOUR" + std::to_string(_nCh) + ":BURS:TDE " + std::to_string(_delay));
    write(":SOUR" + std::to_string(_nCh) + ":BURS:PHAS " + std::to_string(_ph));
    return;
}
