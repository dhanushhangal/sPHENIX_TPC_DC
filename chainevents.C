#include <iostream>
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

#include "assert.h"
#include <fstream>
#include "TMath.h"
#include <vector>
#include <random>

using namespace std;

// Function to sample from a Poisson distribution with a given mean
int sample_poisson(double mean) {
    static std::random_device rd;         // Random seed (initialized once)
    static std::mt19937 gen(rd());        // Mersenne Twister engine (shared across calls)

    std::poisson_distribution<> dist(mean);
    return dist(gen);
}

void chainevents(const std::string& input_file_path = "/sphenix/tg/tg01/jets/hangal/dc_out/May20/hits_adc_53534_14_resid.root", double collision_rate = 15000.0){

    long long n_hits=0;
    int event=0, m_tbin=0, previous_event=0;
    float m_hitgx=0, m_hitgy=0, m_hitgz=0, m_adc=0, r=0, pancake_z=0,phi=0;
    //const int t_start = 450;
    const int t_start = 0;
    //double collision_rate = 15000;
    int pois_coll_rate = 0;
    const double fulltpc_drifttime = 0.078; //78 ms
    double full_tpc_events = 0;
    const int gain = 5; 

  const int r_bins_N = 66;  // 51;
  double r_bins[r_bins_N + 1] = {217.83,
                                 223.83, 229.83, 235.83, 241.83, 247.83, 253.83, 259.83, 265.83, 271.83, 277.83, 283.83, 289.83, 295.83, 301.83, 306.83,
                                 311.05, 317.92, 323.31, 329.27, 334.63, 340.59, 345.95, 351.91, 357.27, 363.23, 368.59, 374.55, 379.91, 385.87, 391.23, 397.19, 402.49,
                                 411.53, 421.70, 431.90, 442.11, 452.32, 462.52, 472.73, 482.94, 493.14, 503.35, 513.56, 523.76, 533.97, 544.18, 554.39, 564.59, 574.76,
                                 583.67, 594.59, 605.57, 616.54, 627.51, 638.48, 649.45, 660.42, 671.39, 682.36, 693.33, 704.30, 715.27, 726.24, 737.21, 748.18, 759.11};

  int nz = 72;
  double z_rdo = 1080;

  double *z_bins = new double[2 * nz + 1];
  for (int z = 0; z <= 2 * nz; z++)
  {
    z_bins[z] = -z_rdo + z_rdo / nz * z;
  }

  const int nphi = 205;

  double phi_bins[nphi + 1] = {0., 6.3083 - 2 * M_PI, 6.3401 - 2 * M_PI, 6.372 - 2 * M_PI, 6.4039 - 2 * M_PI, 6.4358 - 2 * M_PI, 6.4676 - 2 * M_PI, 6.4995 - 2 * M_PI, 6.5314 - 2 * M_PI,
                               0.2618, 0.2937, 0.3256, 0.3574, 0.3893, 0.4212, 0.453, 0.4849, 0.5168, 0.5487, 0.5805, 0.6124, 0.6443, 0.6762, 0.7081,
                               0.7399, 0.7718, 0.7854, 0.8173, 0.8491, 0.881, 0.9129, 0.9448, 0.9767, 1.0085, 1.0404, 1.0723, 1.1041, 1.136, 1.1679,
                               1.1998, 1.2317, 1.2635, 1.2954, 1.309, 1.3409, 1.3727, 1.4046, 1.4365, 1.4684, 1.5002, 1.5321, 1.564, 1.5959, 1.6277,
                               1.6596, 1.6915, 1.7234, 1.7552, 1.7871, 1.819, 1.8326, 1.8645, 1.8963, 1.9282, 1.9601, 1.992, 2.0238, 2.0557, 2.0876,
                               2.1195, 2.1513, 2.1832, 2.2151, 2.247, 2.2788, 2.3107, 2.3426, 2.3562, 2.3881, 2.42, 2.4518, 2.4837, 2.5156, 2.5474,
                               2.5793, 2.6112, 2.6431, 2.6749, 2.7068, 2.7387, 2.7706, 2.8024, 2.8343, 2.8662, 2.8798, 2.9117, 2.9436, 2.9754, 3.0073,
                               3.0392, 3.0711, 3.1029, 3.1348, 3.1667, 3.1986, 3.2304, 3.2623, 3.2942, 3.326, 3.3579, 3.3898, 3.4034, 3.4353, 3.4671,
                               3.499, 3.5309, 3.5628, 3.5946, 3.6265, 3.6584, 3.6903, 3.7221, 3.754, 3.7859, 3.8178, 3.8496, 3.8815, 3.9134, 3.927,
                               3.9589, 3.9907, 4.0226, 4.0545, 4.0864, 4.1182, 4.1501, 4.182, 4.2139, 4.2457, 4.2776, 4.3095, 4.3414, 4.3732, 4.4051,
                               4.437, 4.4506, 4.4825, 4.5143, 4.5462, 4.5781, 4.61, 4.6418, 4.6737, 4.7056, 4.7375, 4.7693, 4.8012, 4.8331, 4.865,
                               4.8968, 4.9287, 4.9606, 4.9742, 5.0061, 5.0379, 5.0698, 5.1017, 5.1336, 5.1654, 5.1973, 5.2292, 5.2611, 5.2929, 5.3248,
                               5.3567, 5.3886, 5.4204, 5.4523, 5.4842, 5.4978, 5.5297, 5.5615, 5.5934, 5.6253, 5.6572, 5.689, 5.7209, 5.7528, 5.7847,
                               5.8165, 5.8484, 5.8803, 5.9122, 5.944, 5.9759, 6.0078, 6.0214, 6.0533, 6.0851, 6.117, 6.1489, 6.1808, 6.2127, 6.2445,
                               6.2764, 2 * M_PI};

    TH3D *h_hit_phi_r_z = new TH3D("h_hit_phi_r_z","",nphi, phi_bins, r_bins_N, r_bins, 2*nz, z_bins); h_hit_phi_r_z->Sumw2();
    TH3D *h_hit_phi_r_z_gainnorm = new TH3D("h_hit_phi_r_z_gainnorm","",nphi, phi_bins, r_bins_N, r_bins, 2*nz, z_bins); h_hit_phi_r_z_gainnorm->Sumw2();
    TH1D *h_t_adc_n = new TH1D("h_t_adc_n","",1000,0,1000); h_t_adc_n->Sumw2();
    TH1D *h_t_adc_s = new TH1D("h_t_adc_s","",1000,0,1000); h_t_adc_s->Sumw2();
    TH1D *h_adc_n = new TH1D("h_adc_n","",200,0,2000); h_adc_n->Sumw2();
    TH1D *h_adc_s = new TH1D("h_adc_s","",200,0,2000); h_adc_s->Sumw2();
    TH1D *h_event_hits = new TH1D("h_event_hits","",3200,0,3200); h_event_hits->Sumw2();

    //TFile *my_file = TFile::Open("/sphenix/tg/tg01/jets/hangal/dc_out/May20/hits_adc_53534_14_resid.root");
    TFile *my_file = TFile::Open(input_file_path.c_str());

    if(!my_file){ 
        cout << "File cannot be found!!" << endl; 
	}	
	if(my_file->IsZombie()) { 
	    cout << "Is zombie" << endl;
	}
	cout<<"Running on file "<<my_file->GetName()<<endl;

	TTree *inp_tree;
	inp_tree = (TTree*)  my_file->Get("hittree");

	cout << "tree loaded" << endl;

    inp_tree->SetBranchStatus("*", 0);

    inp_tree->SetBranchStatus("event",1);        
    inp_tree->SetBranchStatus("gx",1);        
    inp_tree->SetBranchStatus("gy",1);
    inp_tree->SetBranchStatus("gz",1);
    inp_tree->SetBranchStatus("adc",1);
    inp_tree->SetBranchStatus("tbin",1);

    inp_tree->SetBranchAddress("event",&event);
    inp_tree->SetBranchAddress("gx",&m_hitgx);
    inp_tree->SetBranchAddress("gy",&m_hitgy);
    inp_tree->SetBranchAddress("gz",&m_hitgz);
    inp_tree->SetBranchAddress("adc",&m_adc);
    inp_tree->SetBranchAddress("tbin",&m_tbin);

	n_hits = inp_tree->GetEntriesFast();

        pois_coll_rate = sample_poisson(collision_rate);
	full_tpc_events = pois_coll_rate*fulltpc_drifttime;
	//n_hits=5000000;
	cout << "Hits: "<< n_hits << endl;
	cout << "Chaining together "<<full_tpc_events<<" events"<<endl;
    for(int evi = 0; evi < n_hits; evi++) {

		inp_tree->GetEntry(evi);
        if(evi%10000000 == 0) cout<<evi<<" out of "<<n_hits<<endl; 		
	if(event > full_tpc_events)break;
	if(event!=previous_event) pancake_z = (1080./full_tpc_events)*event;
	h_event_hits->Fill(event);
	//if(evi%10000000 == 0) cout<<event<<" p: "<<pois_coll_rate<<"  "<<full_tpc_events<<"  "<<pancake_z<<endl;
	if(m_adc > 1000 || m_tbin <= t_start)continue;
	//if(m_adc > 1000)continue;
	r = 10.*sqrt((m_hitgx*m_hitgx) + (m_hitgy*m_hitgy));
	phi = atan2(m_hitgy,m_hitgx)+M_PI;
	if(m_hitgz>50){
		h_t_adc_n->Fill(m_tbin, m_adc);
		h_adc_n->Fill(m_adc);
		h_hit_phi_r_z->Fill(phi,r,pancake_z, m_adc);
	}
	else if(m_hitgz<50){
		h_t_adc_s->Fill(m_tbin, m_adc);
		h_adc_s->Fill(m_adc);
		h_hit_phi_r_z->Fill(phi,r,-1*pancake_z, m_adc);
	}
		
	previous_event = event;

    }//event loop

    double bincontent = 0;
    for(int i=1; i<h_hit_phi_r_z->GetNbinsX()+1; i++){
        for(int j=1; j<h_hit_phi_r_z->GetNbinsY()+1; j++){
                for(int k=1; k<h_hit_phi_r_z->GetNbinsZ()+1; k++){
                        bincontent = h_hit_phi_r_z->GetBinContent(i,j,k);
                        if(bincontent>0) h_hit_phi_r_z_gainnorm->SetBinContent(i,j,k,bincontent*gain);
                        //if(bincontent>0) h_adc_phi_r_z->SetBinContent(i,j,k,1);
                        else h_hit_phi_r_z_gainnorm->SetBinContent(i,j,k,1);
                }
        }
    }

    my_file->Close();

    TString input = TString(input_file_path);
    TString run_seg = input(63,8);
    TString output_file_path = "/sphenix/tg/tg01/jets/hangal/chained_events/";
    TString output_file_name = output_file_path+"chained_file_"+run_seg+".root";
    //TString output_file_name = "chained_file_tmp.root";
    TFile *output_file = new TFile(output_file_name, "RECREATE");
    output_file->cd();
    h_hit_phi_r_z->Write();
    h_hit_phi_r_z_gainnorm->Write();
    h_t_adc_n->Write();
    h_adc_n->Write();
    h_t_adc_s->Write();
    h_adc_s->Write();
    h_event_hits->Write();
    output_file->Close();

 
}
