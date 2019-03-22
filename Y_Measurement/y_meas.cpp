#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string>
#include <sstream>
#include <ctime>
#include <csignal>

#include <RigolDS1000Z.h>
#include <RigolDG5000.h>

#include <TFile.h>
#include <TTree.h>

//#define DS1054Z_ADDR    "TCPIP0::134.61.14.166::inst0::INSTR"
//#define DG5072_ADDR     "TCPIP0::134.61.14.165::inst0::INSTR"
#define DS1054Z_ADDR	"USB0::0x1AB1::0x04CE::DS1ZA182310387::INSTR"
#define DG5072_ADDR     "USB0::0x1AB1::0x0640::DG5T154900182::INSTR"

#define MAX_SAMPLES 2000

using namespace std;

volatile bool cleanExit = false;

void signalHandler(int sign) {
    cleanExit = true;
    return;
}

int main (int argc, char** argv) {

    int fstart = 8000, fstop = 12000, fstep = 50; // Frequency range

    if (argc == 4) {
        fstart = atoi(argv[1]);
        fstop = atoi(argv[2]);
        fstep = atoi(argv[3]);
        if (fstep%5 != 0) {
            printf("[ERROR] - step %i has to be divisble by 5!\n", fstep);
            return -1;
        }
    } else if (argc != 1) {
        printf("Usage: %s [start stop step]\n", argv[0]);
        return -1;
    }

    printf("[SETUP] - Freq. sweep %i-%i Hz @ df=%i Hz.\n", fstart, fstop, fstep);

    /*   Instrument Setup   */

    printf("[SETUP] - Setting up '%s' ...\n", DS1054Z_ADDR);

    RigolDS1000Z scope(DS1054Z_ADDR);   // Oscilloscope
    for (int i = 1; i <= 2; i++) {
        scope.display(i, true);
        scope.setBWLimit(i, "20M");
        scope.setCoupling(i, "DC");
        scope.setProbe(i, 1);   // Atten x1
        scope.setOffset(i, 0);  // 0V
    }
    scope.setScale(1, 5);       // 5V/div
    scope.setScale(2, 5);
    scope.setMode("NORM");      // Save data on screen olny
    scope.setFormat("BYTE");    // Bytewise transfer
    scope.clearMeasurements();
    scope.setTimebase(1e-3);    // 1ms/div
    // Wait until done...
    scope.OPC();
    scope.run();

    printf("[SETUP] - Setting up '%s' ...\n", DG5072_ADDR);

    RigolDG5000 fgen(DG5072_ADDR);  // Function generator
    for (int i = 1; i <= 2; i++) {
        fgen.disableBurst(i);
        fgen.setImpedance(i, "50");
        fgen.setAttenuation(i, 1);
    }
    // Wait until done...
    fgen.OPC();

    /*   Data Setup   */

    time_t timestamp = time(nullptr);
    struct tm *date = localtime(&timestamp);
    char fName[200];
    sprintf(fName, "meas_%02i%02i%02i_%02i%02i.root", date->tm_mday, date->tm_mon, (date->tm_year + 1900), date->tm_hour, date->tm_min);

    printf("[SETUP] - Output to '%s' ...\n", fName);

    TFile outFile(fName, "RECREATE");
    TTree dataTree("data", "data");
    int nSamples;
    double ch1Volt[MAX_SAMPLES], ch2Volt[MAX_SAMPLES], time[MAX_SAMPLES];
    double ch1Vpp, ch2Vpp, ch1f, ch2f, dPhase;
    int fset;
    dataTree.Branch("nSamples", &nSamples);
    dataTree.Branch("ch1Volt", ch1Volt, "ch1Volt[nSamples]/D");
    dataTree.Branch("ch2Volt", ch2Volt, "ch2Volt[nSamples]/D");
    dataTree.Branch("time", time, "time[nSamples]/D");
    dataTree.Branch("ch1Vpp", &ch1Vpp);
    dataTree.Branch("ch2Vpp", &ch2Vpp);
    dataTree.Branch("ch1f", &ch1f);
    dataTree.Branch("ch2f", &ch2f);
    dataTree.Branch("fset", &fset);
    dataTree.Branch("dPhase", &dPhase);

    // Register signal handler
    signal(SIGINT, signalHandler);

    printf("[SETUP] - Done!\n");

    /*   Measurement Procedure   */

    double timebase;
    int df = fstep;
    for (fset = fstart; fset <= fstop; fset+=df) {

        // Did we receive a sigint (ctrl+c)?
        if (cleanExit) {
            printf("[LOOP] - Received SIGINT, closing '%s'...\n", fName);
            outFile.Write();
            outFile.Close();
            exit(SIGINT);
        }

        vector<double> vCh1Volt, vCh2Volt, vTime;

        // Adjust scope timebase
        timebase = 1./(fset*4.);
        scope.setTimebase(timebase);

        scope.OPC();

        // Emit 1V sine
        fgen.setSine(2, fset, 20);
        fgen.enable(2);

        // Take measurements
        scope.run();
        usleep(500e3);  // 0.5s
        ch1Vpp  = scope.getMeasurement("VPP", 1);
        ch2Vpp  = scope.getMeasurement("VPP", 2);
        ch1f    = scope.getMeasurement("FREQ", 1);
        ch2f    = 0;    //scope.getMeasurement("FREQ", 2);
        dPhase  = 0;    //scope.getMeasurement("RPH", 1, 2);

        // Record single transient
        scope.single();
        while ( !scope.triggered() ) { usleep(1e3); }
        scope.stop();
        fgen.disable(2);

        // Read channel 1
        nSamples = scope.readChannel(1, vTime, vCh1Volt);
        copy(vCh1Volt.begin(), vCh1Volt.end(), ch1Volt);

        // Read channel 2
        nSamples = scope.readChannel(2, vTime, vCh2Volt);
        copy(vCh2Volt.begin(), vCh2Volt.end(), ch2Volt);
        copy(vTime.begin(), vTime.end(), time);

        printf("[LOOP] - Set %i Hz (%d samples)\n", fset, nSamples);
        printf("\tCH1: %.2f VPP - CH2: %.2f V - %.0f Hz\n\n", ch1Vpp, ch2Vpp, ch1f);

        // Store data
        dataTree.Fill();

        scope.clearScreen();
        // Adjust vertical scale for next step

        if (ch2Vpp < 3) {
            scope.setScale(2, 0.5); // 500mV/Div
        } else if (ch2Vpp < 6) {
            scope.setScale(2, 1);   // 1V/Div
        } else if (ch2Vpp < 12) {
            scope.setScale(2, 2);   // 2V/Div
        } else {
            scope.setScale(2, 5);   // 5V/Div
        }

        // Next loop: if close to a resonance, start microstepping
        if ( (fset%fstep == 0) && (ch2Vpp > 1.)) {
            df = fstep;
        } else {
            df = fstep/5;
        }
    }

    // Cleanup
    outFile.Write();
    outFile.Close();

    return 1;
}
