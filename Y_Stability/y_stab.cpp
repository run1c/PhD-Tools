#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string>
#include <sstream>
#include <ctime>

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

int main (int argc, char** argv) {

    RigolDS1000Z scope(DS1054Z_ADDR);

    return 1;
}
