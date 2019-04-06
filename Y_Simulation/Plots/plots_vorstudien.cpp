#include "../../EnExLib/RWTH_style.h"

void plots_vorstudien() {

    /*
     *      Vergleich der Leitfähigkeiten mit idealem (Datenblatt) und
     *      realem (Protokoll) PZT-Werkstoff (Sonox P4)
     */

    TFile *fIdeal = new TFile("../2d_tonpilz_model_SP4_ideal.root", "OPEN");
    TFile *fReal = new TFile("../2d_tonpilz_model_SP4_real.root", "OPEN");

    TGraphErrors *grMagYIdeal  = (TGraphErrors*)fIdeal->Get("Magnitude Y");
    grMagYIdeal->SetTitle("Sonox P4 (ideal)");
    grMagYIdeal->SetLineWidth(2);
    grMagYIdeal->SetLineColor(RWTH::kBlau100);
    grMagYIdeal->SetFillColor(RWTH::kBlau100);
    grMagYIdeal->SetFillStyle(3001);
    TGraphErrors *grPhYIdeal   = (TGraphErrors*)fIdeal->Get("Phase Y");
    grPhYIdeal->SetLineWidth(2);
    grPhYIdeal->SetLineColor(RWTH::kBlau100);
    grPhYIdeal->SetFillColor(RWTH::kBlau100);
    grPhYIdeal->SetFillStyle(3001);

    TGraphErrors *grMagYReal  = (TGraphErrors*)fReal->Get("Magnitude Y");
    grMagYReal->SetTitle("Sonox P4 (real)");
    grMagYReal->SetLineWidth(2);
    grMagYReal->SetLineColor(RWTH::kMagenta100);
    grMagYReal->SetFillColor(RWTH::kMagenta100);
    grMagYReal->SetFillStyle(3001);
    TGraphErrors *grPhYReal   = (TGraphErrors*)fReal->Get("Phase Y");
    grPhYReal->SetLineWidth(2);
    grPhYReal->SetLineColor(RWTH::kMagenta100);
    grPhYReal->SetFillColor(RWTH::kMagenta100);
    grPhYReal->SetFillStyle(3001);

    TMultiGraph *mgMag = new TMultiGraph("MagY", "Magnitude Leitf#ddot{a}higkeit;f [Hz];|Y| [S]");
    mgMag->Add(grMagYIdeal, "L3");
    mgMag->Add(grMagYReal, "L3");
    mgMag->GetXaxis()->SetRangeUser(1000, 25000);

    TMultiGraph *mgPhY = new TMultiGraph("PhY", "Phase Leitf#ddot{a}higkeit;f [Hz];#theta [#circ]");
    mgPhY->Add(grPhYIdeal, "L3");
    mgPhY->Add(grPhYReal, "L3");
    mgPhY->GetXaxis()->SetRangeUser(1000, 25000);
    mgPhY->GetYaxis()->SetRangeUser(-135, 135);
    mgPhY->GetYaxis()->SetNdivisions(406, kFALSE);

    TLegend *leg = new TLegend(0.70, 0.78, 0.95, 0.90);
    leg->AddEntry(grMagYIdeal, "Sonox P4 (ideal)");
    leg->AddEntry(grMagYReal, "Sonox P4 (real)");

    TCanvas *c1 = new TCanvas();
    gPad->SetLogy();
    mgMag->Draw("A");
    leg->Draw();
    c1->SaveAs("comp_sp4_MagY.png");

    TCanvas *c2 = new TCanvas();
    mgPhY->Draw("A");
    leg->Draw();
    c2->SaveAs("comp_sp4_PhY.png");

    // Cleanup
    fIdeal->Close();
    fReal->Close();

    return;
}
