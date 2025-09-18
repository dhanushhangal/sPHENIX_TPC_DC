#define T_DigitalCurrent_cxx
#include "T_DigitalCurrent.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>

#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TRandom.h"
#include "TProfile.h"
#include "TProfile2D.h"
#include "TH2D.h"
#include "TH3D.h"
#include "TF1.h"

#include "TH2F.h"
#include "TMath.h"
#include <TNtuple.h>
#include "TChain.h"
#include <TString.h>
#include <TCut.h>
#include "TStopwatch.h"
#include "TEnv.h"
#include "TStopwatch.h"
#include "TObjArray.h"

#include "assert.h"
#include <fstream>
#include "TMath.h"
#include <vector>

void T_DigitalCurrent::Loop(const char* input_filename)
{
   int adc = 0;
   double min_adc = 0.;
   double max_adc = 3.e4;

   if (fChain == 0) return;

   Long64_t nentries = fChain->GetEntriesFast();
   Long64_t nbytes = 0, nb = 0;

   //define start and end time for run
   //run 72440
   double start_time = 984.659e9;
   double end_time = 984.696e9;

   //26 FEEs
   TH2D *h_channel_gtmbco_current[26];
   TH2D *h_channel_frameindex[26];
   for(int i=0; i<26; i++){
      h_channel_gtmbco_current[i] = new TH2D(Form("h_channel_gtmbco_current_FEE%d",i),"",256,0,256,100,start_time,end_time);
      //defining h_channel_frameindex only for 80 indices at a time for a full TPC volume
      h_channel_frameindex[i] = new TH2D(Form("h_channel_frameindex_FEE%d",i),"",256,0,256,80,0,80);
   }

   int frame_index = 0;
   int start_frame_index = 0;

   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;

      if(jentry%1000000 == 0) cout<<jentry<<" out of "<<nentries<<endl;

      if(dc_data_crc!=dc_calc_crc) continue;
      if(dc_gtm_bco<start_time) continue;
      
      //frame_index = int(dc_gtm_bco/9720) - <starting_index>;
      frame_index = int(dc_gtm_bco/9720) - 101302466;

      if(frame_index<start_frame_index) continue;
      if(frame_index>start_frame_index+80) break;
      frame_index = frame_index-start_frame_index;

      for(int i=0; i<8; i++){
         adc = dc_current[i] - 60 * dc_nsamples[i];
         if(adc < min_adc || adc > max_adc) continue;
         h_channel_gtmbco_current[dc_fee]->Fill(dc_channel-(7-i), dc_gtm_bco, adc);
         h_channel_frameindex[dc_fee]->Fill(dc_channel-(7-i), frame_index, adc);
      }
   }

   TString inFile(input_filename);
   TString outFile = "output_volume_" + inFile(inFile.Last('/')+1, inFile.Length());
   cout<<input_filename<<"  "<<outFile<<endl;
   TFile *output_file = new TFile(outFile.Data(), "RECREATE");
   output_file->cd();

   for(int i=0; i<26; i++){
      h_channel_gtmbco_current[i]->Write();
      h_channel_frameindex[i]->Write();
   }
   output_file->Close();
}
