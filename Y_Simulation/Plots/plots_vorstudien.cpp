#include "../../../lib/RWTH_style.h"

void plots_vorstudien() {

    TFile *fSonoxP4 = new TFile("../2d_tonpilz_SonoxP4.root", "OPEN");
    TFile *fSonoxP8 = new TFile("../2d_tonpilz_SonoxP8.root", "OPEN");
    TFile *fPIC184 = new TFile("../2d_tonpilz_PIC184.root", "OPEN");

    TGraphErrors *grMagYSonoxP4  = (TGraphErrors*)fSonoxP4->Get("Magnitude Y");
    grMagYSonoxP4->SetTitle("Sonox P4");
    grMagYSonoxP4->SetLineWidth(2);
    grMagYSonoxP4->SetLineColor(RWTH::kBlau100);
    grMagYSonoxP4->SetFillColor(RWTH::kBlau100);
    grMagYSonoxP4->SetFillStyle(3001);
    TGraphErrors *grReYSonoxP4   = (TGraphErrors*)fSonoxP4->Get("Real Y");
    grReYSonoxP4->SetLineWidth(2);
    grReYSonoxP4->SetLineColor(RWTH::kBlau100);
    grReYSonoxP4->SetFillColor(RWTH::kBlau100);
    grReYSonoxP4->SetFillStyle(3001);

    TGraphErrors *grMagYSonoxP8  = (TGraphErrors*)fSonoxP8->Get("Magnitude Y");
    grMagYSonoxP8->SetTitle("Sonox P8");
    grMagYSonoxP8->SetLineWidth(2);
    grMagYSonoxP8->SetLineColor(RWTH::kMagenta100);
    grMagYSonoxP8->SetFillColor(RWTH::kMagenta100);
    grMagYSonoxP8->SetFillStyle(3001);
    TGraphErrors *grReYSonoxP8   = (TGraphErrors*)fSonoxP8->Get("Real Y");
    grReYSonoxP8->SetLineWidth(2);
    grReYSonoxP8->SetLineColor(RWTH::kMagenta100);
    grReYSonoxP8->SetFillColor(RWTH::kMagenta100);
    grReYSonoxP8->SetFillStyle(3001);

    TGraphErrors *grMagYPIC184  = (TGraphErrors*)fPIC184->Get("Magnitude Y");
    grMagYPIC184->SetTitle("PIC184");
    grMagYPIC184->SetLineWidth(2);
    grMagYPIC184->SetLineColor(RWTH::kPetrol100);
    grMagYPIC184->SetFillColor(RWTH::kPetrol100);
    grMagYPIC184->SetFillStyle(3001);
    TGraphErrors *grReYPIC184   = (TGraphErrors*)fPIC184->Get("Real Y");
    grReYPIC184->SetLineWidth(2);
    grReYPIC184->SetLineColor(RWTH::kPetrol100);
    grReYPIC184->SetFillColor(RWTH::kPetrol100);
    grReYPIC184->SetFillStyle(3001);

    TMultiGraph *mgMag = new TMultiGraph("MagY", "Magnitude Y;f [Hz];|Y| [S]");
    mgMag->Add(grMagYSonoxP4, "L3");
    mgMag->Add(grMagYSonoxP8, "L3");
    mgMag->Add(grMagYPIC184, "L3");
    mgMag->GetXaxis()->SetRangeUser(0, 20000);

    TMultiGraph *mgRe = new TMultiGraph("PhY", "Wirkleistung;f [Hz];P [W]");
    mgRe->Add(grReYSonoxP4, "L3")   ;
    mgRe->Add(grReYSonoxP8, "L3");
    mgRe->Add(grReYPIC184, "L3");
    mgRe->GetXaxis()->SetRangeUser(4000, 16000);

    TLegend *leg = new TLegend(0.2, 0.3, 0.4, 0.5);
    leg->AddEntry(grMagYSonoxP4, "Sonox P4");
    leg->AddEntry(grMagYSonoxP8, "Sonox P8");
    leg->AddEntry(grMagYPIC184, "PIC184");

    TCanvas *c1 = new TCanvas();
    gPad->SetLogy();
    mgMag->Draw("A");
    leg->Draw();
    c1->SaveAs("mat_comp_y.png");

    TCanvas *c2 = new TCanvas();
    mgRe->Draw("A");
    leg->Draw();
    c2->SaveAs("mat_comp_p.png");

    // Cleanup
    fSonoxP4->Close();
    fSonoxP8->Close();
    fPIC184->Close();

    return;
}
