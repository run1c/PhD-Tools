#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string>

#include "RigolDS1000Z.h"
#include "RigolDG5000.h"

#include <TFile.h>
#include <TGraph.h>

using namespace std;

#define DS1054Z_ADDR    "USB0::0x1AB1::0x04CE::DS1ZA182310387::INSTR"
#define DG5072_ADDR     "USB0::0x1AB1::0x0640::DG5T154900182::INSTR"

int main (int argc, char** argv) {
    string scopeName = gVisaRM.findDevice("USB0::?*DS?*INSTR");
    string fgenName = gVisaRM.findDevice("USB0::?*DG?*INSTR");
    printf("Found Devices:\n\t%s\n\t%s\n", scopeName.c_str(), fgenName.c_str());

    RigolDG5000 fgen(fgenName);
    fgen.setImpedance(1, "50");
    fgen.setAttenuation(1, 1);
    fgen.setSine(1, 5000, 1);
    fgen.OPC();

    RigolDS1000Z scope(scopeName);
    scope.setBWLimit(1, true);
    scope.setBWLimit(2, true);
    scope.setTimebase(2e-3);
    scope.setDelay(0);

    scope.setSource(1);
    scope.setMode("NORM");
    scope.setFormat("BYTE");
    scope.clearMeasurements();
    scope.OPC();

    scope.single();

    fgen.enable(1);

    while ( !scope.triggered() ) { usleep(100); }
    fgen.disable(1);
    scope.stop();

    scope.getMeasurement("FREQ", 1);
    scope.getMeasurement("FREQ", 2);
    scope.getMeasurement("VPP", 1);
    scope.getMeasurement("VPP", 2);
    scope.getMeasurement("RPH", 1, 2);

    vector<double> time1, volt1, time2, volt2;
    scope.setSource(1);
    scope.getPreamble();
    int n1 = scope.getData(time1, volt1);
    scope.setSource(2);
    scope.getPreamble();
    int n2 = scope.getData(time2, volt2);

    TFile* out = new TFile("Moop.root", "RECREATE");
    TGraph* gr1 = new TGraph(n1, time1.data(), volt1.data());
    TGraph* gr2 = new TGraph(n2, time2.data(), volt2.data());
    gr1->Write();
    gr2->Write();
    out->Write();

    // Cleanup
    out->Close();

    return 1;
}
