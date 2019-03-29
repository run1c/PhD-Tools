#include "../../../lib/RWTH_style.h"

void plots_labormessung () {

    /*   M E S S U N G   L U F T   */

    TFile *fMeas = new TFile("../SonoxP4_Wasser.root", "OPEN");

    TGraphErrors *grMeasMagY  = (TGraphErrors*)fMeas->Get("Magnitude Y");
    TGraphErrors *grMeasPhY   = (TGraphErrors*)fMeas->Get("Phase Y");
    TGraphErrors *grMeasReY   = (TGraphErrors*)fMeas->Get("Real Y");

    grMeasMagY->SetTitle("Messung Wasser");
    grMeasPhY->SetTitle("Messung Wasser");
    grMeasReY->SetTitle("Messung Wasser");

    grMeasMagY->SetLineWidth(1);
    grMeasPhY->SetLineWidth(1);
    grMeasReY->SetLineWidth(1);

    grMeasMagY->SetLineColor(RWTH::kBlau100);
    grMeasPhY->SetLineColor(RWTH::kBlau100);
    grMeasReY->SetLineColor(RWTH::kBlau100);

    grMeasMagY->SetMarkerStyle(kFullDotSmall);
    grMeasPhY->SetMarkerStyle(kFullDotSmall);
    grMeasReY->SetMarkerStyle(kFullDotSmall);

    grMeasMagY->SetMarkerColor(RWTH::kBlau100);
    grMeasPhY->SetMarkerColor(RWTH::kBlau100);
    grMeasReY->SetMarkerColor(RWTH::kBlau100);

    /*   S I M U L A T I O N   L U F T   */

    TFile *fSim = new TFile("../SonoxP4_Wasser_sim.root", "OPEN");

    TGraphErrors *grSimMagY  = (TGraphErrors*)fSim->Get("Magnitude Y");
    TGraphErrors *grSimPhY   = (TGraphErrors*)fSim->Get("Phase Y");
    TGraphErrors *grSimReY   = (TGraphErrors*)fSim->Get("Real Y");

    grSimMagY->SetTitle("Simulation Wasser");
    grSimPhY->SetTitle("Simulation Wasser");
    grSimReY->SetTitle("Simulation Wasser");

    grSimMagY->SetLineWidth(2);
    grSimPhY->SetLineWidth(2);
    grSimReY->SetLineWidth(2);

    grSimMagY->SetLineColor(RWTH::kMagenta100);
    grSimPhY->SetLineColor(RWTH::kMagenta100);
    grSimReY->SetLineColor(RWTH::kMagenta100);

    grSimMagY->SetFillStyle(3001);
    grSimPhY->SetFillStyle(3001);
    grSimReY->SetFillStyle(3001);

    grSimMagY->SetFillColor(RWTH::kMagenta100);
    grSimPhY->SetFillColor(RWTH::kMagenta100);
    grSimReY->SetFillColor(RWTH::kMagenta100);

    /*   V E R G L E I C H E   */

    // Magnitude
    TMultiGraph *mgMagY = new TMultiGraph("mgMagY", "Magnitude Y;f [Hz];|Y| [S]");
    mgMagY->Add(grSimMagY, "L3");
    mgMagY->Add(grMeasMagY, "LP");

    TLegend *legMagY = new TLegend(0.7, 0.25, 0.9, 0.35);
    legMagY->AddEntry(grMeasMagY);
    legMagY->AddEntry(grSimMagY);

    // Phase
    TMultiGraph *mgPhY = new TMultiGraph("mgPhY", "Phase Y;f [Hz];#theta [#circ]");
    mgPhY->Add(grSimPhY, "L3");
    mgPhY->Add(grMeasPhY, "LP");

    // Real
    TMultiGraph *mgReY = new TMultiGraph("mgReY", "Realanteil Y;f [Hz];Re(Y) [S]");
    mgReY->Add(grSimReY, "L3");
    mgReY->Add(grMeasReY, "LP");

    // Set Canvas sizes
    gStyle->SetCanvasDefH(600);
    gStyle->SetCanvasDefW(1200);

    TCanvas* cMag = new TCanvas();
    mgMagY->Draw("A");
    legMagY->Draw();
    gPad->SetLogy();
    cMag->SaveAs("meas_mag_water.png");

    TCanvas* cPh = new TCanvas();
    mgPhY->Draw("A");
    cPh->SaveAs("meas_phase_water.png");

    TCanvas* cRe = new TCanvas();
    mgReY->Draw("A");
    cRe->SaveAs("meas_real_water.png");


    // Cleanup
    fMeas->Close();
    fSim->Close();

    return;
}
