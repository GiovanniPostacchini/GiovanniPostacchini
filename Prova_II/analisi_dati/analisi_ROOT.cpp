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
#include <valarray>

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
std::vector<double> absolute(std::vector<double> v) {
  auto v_copy = v;
  int size = v.size();
  for (int n = 0; n < size; ++n) {
    v_copy[n] = std::abs(v[n]);
  }
  return v_copy;
}
void linear_fit(std::string filename = "./dati_csv/IV200.csv",
                std::array<std::string, 4> column = {"V", "V_err", "I",
                                                     "I_err"},
                std::string title = "Fit Lineare") {
  gStyle->SetOptFit(1111);

  std::vector<double> vx0 = read_csv<double>(filename, column[2], ';');
  std::vector<double> vx_err0 = read_csv<double>(filename, column[3], ';');
  std::vector<double> vy0 = read_csv<double>(filename, column[0], ';');
  std::vector<double> vy_err0 = read_csv<double>(filename, column[1], ';');

  std::vector<double> vx = absolute(vx0);
  std::vector<double> vx_err = absolute(vx_err0);
  std::vector<double> vy = absolute(vy0);
  std::vector<double> vy_err = absolute(vy_err0);

  int size = vx.size();
  TGraphErrors *ge = new TGraphErrors(size, &vx[0], &vy[0], &vx_err[0],
                                      &vy_err[0]); // X Y EX EY
  TF1 *line = new TF1("line", "[0]+[1]*x", 26.2, 30.42);
  line->SetParameters(0, 1);
  line->SetLineColor(kOrange + 10);
  line->SetParNames("V_{ey}", "r"); // A + BX = 0
  ge->Fit(line, "R");
  ge->SetTitle(title.c_str());
  ge->GetYaxis()->SetTitle("Tensione Collettore-Emettitore |V_{CE}|   [V]");
  ge->GetXaxis()->SetTitle("Corrente Collettore |I_{E}|   [mA]");
  ge->GetXaxis()->SetDecimals();
  ge->SetMarkerStyle(kFullDotMedium);
  ge->SetLineColor(kGray + 3);
  ge->SetMarkerColor(kBlack);

  std::cout << "Par A\t" << line->GetParameter(0) << "\t+/-\t"
            << line->GetParError(0) << '\n';
  std::cout << "Par B\t" << line->GetParameter(1) << "\t+/-\t"
            << line->GetParError(1) << '\n';

  TCanvas *c1 = new TCanvas("c", " ");
  c1->SetGrid();
  c1->SetTicks();
  ge->Draw("APEZ");
  gPad->Update();
  TPaveStats *st = (TPaveStats *)ge->FindObject("stats");
  st->SetX1NDC(/*0.495702*/ 0.17);
  st->SetY1NDC(/*0.399789*/ 0.63);
  st->SetX2NDC(/*0.858166*/ 0.53);
  st->SetY2NDC(/*0.600211*/ 0.83);
  c1->Print("IV200_inverted.png");
  // ge->Print();
}

// for exponential fit --> linear fit
TGraphErrors *exponential_fit_ln(
    std::string filename = "./dati_csv/germanio.csv",
    std::string title = "Caratteristica Tensione Corrente Diodo Ge",
    std::array<std::string, 4> column = {"V", "V_err", "I", "I_err"},
    bool const fit = true) {

  std::vector<double> vx0 = read_csv<double>(filename, column[0], ';');
  std::vector<double> vx_err0 = read_csv<double>(filename, column[1], ';');
  std::vector<double> vy0 = read_csv<double>(filename, column[2], ';');
  std::vector<double> vy_err0 = read_csv<double>(filename, column[3], ';');

  std::vector<double> vx = absolute(vx0);
  std::vector<double> vx_err = absolute(vx_err0);
  std::vector<double> vy = absolute(vy0);
  std::vector<double> vy_err = absolute(vy_err0);
  int size = vx.size();

  TGraphErrors *ge = new TGraphErrors(size, &vx[0], &vy[0], &vx_err[0],
                                      &vy_err[0]); // X Y EX EY

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
  TGraphErrors *si = exponential_fit_ln("./dati_csv/IV100.csv", "",
                                        {"V", "V_err", "I", "I_err"}, false);
  TGraphErrors *ge = exponential_fit_ln("./dati_csv/IV200.csv", "",
                                        {"V", "V_err", "I", "I_err"}, false);
  si->SetMarkerStyle(kFullDotMedium);
  ge->SetMarkerStyle(kFullDotMedium);
  si->SetMarkerSize(1);
  ge->SetMarkerSize(1);
  si->SetMarkerColor(kBlack);
  ge->SetMarkerColor(kBlack);
  si->SetLineColor(kRed);
  ge->SetLineColor(kBlue);
  mg->Add(si);
  mg->Add(ge);
  mg->GetYaxis()->SetTitle("Corrente Collettore |I_{C}|   [mA]");
  mg->GetXaxis()->SetTitle("Tensione Collettore-Emettitore |V_{CE}|   [V]");
  mg->GetYaxis()->SetRangeUser(0, 35);
  mg->GetXaxis()->SetDecimals();

  TLegend *leg = new TLegend(.147, .753, .420, .859);
  // leg->SetX1NDC(0.571633);
  // leg->SetY1NDC(0.71308);
  // leg->SetX2NDC(0.872493);
  // leg->SetY2NDC(0.862869);
  leg->AddEntry(ge, "I_{B} = -200 #muA");
  leg->AddEntry(si, "I_{B} = -100 #muA");
  TCanvas *c = new TCanvas("c", "");
  c->SetTicks();
  c->SetGrid();
  mg->Draw("APEZ");
  leg->Draw("SAME");
  c->Update();
  c->Print("IVdati.png");
}

// int main() { linear_fit("calibrazione.csv"); }