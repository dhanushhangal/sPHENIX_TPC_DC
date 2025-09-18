// run_digital_current.C
#include "TROOT.h"
#include "TSystem.h"
#include "TString.h"
#include "TStopwatch.h"
#include <cstdio>

// Run like:
//   root -l -b -q 'run_digital_current.C()'
// Or with args:
//   root -l -b -q 'run_digital_current.C("T_DigitalCurrent.C","dc_debug_trees/run72440",0,9,0,1,true)'
void run_digital_current(const char* macro    = "T_DigitalCurrent.C",
                         const char* inputDir = "dc_debug_trees/run72440",
                         int iStart = 0, int iEnd = 23,
                         int jStart = 0, int jEnd = 1,
                         bool compileMacro = true)
{
  TStopwatch sw; sw.Start();

  // Compile / load the analysis macro once
  TString loadCmd = compileMacro
                    ? Form(".L %s+", macro)
                    : Form(".L %s",  macro);
  if (gROOT->ProcessLine(loadCmd) != 0) {
    ::Error("run_digital_current", "Failed to load: %s", loadCmd.Data());
    return;
  }

  // Process all (i, j)
  for (int i = iStart; i <= iEnd; ++i) {
    for (int j = jStart; j <= jEnd; ++j) {

      // Matches ebdc00..ebdc09 for i=0..9, and ebdc10.. for >=10 (like %02d)
      TString file = Form("%s/DST_STREAMING_EVENT_ebdc%02d_%d_run3auau_physics_new_nocdbtag-00072440-00000_DigitalCurrentDebugTTree.root",
                          inputDir, i, j);

      // Skip if missing to avoid hard errors
      if (gSystem->AccessPathName(file)) {
        printf("Skipping missing file: %s\n", file.Data());
        continue;
      }

      printf("Processing: %s\n", file.Data());

      // Construct and run via the interpreter so we don't require a header
      // (T_DigitalCurrent becomes known after the .L above).
      gROOT->ProcessLine(Form("T_DigitalCurrent t(\"%s\"); t.Loop(\"%s\");",
                              file.Data(), file.Data()));
    }
  }

  sw.Stop();
  printf("\nDone.\n");
  sw.Print();
}
