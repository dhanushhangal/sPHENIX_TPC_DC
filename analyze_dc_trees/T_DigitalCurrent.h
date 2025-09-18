//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Mon Jul 28 14:09:02 2025 by ROOT version 6.34.08
// from TTree T_DigitalCurrent/DigitalCurrent Debug TTree
// found on file: DST_STREAMING_EVENT_ebdc23_1_run3auau_physics_new_nocdbtag-00069414-00000_DigitalCurrentDebugTTree.root
//////////////////////////////////////////////////////////

#ifndef T_DigitalCurrent_h
#define T_DigitalCurrent_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.

class T_DigitalCurrent {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

// Fixed size dimensions of array or collections stored in the TTree if any.

   // Declaration of leaf types
   ULong64_t       dc_gtm_bco;
   UInt_t          dc_bx_timestamp_predicted;
   UShort_t        dc_fee;
   UShort_t        dc_pkt_length;
   UShort_t        dc_channel;
   UShort_t        dc_sampa_address;
   UInt_t          dc_bx_timestamp;
   UInt_t          dc_current[8];
   UInt_t          dc_nsamples[8];
   UShort_t        dc_data_crc;
   UShort_t        dc_calc_crc;

   // List of branches
   TBranch        *b_dc;   //!

   T_DigitalCurrent(const char* input_filename);
   virtual ~T_DigitalCurrent();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop(const char* input_filename);
   virtual bool     Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef T_DigitalCurrent_cxx
T_DigitalCurrent::T_DigitalCurrent(const char* input_filename) : fChain(0) 
{
    TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject(input_filename);
    if (!f || !f->IsOpen()) {
        f = new TFile(input_filename);
    }
    TTree *tree = nullptr;
    f->GetObject("T_DigitalCurrent", tree);

    Init(tree);
}

T_DigitalCurrent::~T_DigitalCurrent()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t T_DigitalCurrent::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t T_DigitalCurrent::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (fChain->GetTreeNumber() != fCurrent) {
      fCurrent = fChain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void T_DigitalCurrent::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("dc", &dc_gtm_bco, &b_dc);
   Notify();
}

bool T_DigitalCurrent::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return true;
}

void T_DigitalCurrent::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t T_DigitalCurrent::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef T_DigitalCurrent_cxx
