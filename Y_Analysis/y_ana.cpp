#include <stdlib.h>
#include <sstream>

#include <TFile.h>
#include <TTree.h>
#include <TGraphErrors.h>
#include <TF1.h>

#define MAX_SAMPLES 2000

using namespace std;

int main (int argc, char** argv) {

    if (argc != 3) {
        printf("Usage: %s <source.root> <target.root>\n", argv[0]);
        return -1;
    }

    // Setup output data
    TFile output(argv[2], "CREATE");
    TTree outTree("conductiviTree", "conductiviTree");
    double freq, ReY, ImY, MagY, PhaseY;
    outTree.Branch("freq", &freq);
    outTree.Branch("ReY", &ReY);
    outTree.Branch("ImY", &ImY);
    outTree.Branch("MagY", &MagY);
    outTree.Branch("PhaseY", &PhaseY);
    double freqErr, ReYErr, ImYErr, MagYErr, PhaseYErr;
    outTree.Branch("freqErr", &freqErr);
    outTree.Branch("ReYErr", &ReYErr);
    outTree.Branch("ImYErr", &ImYErr);
    outTree.Branch("MagYErr", &MagYErr);
    outTree.Branch("PhaseYErr", &PhaseYErr);

    // Extract input data
    TFile input(argv[1], "READ");
    TTree* dataTree = (TTree*)input.Get("data");
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

    output.mkdir("plots");
    output.cd("plots");

    // Loop over input data
    int nEntries = dataTree->GetEntries();
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
        gr1.Write();
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
        gr2.Write();
        // Store fit results
        double ch2Vpp_fit   = fSine.GetParameter(1);
        double ch2f_fit     = fSine.GetParameter(2);
        double ch2ph        = fSine.GetParameter(3);
        double ch2VppErr    = fSine.GetParError(1);
        double ch2fErr      = fSine.GetParError(2);
        double ch2phErr     = fSine.GetParError(3);
        double dPhi         = ch1ph - ch2ph;

        printf("[%3i]\tCH1: %.2fV - %.0fHz - %.1fdeg\n", iEntry, ch1Vpp_fit, ch1f_fit, ch1ph*180./TMath::Pi());
        printf("\tCH2: %.2fV - %.0fHz - %.1fdeg", ch2Vpp_fit, ch2f_fit, ch2ph*180./TMath::Pi());

        // Measurement resistor
        double Rmeas = 975.;    // 1k with 5% tolerance (measured)

        // Calculate conductivity
        freq = (ch1f_fit + ch2f_fit)/2;
        freqErr = sqrt(ch1fErr*ch1fErr + ch1fErr*ch2fErr);
        ReY = (ch1Vpp_fit/ch2Vpp_fit*cos(dPhi) - 1)/Rmeas;
        ImY = (ch1Vpp_fit/ch2Vpp_fit*sin(dPhi))/Rmeas;
        MagY = sqrt(ReY*ReY + ImY*ImY);
        PhaseY = atan2(ImY, ReY) * 180./TMath::Pi();

        printf("\t=>\tMagY=%.2fmS - PhaseY=%.1fdeg\n\n", 1000*MagY, PhaseY);

        // Errors on ch1 & ch2 amplitude are dominating all other sources!
        ReYErr = sqrt( pow(ch1VppErr/ch2Vpp_fit, 2) +
            pow(ch2VppErr*ch1Vpp_fit/(ch2Vpp_fit*ch2Vpp_fit), 2) )/Rmeas;
        ImYErr = ReYErr;
        MagYErr = ReYErr;
        PhaseYErr = sqrt( ch1phErr*ch1phErr + ch2phErr*ch2phErr ) * 180./TMath::Pi();

        // Create plots
        int n = grReY.GetN();
        grReY.SetPoint(n, freq, ReY);
        grReY.SetPointError(n, freqErr, ReYErr);
        grImY.SetPoint(n, freq, ImY);
        grImY.SetPointError(n, freqErr, ImYErr);
        grMagY.SetPoint(n, freq, MagY);
        grMagY.SetPointError(n, freqErr, MagYErr);
        grPhaseY.SetPoint(n, freq, PhaseY);
        grPhaseY.SetPointError(n, freqErr, PhaseYErr);

        // Save data
        outTree.Fill();
    }

    // Sort points ascending x values & save!
    grReY.Sort();
    grImY.Sort();
    grMagY.Sort();
    grPhaseY.Sort();

    output.cd();

    grReY.Write();
    grImY.Write();
    grMagY.Write();
    grPhaseY.Write();
    output.Write();

    // Cleanup
    input.Close();
    output.Close();

    return 1;
}
