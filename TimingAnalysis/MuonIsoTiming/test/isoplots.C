#include "TFile.h"
#include "TH1D.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TCut.h"
#include "TGraph.h"
#include "TLegend.h"

void isoplots() {
  
//   gStyle->SetOptStat(0111100);
  gStyle->SetOptStat(0);
  
  TFile *fin = TFile::Open("muontimingtt50.root");
  TTree *tree = (TTree*)fin->Get("muonIsoTiming/TimingTree"); 
  
  TFile *finprompt = TFile::Open("muontimingzmm50.root");
  TTree *treeprompt = (TTree*)finprompt->Get("muonIsoTiming/TimingTree"); 
  
  
  TFile *finnopu = TFile::Open("muontimingttnopu.root");
  TTree *treenopu = (TTree*)finnopu->Get("muonIsoTiming/TimingTree"); 
  
  TFile *finpromptnopu = TFile::Open("muontimingzmmnopu.root");
  TTree *treepromptnopu = (TTree*)finpromptnopu->Get("muonIsoTiming/TimingTree");   
  
  TCut promptsel = "genMatched && pt>15.";
  TCut fakesel = "!genMatched && pt>15.";
  
  TH1D *hptprompt = new TH1D("hptprompt","",17,15.,100.);
  TH1D *hptfake = new TH1D("hptfake","",17,15.,100.);
  
  treeprompt->Draw("pt>>hptprompt","genMatched","goff");
  tree->Draw("pt>>hptfake","!genMatched","goff");
  
  hptprompt->SetLineColor(kBlue);
  hptfake->SetLineColor(kRed);
  
  hptprompt->Scale(1./hptprompt->GetSumOfWeights());
  hptfake->Scale(1./hptfake->GetSumOfWeights());
  
  TCanvas *cpt = new TCanvas;
  hptfake->Draw("HIST");
  hptprompt->Draw("HISTSAME");
  
  TLegend *leg = new TLegend(0.6,0.6,0.85,0.85);
  leg->SetBorderSize(0);
  leg->SetFillStyle(0);
  leg->AddEntry(hptprompt,"Prompt PU50","L");
  leg->AddEntry(hptfake,"Fake PU50","L");
  leg->Draw();
  
  TH1D *hisoprompt = new TH1D("hisoprompt","",20,0.,20.);
  TH1D *hisocutprompt = new TH1D("hisocutprompt","",20,0.,20.);
  
  treeprompt->Draw("min(ecalPFIsoAll,19.9)>>hisoprompt",promptsel,"goff");
  treeprompt->Draw("min(ecalPFIsoTCut,19.9)>>hisocutprompt",promptsel,"goff");
    
  hisoprompt->SetLineColor(kMagenta);
  hisocutprompt->SetLineColor(kBlue);
  
//   hptprompt->Scale(1./hptprompt->GetSumOfWeights());
//   hptfake->Scale(1./hptfake->GetSumOfWeights());
  
  TCanvas *cisoprompt = new TCanvas;
  hisocutprompt->Draw("HIST");
  hisoprompt->Draw("HISTSAME");
  cisoprompt->SetLogy();
  
  TLegend *legisoprompt = new TLegend(0.6,0.6,0.85,0.85);
  legisoprompt->SetBorderSize(0);
  legisoprompt->SetFillStyle(0);
  legisoprompt->AddEntry(hisoprompt,"PU50","L");
  legisoprompt->AddEntry(hisocutprompt,"PU50+Timing Cut","L");
  legisoprompt->Draw();  
  
  
  TH1D *hisofake = new TH1D("hisofake","",20,0.,20.);
  TH1D *hisocutfake = new TH1D("hisocutfake","",20,0.,20.);
  
  tree->Draw("min(ecalPFIsoAll,19.9)>>hisofake",fakesel,"goff");
  tree->Draw("min(ecalPFIsoTCut,19.9)>>hisocutfake",fakesel,"goff");
  
  hisofake->SetLineColor(kMagenta);
  hisocutfake->SetLineColor(kBlue);
  
//   hptfake->Scale(1./hptfake->GetSumOfWeights());
//   hptfake->Scale(1./hptfake->GetSumOfWeights());
  
  TCanvas *cisofake = new TCanvas;
  hisocutfake->Draw("HIST");
  hisofake->Draw("HISTSAME");
  cisofake->SetLogy();
  
  TLegend *legisofake = new TLegend(0.6,0.6,0.85,0.85);
  legisofake->SetBorderSize(0);
  legisofake->SetFillStyle(0);
  legisofake->AddEntry(hisofake,"PU50","L");
  legisofake->AddEntry(hisocutfake,"PU50+Timing Cut","L");
  legisofake->Draw();  
  
  
  
  TH1D *hisopromptfine = new TH1D("hisopromptfine","",5000,0.,20.);
  TH1D *hisopromptnopufine = new TH1D("hisopromptnopufine","",5000,0.,20.);
  TH1D *hisocutpromptfine = new TH1D("hisocutpromptfine","",5000,0.,20.);
  
  treeprompt->Draw("ecalPFIsoAll>>hisopromptfine",promptsel,"goff");
  treepromptnopu->Draw("ecalPFIsoAll>>hisopromptnopufine",promptsel,"goff");
  treeprompt->Draw("ecalPFIsoTCut>>hisocutpromptfine",promptsel,"goff");    
  
  
  TH1D *hisofakefine = new TH1D("hisofakefine","",5000,0.,20.);
  TH1D *hisocutfakefine = new TH1D("hisocutfakefine","",5000,0.,20.);
  
  tree->Draw("ecalPFIsoAll>>hisofakefine",fakesel,"goff");
  tree->Draw("ecalPFIsoTCut>>hisocutfakefine",fakesel,"goff");   
  
  int nbins = hisopromptfine->GetNbinsX();
  
  TGraph *hrocall = new TGraph(nbins);
  TGraph *hroccut = new TGraph(nbins);
  
  double prompttotal = hisopromptfine->Integral(0,nbins+1);
  double promptnoputotal = hisopromptnopufine->Integral(0.,nbins+1);
  double faketotal = hisofakefine->Integral(0,nbins+1);
  
//   double promptcut = hisopromptfine->Integral(0,nbins+1);
  
  for (int ibin=0; ibin<nbins; ++ibin) {
    double promptall = hisopromptfine->Integral(0,ibin);
    double fakeall = hisofakefine->Integral(0,ibin);
    double promptcut = hisocutpromptfine->Integral(0,ibin);
    double fakecut = hisocutfakefine->Integral(0,ibin);
    
    double effpromptall = promptall/prompttotal;
    double efffakeall = fakeall/faketotal;
    double effpromptcut = promptcut/prompttotal;
    double efffakecut = fakecut/faketotal;
    
    hrocall->SetPoint(ibin,efffakeall,effpromptall);
    hroccut->SetPoint(ibin,efffakecut,effpromptcut);
    
  }
  
  hrocall->SetLineColor(kMagenta);
  hroccut->SetLineColor(kBlue);
  
  TCanvas *cisoroc = new TCanvas;
  hrocall->Draw("AL");
  hroccut->Draw("LSAME");
  
  
  TH1D *hdtprompt = new TH1D("hdtprompt","",50,-1.,1.);
  TH1D *hdtpromptnopu = new TH1D("hdtpromptnopu","",50,-1.,1.);
  
  treeprompt->Draw("clusTsmeared-vtxT>>hdtprompt",promptsel,"goff");
  treepromptnopu->Draw("clusTsmeared-vtxT>>hdtpromptnopu",promptsel,"goff"); 
  
  hdtprompt->SetLineColor(kMagenta);
  hdtpromptnopu->SetLineColor(kBlue);
  
  hdtprompt->Scale(1./prompttotal);
  hdtpromptnopu->Scale(1./promptnoputotal);
  
  
  hdtprompt->GetXaxis()->SetTitle("#Delta t(cluster,vtx0) (ns)");
  hdtprompt->GetYaxis()->SetTitle("# of clusters/bin/muon");
  
  TCanvas *cdtprompt = new TCanvas;
  hdtprompt->Draw("HIST");
  hdtpromptnopu->Draw("HISTSAME");
  
  TLegend *legpu = new TLegend(0.6,0.6,0.85,0.85);
  legpu->SetBorderSize(0);
  legpu->SetFillStyle(0);
  legpu->AddEntry(hdtpromptnopu,"NoPU","L");
  legpu->AddEntry(hdtprompt,"PU50","L");
  legpu->Draw();    
  
  cdtprompt->SaveAs("dtpromptsmeared.pdf");
  
  
//   TCut sel = "Entry$<100";
  
//   events->Draw("floatss_ecalBarrelClusterFastTimer_PerfectResolutionModel_RECO.obj[0][] - recoVertexs_offlinePrimaryVertices4D__RECO.obj[0].time_>>htimenopu",sel,"goff");
//   eventspu->Draw("floatss_ecalBarrelClusterFastTimer_PerfectResolutionModel_RECO.obj[0][] - recoVertexs_offlinePrimaryVertices4D__RECO.obj[0].time_>>htimepu",sel,"goff");

  
//   eventspu->Draw("floatss_ecalBarrelClusterFastTimer_PerfectResolutionModel_RECO.obj[0][]>>htimepu",sel,"goff");
  
//   htimepu->SetLineColor(kRed);
//   htimepu->SetMarkerColor(kRed);
//   
//   htimepu->GetXaxis()->SetTitle("#Delta t(cluster,vtx0) (ns)");
//   htimepu->GetYaxis()->SetTitle("# of clusters");
//   
//   TCanvas *c = new TCanvas;
//   htimepu->Draw("HIST");
//   htimepu->SetMinimum(0.);
//   htimenopu->Draw("HISTSAME");
//   
//   TLegend *leg = new TLegend(0.6,0.6,0.85,0.85);
//   leg->AddEntry(htimenopu,"NoPU","L");
//   leg->AddEntry(htimepu,"PU50","L");
//   leg->Draw();
//   
//   c->SaveAs("timing.pdf");
}
