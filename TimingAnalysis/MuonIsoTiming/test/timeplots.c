#include "TFile.h"
#include "TH1D.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TCut.h"

void timeplots() {
  
//   gStyle->SetOptStat(0111100);
  gStyle->SetOptStat(0);
  
  TFile *fin = TFile::Open("root://cmsxrootd.fnal.gov///store/mc/TP2023HGCALDR/DYToMuMu_M-20_TuneZ2star_14TeV-pythia6-tauola/GEN-SIM-RECO/62028p1_NoPU_FastTimeHLLHC_PH2_1K_FB_V6-v2/00000/0C7ABC3E-091F-E611-92D1-842B2B180882.root");

  TTree *events = (TTree*)fin->Get("Events");

//   events->Print();
//   return;
  
  TFile *finpu = TFile::Open("root://cmsxrootd.fnal.gov///store/mc/TP2023HGCALDR/DYToMuMu_M-20_TuneZ2star_14TeV-pythia6-tauola/GEN-SIM-RECO/62028p1_PU50NoOOT_FastTimeHLLHC_PH2_1K_FB_V6-v1/10000/00D9C228-B12E-E611-942D-0025901AC182.root");
  TTree *eventspu = (TTree*)finpu->Get("Events");
  
//   events->Print();
  
  TH1D *htimenopu = new TH1D("htimenopu","",200,-1.,1.);
  TH1D *htimepu = new TH1D("htimepu","",200,-1.,1.);
  
  TCut sel = "Entry$<100";
  
  events->Draw("floatss_ecalBarrelClusterFastTimer_PerfectResolutionModel_RECO.obj[0][] - recoVertexs_offlinePrimaryVertices4D__RECO.obj[0].time_>>htimenopu",sel,"goff");
  eventspu->Draw("floatss_ecalBarrelClusterFastTimer_PerfectResolutionModel_RECO.obj[0][] - recoVertexs_offlinePrimaryVertices4D__RECO.obj[0].time_>>htimepu",sel,"goff");

  
//   eventspu->Draw("floatss_ecalBarrelClusterFastTimer_PerfectResolutionModel_RECO.obj[0][]>>htimepu",sel,"goff");
  
  htimepu->SetLineColor(kRed);
  htimepu->SetMarkerColor(kRed);
  
  htimepu->GetXaxis()->SetTitle("#Delta t(cluster,vtx0) (ns)");
  htimepu->GetYaxis()->SetTitle("# of clusters");
  
  TCanvas *c = new TCanvas;
  htimepu->Draw("HIST");
  htimepu->SetMinimum(0.);
  htimenopu->Draw("HISTSAME");
  
  TLegend *leg = new TLegend(0.6,0.6,0.85,0.85);
  leg->AddEntry(htimenopu,"NoPU","L");
  leg->AddEntry(htimepu,"PU50","L");
  leg->Draw();
  
  c->SaveAs("timing.pdf");
}
