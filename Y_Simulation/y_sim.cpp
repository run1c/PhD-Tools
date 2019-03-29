#include <stdio.h>
#include <sstream>
#include <fstream>
#include <vector>

#include <TFile.h>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <TCanvas.h>
#include <TTree.h>
#include <TMath.h>

class multisim_data {
public:
    multisim_data(std::string file) {
        // Parse real part
        _input.open( file.c_str(), std::ifstream::in );
        findPos("Magnitude");
        getRun(_freq, _Y, _Y_err);
        findPos("Phase");
        getRun(_freq, _phaseY, _phaseY_err);
        _input.close();
    }

    ~multisim_data() { }

    // Read a single run
    int getRun(std::vector<double> &x, std::vector<double> &y, std::vector<double> &yerr) {
        x.clear();
        y.clear();
        yerr.clear();

        std::stringstream ss;
        std::string line, dummy;
        double xbuf = 0., ybuf = 0.;
        _samples = 0;
        _runs = 0;

        // Loop over file
        while ( std::getline(_input, line) ) {
            ss.clear();
            ss.str(line);

            // Reset counter
            if ( line.find("---- Trace:") != std::string::npos ) {
                _samples = 0;
                //printf("New run; resetting counter\n");
                continue;
            }

            // Increase run count
            if ( line.find("---- End Trace:") != std::string::npos ) {
                _runs++;
                //printf("Run complete; _runs = %i\n", _runs);
                continue;
            }

            // Extract data & increase counter
            if ( line.find_first_of("0123456789") != std::string::npos) {
                ss >> xbuf >> ybuf;
                //printf("\t%i\t%e\t%e\n", _samples, xbuf, ybuf);
                if (_runs == 0) {
                    x.push_back(xbuf);
                    y.push_back(ybuf);
                    yerr.push_back(ybuf*ybuf);
                } else {
                    y.at(_samples) += ybuf;
                    yerr.at(_samples) += ybuf*ybuf;
                    _samples++;
                }
            }

            // Done!
            if ( line.find("---- End Graph:") != std::string::npos ) {
                //printf("Done! (%i)\n", _samples);
                break;
            }
        }

        for (int j = 0; j < _samples; j++) {
            y.at(j) /= _runs;
            yerr.at(j) /= _runs;
            yerr.at(j) = sqrt( yerr.at(j) - y.at(j)*y.at(j) );
        }
        return _samples;
    }

    void findPos(std::string name) {
        _input.clear();
        _input.seekg(0, _input.beg);
        std::string line;
        while ( std::getline(_input, line) ) {
            if ( line.find(name) != std::string::npos )
                break;
        }
    }

    std::vector<double> getFreq() { return _freq; }
    std::vector<double> getMagY() { return _Y; }
    std::vector<double> getMagYErr() { return _Y_err; }
    std::vector<double> getPhaseY() { return _phaseY; }
    std::vector<double> getPhaseYErr() { return _phaseY_err; }
    int getN() { return _samples; }

private:
    std::ifstream _input;
    int _runs, _samples;
    std::vector<double> _Y, _Y_err, _freq;
    std::vector<double> _phaseY, _phaseY_err;
};

using namespace std;

int main (int argc, char** argv) {

    if (argc != 3) {
        printf("Usage: %s <data.txt> <target.root>\n", argv[0]);
        return -1;
    }

    TFile file(argv[2], "RECREATE");

    multisim_data ms_data(argv[1]);
    int n = ms_data.getN();

    TTree dataTree("data", "data");
    double freq, MagY, PhaseY, ReY, ImY, RePwr, ImPwr;
    dataTree.Branch("ReY", &ReY);
    dataTree.Branch("ImY", &ImY);
    dataTree.Branch("MagY", &MagY);
    dataTree.Branch("PhaseY", &PhaseY);
    dataTree.Branch("RePwr", &RePwr);
    dataTree.Branch("ImPwr", &ImPwr);
    double freqErr, MagYErr, PhaseYErr, ReYErr, ImYErr, RePwrErr, ImPwrErr;
    dataTree.Branch("ReYErr", &ReYErr);
    dataTree.Branch("ImYErr", &ImYErr);
    dataTree.Branch("MagYErr", &MagYErr);
    dataTree.Branch("PhaseYErr", &PhaseYErr);
    dataTree.Branch("RePwrErr", &RePwrErr);
    dataTree.Branch("ImPwrErr", &ImPwrErr);

    TGraphErrors grReY, grImY, grMagY, grPhaseY, grRePwr, grImPwr;
    grReY.SetName("Real Y");
    grImY.SetName("Imaginary Y");
    grMagY.SetName("Magnitude Y");
    grPhaseY.SetName("Phase Y");
    grRePwr.SetName("Real Power");
    grImPwr.SetName("Imaginary Power");

    for (int i = 0; i < n; i++) {
        freq = ms_data.getFreq()[i];
        MagY = ms_data.getMagY()[i];
        PhaseY = ms_data.getPhaseY()[i] + 180.;
        ReY = MagY*cos(PhaseY * TMath::Pi()/180.);
        ImY = MagY*sin(PhaseY * TMath::Pi()/180.);
        RePwr = ReY;     // At amplitude 1V and 1Ohm
        ImPwr = ImY;

        freqErr = 0.;
        MagYErr = ms_data.getMagYErr()[i];
        PhaseYErr = ms_data.getPhaseYErr()[i];
        ReYErr = MagYErr;
        ImYErr = MagYErr;
        RePwrErr = ReYErr;
        ImPwrErr = ImYErr;

        dataTree.Fill();

        grImY.SetPoint(i, freq, ImY);
        grImY.SetPointError(i, freqErr, ImYErr);
        grReY.SetPoint(i, freq, ReY);
        grReY.SetPointError(i, freqErr, ReYErr);
        grMagY.SetPoint(i, freq, MagY);
        grMagY.SetPointError(i, freqErr, MagYErr);
        grPhaseY.SetPoint(i, freq, PhaseY);
        grPhaseY.SetPointError(i, freqErr, PhaseYErr);
        grRePwr.SetPoint(i, freq, RePwr);
        grRePwr.SetPointError(i, freqErr, RePwrErr);
        grImPwr.SetPoint(i, freq, ImPwr);
        grImPwr.SetPointError(i, freqErr, ImPwrErr);
    }

    grReY.Write();
    grImY.Write();
    grMagY.Write();
    grPhaseY.Write();
    grRePwr.Write();

    file.Write();
    file.Close();
    return 1;
}
