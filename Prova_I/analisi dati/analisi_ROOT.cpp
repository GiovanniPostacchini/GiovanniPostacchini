//#include "MinimizerOptions.h."
#include "ROOT/RCsvDS.hxx"
#include "TCanvas.h"
#include "TF1.h"
#include "TFitResult.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TLegend.h"
#include "TMultiGraph.h"
#include "TPaveStats.h"
#include "TPaveText.h"
#include "TVirtualFitter.h"
#include "read_csv.h"
#include <algorithm>
#include <string>

/*template <class TG> double R_2(TF1 *f, TG g) {
  double fscarts = 0;
  double yscarts = 0;
  double yMean = g->GetRMS(2);
  for (int i = 0; i < g->GetN(); ++i) {
    double y = g->GetPointY(i);
    double fi = f->Eval(g->GetPointX(i));
    yscarts += TMath::Power(y - yMean, 2);
    fscarts += TMath::Power(y - fi, 2);
  }
  double r2 = 1 - fscarts / yscarts;
  cout << std::setprecision(10) << "R2:\t" << r2 << '\n';
  cout << "N:\t" << g->GetN() << "\tfscarts:\t" << fscarts << '\n'
       << "Ym:\t" << yMean << '\n';
  return r2;
}
*/
// for simple linear fit (calibration)
void linear_fit(std::string filename = "./dati_csv/calibrazione.csv",
                std::array<std::string, 4> column = {"V_mult", "Err_mult",
                                                     "V_osc", "Err_osc"},
                std::string title = "Fit Lineare") {
  gStyle->SetOptFit(1111);

  std::vector<double> vx = read_csv<double>(filename, column[0], ';');
  std::vector<double> vx_err = read_csv<double>(filename, column[1], ';');
  std::vector<double> vy = read_csv<double>(filename, column[2], ';');
  std::vector<double> vy_err = read_csv<double>(filename, column[3], ';');
  int size = vx.size();
  TGraphErrors *ge = new TGraphErrors(size, &vx[0], &vy[0], /* &vx_err[0] */ 0,
                                      &vy_err[0]); // X Y EX EY
  TF1 *line = new TF1("line", "[0]+[1]*x");
  line->SetParameters(0, 1);
  line->SetLineColor(kRed);
  line->SetParNames("A", "B"); // A + BX = 0
  ge->Fit(line);
  ge->SetTitle(title.c_str());
  ge->GetXaxis()->SetTitle("Tensione multimetro [mV]");
  ge->GetYaxis()->SetTitle("Tensione oscilloscopio [mV]");
  ge->SetMarkerStyle(kFullDotMedium);

  std::cout << "Par A\t" << line->GetParameter(0) << "\t+/-\t"
            << line->GetParError(0) << '\n';
  std::cout << "Par B\t" << line->GetParameter(1) << "\t+/-\t"
            << line->GetParError(1) << '\n';

  TCanvas *c1 = new TCanvas("c", " ");
  c1->SetGrid();
  c1->SetTicks();
  ge->Draw("APE");
  gPad->Update();
  TPaveStats *st = (TPaveStats *)ge->FindObject("stats");
  st->SetX1NDC(0.138968);
  st->SetY1NDC(0.666667);
  st->SetX2NDC(0.5);
  st->SetY2NDC(0.867089);
  c1->Print("calibrazione.png");
  // ge->Print();
}

// for exponential fit --> linear fit
TGraphErrors *exponential_fit_ln(
    std::string filename = "./dati_csv/germanio.csv",
    std::string title = "Caratteristica Tensione Corrente Diodo Ge",
    std::array<std::string, 4> column = {"V", "V_err", "I", "I_err"},
    bool const fit = true) {

  std::vector<double> vx = read_csv<double>(filename, column[0], ';');
  std::vector<double> vx_err = read_csv<double>(filename, column[1], ';');
  std::vector<double> vy = read_csv<double>(filename, column[2], ';');
  std::vector<double> vy_err = read_csv<double>(filename, column[3], ';');
  int size = vx.size();

  TGraphErrors *ge = new TGraphErrors(size, &vx[0], &vy[0], &vx_err[0],
                                      &vy_err[0]); // X Y EX EY
  ge->SetDrawOption("Z");
  if (fit) {
    gStyle->SetOptFit(1111);
    TF1 *line = new TF1("line", "[0]*(TMath::Exp(x/[1])-1)", 60, 170);
    line->SetParameters(1e-3, 45);
    line->SetLineColor(kRed);
    line->SetParNames("I_{0}", "#eta V_{T}"); // A + BX = 0
    ge->SetTitle(title.c_str());
    ge->GetYaxis()->SetTitle("I [mA]");
    ge->GetXaxis()->SetTitle("V [mV]");
    ge->Fit(line, "R");

    std::cout << "Par A\t" << line->GetParameter(0) << "\t+/-\t"
              << line->GetParError(0) << '\n';
    std::cout << "Par B\t" << line->GetParameter(1) << "\t+/-\t"
              << line->GetParError(1) << '\n';

    TCanvas *c1 = new TCanvas("c", "Linear Fit");
    c1->SetGrid();
    c1->SetLogy();
    c1->SetTicks();
    line->GetXaxis()->SetRangeUser(60, 170);
    ge->Draw("APE");

    gPad->Update();
    TPaveStats *st = (TPaveStats *)ge->FindObject("stats");
    st->SetX1NDC(0.13467);
    st->SetY1NDC(0.664557);
    st->SetX2NDC(0.495702);
    st->SetY2NDC(0.864979);
    c1->Print("germanio_fit.png");
  }
  return ge;
}

void multiplot_exp() {
  TMultiGraph *mg = new TMultiGraph("mg", "");
  TGraphErrors *si = exponential_fit_ln("./dati_csv/silicio.csv", "",
                                        {"V", "V_err", "I", "I_err"}, false);
  TGraphErrors *ge = exponential_fit_ln("./dati_csv/germanio.csv", "",
                                        {"V", "V_err", "I", "I_err"}, false);
  si->SetMarkerStyle(kFullDotMedium);
  ge->SetMarkerStyle(kFullDotMedium);
  si->SetMarkerColor(kBlue);
  ge->SetMarkerColor(kRed);
  si->SetLineColor(kBlue);
  ge->SetLineColor(kRed);
  mg->Add(si);
  mg->Add(ge);
  mg->GetYaxis()->SetTitle("I [mA]");
  mg->GetXaxis()->SetTitle("V [mV]");
  mg->GetYaxis()->SetRangeUser(0., 3);

  TLegend *leg = new TLegend(.57, .71, .87, .86);
  // leg->SetX1NDC(0.571633);
  // leg->SetY1NDC(0.71308);
  // leg->SetX2NDC(0.872493);
  // leg->SetY2NDC(0.862869);
  leg->AddEntry(ge, "Germanio");
  leg->AddEntry(si, "Silicio");
  TCanvas *c = new TCanvas("c", "");
  c->SetTicks();
  c->SetGrid();
  mg->Draw("APE");
  leg->Draw("SAME");
  c->Print("caratteristiche _dati.png");
}

// int main() { linear_fit("calibrazione.csv"); }