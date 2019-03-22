#include <stdlib.h>
#include <sstream>
#include <fstream>
#include <string>

#include <TFile.h>
#include <TTree.h>
#include <TGraphErrors.h>
#include <TF1.h>

#define MAX_SAMPLES 2000

using namespace std;

int main (int argc, char** argv) {

    if (argc != 3) {
        printf("Usage: %s <source.root> <target.root/.txt>\n", argv[0]);
        return -1;
    }

    bool isRoot = false, isTxt = false;
    string fileName(argv[2]);
    if (fileName.find(".root") != string::npos) {
        isTxt = false;
        isRoot = true;
    }
    if (fileName.find(".txt") != string::npos) {
        isRoot = false;
        isTxt = true;
    }

    if ( !(isRoot || isTxt) ) {
        printf("[ERROR] - '%s' has to be .root or .txt file!\n", fileName.c_str());
        return -1;
    }

    printf("[SETUP] - Writing data to file '%s'\n", fileName.c_str());

    // Setup output data
    TFile* output = nullptr;
    TTree* outTree = nullptr;
    ofstream outStream;
    double freq, ReY, ImY, MagY, PhaseY;
    double freqErr, ReYErr, ImYErr, MagYErr, PhaseYErr;
    // Root setup
    if (isRoot) {
        output = new TFile(argv[2], "CREATE");
        if ( output->IsZombie() ) {
            printf("[ERROR] - Cannot create '%s'\n", fileName.c_str());
            return -1;
        }
        outTree = new TTree("conductiviTree", "conductiviTree");
        outTree->Branch("freq", &freq);
        outTree->Branch("ReY", &ReY);
        outTree->Branch("ImY", &ImY);
        outTree->Branch("MagY", &MagY);
        outTree->Branch("PhaseY", &PhaseY);
        outTree->Branch("freqErr", &freqErr);
        outTree->Branch("ReYErr", &ReYErr);
        outTree->Branch("ImYErr", &ImYErr);
        outTree->Branch("MagYErr", &MagYErr);
        outTree->Branch("PhaseYErr", &PhaseYErr);
        // Subdir for plots..
        output->mkdir("plots");
    }
    // Txt setup
    if (isTxt)
        outStream.open(fileName.c_str(), ofstream::app);

    printf("[SETUP] - Reading '%s'\n", argv[1]);

    // Extract input data
    TFile* input = new TFile(argv[1], "READ");
    TTree* dataTree = (TTree*)input->Get("data");
    int nSamples;
    double ch1Volt[MAX_SAMPLES], ch2Volt[MAX_SAMPLES], time[MAX_SAMPLES];
    double ch1Vpp, ch2Vpp, ch1f, ch2f, dPhase;
    int fset;
    dataTree->SetBranchAddress("nSamples", &nSamples);
    dataTree->SetBranchAddress("ch1Volt", ch1Volt);
    dataTree->SetBranchAddress("ch2Volt", ch2Volt);
    dataTree->SetBranchAddress("time", time);
    dataTree->SetBranchAddress("ch1Vpp", &ch1Vpp);
    dataTree->SetBranchAddress("ch2Vpp", &ch2Vpp);
    dataTree->SetBranchAddress("ch1f", &ch1f);
    dataTree->SetBranchAddress("ch2f", &ch2f);
    dataTree->SetBranchAddress("fset", &fset);
    dataTree->SetBranchAddress("dPhase", &dPhase);

    TGraphErrors grReY, grImY, grMagY, grPhaseY;
    grReY.SetName("Real Y");
    grImY.SetName("Imaginary Y");
    grMagY.SetName("Magnitude Y");
    grPhaseY.SetName("Phase Y");

    // Loop over input data
    int nEntries = dataTree->GetEntries();
    double ResY = 0, ResF = 0;
    for (int iEntry = 0; iEntry < nEntries; iEntry++) {
        dataTree->GetEntry(iEntry);

        // Errors on time and voltage
        double *timeErr = new double[nSamples];
        fill(timeErr, timeErr + nSamples, 200e-9);  // 500MSps
        double *voltErr = new double[nSamples];
        fill(voltErr, voltErr + nSamples, 0.035);   // 8-bit @ 8V range
        stringstream ss("");

        // Sine fit function
        TF1 fSine("fsine", "[0] + [1]*sin(2*TMath::Pi()*[2]*x + [3])", 0, 0.001);
        fSine.SetParNames("Offset", "Ampl.", "Freq.", "Phase");
        fSine.SetParLimits(0, -1, 1);
        fSine.SetParLimits(2, 0.85*fset, 1.1*fset);
        fSine.SetParLimits(3, -1*TMath::Pi(), TMath::Pi());

        // Fit ch1 and ch2!
        TGraphErrors gr1(nSamples, time, ch1Volt, timeErr, voltErr);
        ss << "CH1_" << fset << "Hz_" << iEntry;
        gr1.SetName( ss.str().c_str() );

        double ch1amp = ch1Vpp/2.;
        fSine.SetParameters(0, ch1amp, fset, 0);
        fSine.SetParLimits(1, 0.8*ch1amp, 1.2*ch1amp);  // Amplitude
        gr1.Fit(&fSine, "Q");
        // Store fit results
        double ch1Vpp_fit   = fSine.GetParameter(1);
        double ch1f_fit     = fSine.GetParameter(2);
        double ch1ph        = fSine.GetParameter(3);
        double ch1VppErr    = fSine.GetParError(1);
        double ch1fErr      = fSine.GetParError(2);
        double ch1phErr     = fSine.GetParError(3);

        TGraphErrors gr2(nSamples, time, ch2Volt, timeErr, voltErr);
        ss.str("");
        ss << "CH2_" << fset << "Hz_" << iEntry;
        gr2.SetName( ss.str().c_str() );

        double ch2amp = ch2Vpp/2.;
        fSine.SetParameters(0, ch2amp, fset, 0);
        fSine.SetParLimits(1, 0.8*ch2amp, 1.2*ch2amp);  // Amplitude
        gr2.Fit(&fSine, "Q");

        // Store fit results
        double ch2Vpp_fit   = fSine.GetParameter(1);
        double ch2f_fit     = fSine.GetParameter(2);
        double ch2ph        = fSine.GetParameter(3);
        double ch2VppErr    = fSine.GetParError(1);
        double ch2fErr      = fSine.GetParError(2);
        double ch2phErr     = fSine.GetParError(3);
        double dPhi         = ch1ph - ch2ph;

        // Measurement resistor
        double Rmeas = 975.;    // 1k with 5% tolerance (measured)

        // Calculate conductivity
        freq = (ch1f_fit + ch2f_fit)/2;
        freqErr = sqrt(ch1fErr*ch1fErr + ch1fErr*ch2fErr);
        ReY = (ch1Vpp_fit/ch2Vpp_fit*cos(dPhi) - 1)/Rmeas;
        ImY = (ch1Vpp_fit/ch2Vpp_fit*sin(dPhi))/Rmeas;
        MagY = sqrt(ReY*ReY + ImY*ImY);
        PhaseY = atan2(ImY, ReY) * 180./TMath::Pi();

        // Find resonance under 15 kHz
        if ( (MagY > ResY) && (freq < 15000) ) {
            ResY = MagY;
            ResF = freq;
        }

        // Errors on ch1 & ch2 amplitude are dominating all other sources!
        ReYErr = sqrt( pow(ch1VppErr/ch2Vpp_fit, 2) +
            pow(ch2VppErr*ch1Vpp_fit/(ch2Vpp_fit*ch2Vpp_fit), 2) )/Rmeas;
        ImYErr = ReYErr;
        MagYErr = ReYErr;
        PhaseYErr = sqrt( ch1phErr*ch1phErr + ch2phErr*ch2phErr ) * 180./TMath::Pi();

        // Text output!
        printf("[%3i]\tCH1: %.2fV - %.0fHz - %.1fdeg\n", iEntry, ch1Vpp_fit, ch1f_fit, ch1ph*180./TMath::Pi());
        printf("\tCH2: %.2fV - %.0fHz - %.1fdeg", ch2Vpp_fit, ch2f_fit, ch2ph*180./TMath::Pi());
        printf("\t=>\tMagY=%.2fmS - PhaseY=%.1fdeg\n\n", 1000*MagY, PhaseY);

        // Save to root file, if specified
        if (isRoot) {
            // Fill plots
            int n = grReY.GetN();
            grReY.SetPoint(n, freq, ReY);
            grReY.SetPointError(n, freqErr, ReYErr);
            grImY.SetPoint(n, freq, ImY);
            grImY.SetPointError(n, freqErr, ImYErr);
            grMagY.SetPoint(n, freq, MagY);
            grMagY.SetPointError(n, freqErr, MagYErr);
            grPhaseY.SetPoint(n, freq, PhaseY);
            grPhaseY.SetPointError(n, freqErr, PhaseYErr);

            // Save transients & fit results
            output->cd("plots");
            gr1.Write();
            gr2.Write();

            // Save other data
            outTree->Fill();
        }
    }

    // Sort points ascending x values & save!
    grReY.Sort();
    grImY.Sort();
    grMagY.Sort();
    grPhaseY.Sort();

    if (isRoot) {
        output->cd();
        grReY.Write();
        grImY.Write();
        grMagY.Write();
        grPhaseY.Write();
        output->Write();

        // Cleanup
        input->Close();
        output->Close();
    }

    // Append to txt file, if specified
    if (isTxt) {
        outStream << ResF << "\t" << ResY << "\n";
        outStream.close();
    }

    return 1;
}
