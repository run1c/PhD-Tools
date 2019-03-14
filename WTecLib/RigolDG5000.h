#ifndef WT_RIGOL_DG5000_H
#define WT_RIGOL_DG5000_H

#include <string>

#include "wt_visaDevice.h"

class RigolDG5000 : public wt_visaDevice {
public:
    RigolDG5000(std::string _devName);
    ~RigolDG5000();

    /* * * * * * * * * * * * */
    /*    General Commands   */
    /* * * * * * * * * * * * */

    void trigger() { write("*TRG"); }

    /* * * * * * * * * * * */
    /*   Output Commands   */
    /* * * * * * * * * * * */

    void enable(int _nCh, bool _en = true);
    void disable(int _nCh) { enable(_nCh, false); }

    void setImpedance(int _nCh, std::string _imp);

    void invert(int _nCh, bool _inv = true);

    void setAttenuation(int _nCh, int _att);

    /* * * * * * * * * * * */
    /*   Source Commands   */
    /* * * * * * * * * * * */

    void setSine(int _nCh, double _f, double _a, double _offs = 0., double _ph = 0.);

    void setSquare(int _nCh, double _f, double _a, double _offs = 0., double _ph = 0.);

    void setRamp(int _nCh, double _f, double _a, double _offs = 0., double _ph = 0.);

    void setPulse(int _nCh, double _f, double _a, double _offs = 0., double _delay = 0.);

    void setNoise(int _nCh, double _a, double _offs);

    void enableBurst(int _nCh, bool _en = true);
    void disableBurst(int _nCh) { enableBurst(_nCh, false); }

    void setBurst(int _nCh, int _n, double _delay = 0, double _ph = 0, std::string _trg = "INT");

private:

};


#endif  // !WT_RIGOL_DG5000_H
