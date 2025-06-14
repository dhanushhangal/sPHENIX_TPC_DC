#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/Fun4AllDstOutputManager.h>
#include <fun4all/Fun4AllRunNodeInputManager.h>
#include <fun4all/Fun4AllUtils.h>

#include <phool/PHRandomSeed.h>
#include <phool/recoConsts.h>

// Micromegas
#include <micromegas/MicromegasCombinedDataDecoder.h>
#include <micromegas/MicromegasCombinedDataEvaluation.h>
#include <micromegas/MicromegasClusterizer.h>

// INTT
#include <intt/InttCombinedRawDataDecoder.h>
#include <intt/InttClusterizer.h>

// TPC
#include <tpc/TpcCombinedRawDataUnpacker.h>

#include <tpccalib/PHTpcResiduals.h>

#include <trackingdiagnostics/TrackResiduals.h>



// local macros
//#include "G4Setup_sPHENIX.C"
#include "G4_Global.C"

#include "Trkr_RecoInit.C"
#include "Trkr_TpcReadoutInit.C"

#include "Trkr_Clustering.C"
#include "Trkr_Reco.C"

R__LOAD_LIBRARY(libfun4all.so)


R__LOAD_LIBRARY(libmicromegas.so)

// tag = "ana464_nocdbtag_v001/"

#include "make_filelist.C"

//49709
//53513
//____________________________________________________________________
int Fun4All_CombinedDataReconstruction_hits(
  const int nEvents = 1,
  const std::string filelist = "filelist_ana478_nocdbtag_v002_run00053513_seg00000.list",
  const std::string outfilename = "hits_adc"
  )
{
  bool TPC_ONLY = true;


  const char* outputFile =  "dst_00053513_0000.root";

  const int nSkipEvents = 0; 
  const char* tag = "ana464_nocdbtag_v001";

  auto se = Fun4AllServer::instance();
  se->Verbosity(0);
  auto rc = recoConsts::instance();
  
  std::ifstream ifs(filelist);
  std::string filepath;
  int runnumber = std::numeric_limits<int>::quiet_NaN();
  int segment = std::numeric_limits<int>::quiet_NaN();
  int i = 0;
  while(std::getline(ifs,filepath))
    {
      std::cout << "Adding DST with filepath: " << filepath << std::endl; 
     if(i==0)
	{
	   std::pair<int, int> runseg = Fun4AllUtils::GetRunSegment(filepath);
	   runnumber = runseg.first;
	   segment = runseg.second;
	   rc->set_IntFlag("RUNNUMBER", runnumber);
	   rc->set_uint64Flag("TIMESTAMP", runnumber);
        
	}
      std::string inputname = "InputManager" + std::to_string(i);
      auto hitsin = new Fun4AllDstInputManager(inputname);
      hitsin->fileopen(filepath);
      se->registerInputManager(hitsin);
      i++;
    }

  //std::string outdir = "/sphenix/u/hangal/digital_current/";
  std::string outdir = "/sphenix/tg/tg01/jets/hangal/dc_out/pp_streaming_data/";
  TString outfile = outdir + outfilename + "_" + runnumber + "_" + segment;
  std::string theOutfile = outfile.Data();
  TString residualFile = theOutfile + "_resid.root";
  std::string residstring(residualFile.Data());
  // print inputs
  std::cout << "Fun4All_CombinedDataReconstruction - nEvents: " << nEvents << std::endl;
  std::cout << "Fun4All_CombinedDataReconstruction - nSkipEvents: " << nSkipEvents << std::endl;
  std::cout << "Fun4All_CombinedDataReconstruction - tag: " << tag << std::endl;
  std::cout << "Fun4All_CombinedDataReconstruction - runnumber: " << runnumber << std::endl;
  std::cout << "Fun4All_CombinedDataReconstruction - segment: " << segment << std::endl;
  std::cout << "Fun4All_CombinedDataReconstruction - residualFile: " << residstring << std::endl;



   Enable::CDB = true;

  //! flags to set
  TRACKING::tpc_zero_supp = true;
  TRACKING::pp_mode = true;
  Enable::MVTX_APPLYMISALIGNMENT = true;
  ACTSGEOM::mvtx_applymisalignment = Enable::MVTX_APPLYMISALIGNMENT;
  G4TRACKING::convert_seeds_to_svtxtracks = true;



  rc->set_StringFlag("CDB_GLOBALTAG", "ProdA_2024");



  // try use hard coded geometry file
  // /cvmfs/sphenix.sdcc.bnl.gov/calibrations/sphnxpro/cdb/Tracking_Geometry/./87/6a/876abfa36f58e100f29e6641f9ff4f5b_tracking_geometry_50nsclock.root
  TpcReadoutInit( runnumber );
  // TPC unpacker t0
  float tpcunpacker_t0 = 0;
    CDBInterface *cdb = CDBInterface::instance();
  std::string tpc_dv_calib_dir = cdb->getUrl("TPC_DRIFT_VELOCITY");
 if(false  )//!tpc_dv_calib_dir.empty()
  {

    CDBTTree *cdbttree = new CDBTTree(tpc_dv_calib_dir);
    cdbttree->LoadCalibrations();
    G4TPC::tpc_drift_velocity_reco = cdbttree->GetSingleFloatValue("tpc_drift_velocity");
    std::cout << "Use calibrated TPC drift velocity for Run " << runnumber << ": " << G4TPC::tpc_drift_velocity_reco << " cm/ns" << std::endl;

  } else {

  
    G4TPC::tpc_drift_velocity_reco = 0.0070174;

    TpcClusterZCrossingCorrection::_vdrift = G4TPC::tpc_drift_velocity_reco;

    tpcunpacker_t0 = -3.65;
 //tpcunpacker_t0 = -4;
  }

  // printout
  std::cout<< "Fun4All_CombinedDataReconstruction - samples: " << TRACKING::reco_tpc_maxtime_sample << std::endl;
  std::cout<< "Fun4All_CombinedDataReconstruction - pre: " << TRACKING::reco_tpc_time_presample << std::endl;
  std::cout<< "Fun4All_CombinedDataReconstruction - vdrift: " << G4TPC::tpc_drift_velocity_reco << std::endl;

  ACTSGEOM::mvtxMisalignment = 100;
  ACTSGEOM::inttMisalignment = 100;
  ACTSGEOM::tpotMisalignment = 100;


  PHRandomSeed::Verbosity(1);


  // tracking geometry
  std::string geofile = CDBInterface::instance()->getUrl("Tracking_Geometry");
  std::cout << "CDB tracking geometry file "<<geofile << std::endl;
  Fun4AllRunNodeInputManager *ingeo = new Fun4AllRunNodeInputManager("GeoIn");
  ingeo->AddFile(geofile);
  se->registerInputManager(ingeo);

  // distortion correction
  if(false)
  {
    // module edge corrections
    G4TPC::ENABLE_MODULE_EDGE_CORRECTIONS = false;

    // static distortions
    G4TPC::ENABLE_STATIC_CORRECTIONS = false;

    // average distortions
    G4TPC::ENABLE_AVERAGE_CORRECTIONS = false;
  }

  //CDBInterface *cdb = CDBInterface::instance();



  // tpc zero suppression
  TRACKING::tpc_zero_supp = true;

// can use for zero field
  //double fieldstrength = 0.01;
 // G4MAGNET::magfield_tracking = "0.01";
  double fieldstrength = std::numeric_limits<double>::quiet_NaN();
  bool ConstField = isConstantField(G4MAGNET::magfield_tracking,fieldstrength);

  if(ConstField && fieldstrength < 0.1)
  {
    G4MAGNET::magfield = "0.01";
    G4MAGNET::magfield_rescale = 1;
  }
  TrackingInit();

  // input managers
  //{
  //  const auto filelist = make_filelist( tag, runnumber, segment );
  //  for( size_t i = 0; i < filelist.size(); ++i )
  //  {
  //    const auto& inputfile = filelist[i];
  //    auto in = new Fun4AllDstInputManager(Form("DSTin_%zu", i));
  //    in->fileopen(inputfile);
  //    se->registerInputManager(in);
  //  }
  //}

  // hit unpackers
  if (!TPC_ONLY){
    for(int felix=0; felix < 6; felix++)
    { Mvtx_HitUnpacking(std::to_string(felix)); }

    for(int server = 0; server < 8; server++)
    { Intt_HitUnpacking(std::to_string(server)); }
  }

  {
    // TPC unpacking
    for(int ebdc = 0; ebdc < 24; ebdc++)
    {
      const std::string ebdc_string = (Form( "%02i", ebdc ));

      auto tpcunpacker = new TpcCombinedRawDataUnpacker("TpcCombinedRawDataUnpacker"+ebdc_string);
      tpcunpacker->useRawHitNodeName("TPCRAWHIT_" + ebdc_string);
      tpcunpacker->set_presampleShift(TRACKING::reco_tpc_time_presample);
      tpcunpacker->set_t0( tpcunpacker_t0 );
      if(TRACKING::tpc_zero_supp)
      { tpcunpacker->ReadZeroSuppressedData(); }

      tpcunpacker->doBaselineCorr(TRACKING::tpc_baseline_corr);
      se->registerSubsystem(tpcunpacker);
    }

  }

    if (!TPC_ONLY){
    // micromegas unpacking
  
    auto tpotunpacker = new MicromegasCombinedDataDecoder;
    const auto calibrationFile = CDBInterface::instance()->getUrl("TPOT_Pedestal");
    tpotunpacker->set_calibration_file(calibrationFile);
    tpotunpacker->set_sample_max(1024);
    // tpotunpacker->set_hot_channel_map_file("Calibrations/TPOT_HotChannels-00041989-0000.root" );
    se->registerSubsystem(tpotunpacker);
  }

  MvtxClusterizer* mvtxclusterizer = new MvtxClusterizer("MvtxClusterizer");
  mvtxclusterizer->Verbosity(0);
  //se->registerSubsystem(mvtxclusterizer);

  Intt_Clustering();
  
  Tpc_LaserEventIdentifying();

  auto tpcclusterizer = new TpcClusterizer;
  tpcclusterizer->Verbosity(0);
  tpcclusterizer->set_do_hit_association(G4TPC::DO_HIT_ASSOCIATION);
  tpcclusterizer->set_rawdata_reco();
  tpcclusterizer->set_sampa_tbias(0); 
  tpcclusterizer->set_reject_event(G4TPC::REJECT_LASER_EVENTS);
  //se->registerSubsystem(tpcclusterizer);

  Micromegas_Clustering();

 
  auto silicon_Seeding = new PHActsSiliconSeeding;
  silicon_Seeding->Verbosity(0);
  // these get us to about 83% INTT > 1
  silicon_Seeding->setinttRPhiSearchWindow(0.4);
  silicon_Seeding->setinttZSearchWindow(2.0);
  silicon_Seeding->setStrobeRange(-5,5);
  silicon_Seeding->seedAnalysis(false);
  se->registerSubsystem(silicon_Seeding);

  auto merger = new PHSiliconSeedMerger;
  merger->Verbosity(0);
  se->registerSubsystem(merger);


  /*
   * Tpc Seeding
   */
  auto seeder = new PHCASeeding("PHCASeeding");
 if (ConstField)
  {
    seeder->useConstBField(true);
    seeder->constBField(fieldstrength);
  }
  else
  {
    seeder->set_field_dir(-1 * G4MAGNET::magfield_rescale);
    seeder->useConstBField(false);
    seeder->magFieldFile(G4MAGNET::magfield_tracking);  // to get charge sign right
  }
  seeder->Verbosity(0);
  seeder->SetLayerRange(7, 55);
  seeder->SetSearchWindow(2.,0.05); // z-width and phi-width, default in macro at 1.5 and 0.05
  seeder->SetClusAdd_delta_window(3.0,0.06); //  (0.5, 0.005) are default; sdzdr_cutoff, d2/dr2(phi)_cutoff
  //seeder->SetNClustersPerSeedRange(4,60); // default is 6, 6
  seeder->SetMinHitsPerCluster(0);
  seeder->SetMinClustersPerTrack(3);
  seeder->useFixedClusterError(true);
  seeder->set_pp_mode(true);
  se->registerSubsystem(seeder);

  // expand stubs in the TPC using simple kalman filter
  auto cprop = new PHSimpleKFProp("PHSimpleKFProp");
  cprop->set_field_dir(G4MAGNET::magfield_rescale);
  if (ConstField)
  {
    cprop->useConstBField(true);
    cprop->setConstBField(fieldstrength);
  }
  else
  {
    cprop->magFieldFile(G4MAGNET::magfield_tracking);
    cprop->set_field_dir(-1 * G4MAGNET::magfield_rescale);
  }
  cprop->useFixedClusterError(true);
  cprop->set_max_window(5.);
  cprop->Verbosity(0);
  cprop->set_pp_mode(true);
  se->registerSubsystem(cprop);

  // Always apply preliminary distortion corrections to TPC clusters before silicon matching
  // and refit the trackseeds. Replace KFProp fits with the new fit parameters in the TPC seeds.
  auto prelim_distcorr = new PrelimDistortionCorrection;
  prelim_distcorr->set_pp_mode(true);
  prelim_distcorr->Verbosity(0);
  se->registerSubsystem(prelim_distcorr);

  /*
   * Track Matching between silicon and TPC
   */
  // The normal silicon association methods
  // Match the TPC track stubs from the CA seeder to silicon track stubs from PHSiliconTruthTrackSeeding
  auto silicon_match = new PHSiliconTpcTrackMatching;
  silicon_match->Verbosity(0);
  silicon_match->set_use_legacy_windowing(false);
  silicon_match->set_pp_mode(TRACKING::pp_mode);
  se->registerSubsystem(silicon_match);

  // Match TPC track stubs from CA seeder to clusters in the micromegas layers
  auto mm_match = new PHMicromegasTpcTrackMatching;
  mm_match->Verbosity(0);
  mm_match->set_rphi_search_window_lyr1(3.);
  mm_match->set_rphi_search_window_lyr2(15.0);
  mm_match->set_z_search_window_lyr1(30.0);
  mm_match->set_z_search_window_lyr2(3.);

  mm_match->set_min_tpc_layer(38);             // layer in TPC to start projection fit
  mm_match->set_test_windows_printout(false);  // used for tuning search windows only
  se->registerSubsystem(mm_match);

  /*
   * End Track Seeding
   */


  /*
   * Either converts seeds to tracks with a straight line/helix fit
   * or run the full Acts track kalman filter fit
   */
  if (G4TRACKING::convert_seeds_to_svtxtracks)
  {
    auto converter = new TrackSeedTrackMapConverter;
    // Default set to full SvtxTrackSeeds. Can be set to
    // SiliconTrackSeedContainer or TpcTrackSeedContainer
    converter->setTrackSeedName("SvtxTrackSeedContainer");
    converter->setFieldMap(G4MAGNET::magfield_tracking);
    converter->Verbosity(0);
    se->registerSubsystem(converter);
  }
  else
  {
    auto deltazcorr = new PHTpcDeltaZCorrection;
    deltazcorr->Verbosity(0);
    se->registerSubsystem(deltazcorr);

    // perform final track fit with ACTS
    auto actsFit = new PHActsTrkFitter;
    actsFit->Verbosity(0);
    actsFit->commissioning(G4TRACKING::use_alignment);
    // in calibration mode, fit only Silicons and Micromegas hits
    actsFit->fitSiliconMMs(G4TRACKING::SC_CALIBMODE);
    actsFit->setUseMicromegas(G4TRACKING::SC_USE_MICROMEGAS);
    actsFit->set_pp_mode(TRACKING::pp_mode);
    actsFit->set_use_clustermover(true);  // default is true for now
    actsFit->useActsEvaluator(false);
    actsFit->useOutlierFinder(false);
    actsFit->setFieldMap(G4MAGNET::magfield_tracking);
    se->registerSubsystem(actsFit);

    auto cleaner = new PHTrackCleaner();
    cleaner->Verbosity(0);
    cleaner->set_pp_mode(TRACKING::pp_mode);
    //se->registerSubsystem(cleaner);
  }

 

  //auto finder = new PHSimpleVertexFinder;
  //finder->Verbosity(0);
  //finder->setDcaCut(0.5);
  //finder->setTrackPtCut(-99999.);
  //finder->setBeamLineCut(1);
  //finder->setTrackQualityCut(1000000000);
  //finder->setNmvtxRequired(3);
  //finder->setOutlierPairCut(0.1);
  //se->registerSubsystem(finder);
  
  //TString residoutfile = outfilename + "_resid.root";
  //std::string residstring(residoutfile.Data());

  auto resid = new TrackResiduals("TrackResiduals");
  resid->Verbosity(0);
  resid->outfileName(residstring);
  resid->alignment(false);
  //resid->clusterTree();
  //resid->failedTree();
  resid->hitTree();
  //resid->noEventTree();
  //resid->convertSeeds(G4TRACKING::convert_seeds_to_svtxtracks);
    se->registerSubsystem(resid);

  // process events
  if( nSkipEvents > 0 ) {
    se->skip(nSkipEvents);
  }

  se->run(nEvents);
  

  // terminate
  se->End();
  se->PrintTimer();

  std::cout << "All done" << std::endl;
  delete se;
  gSystem->Exit(0);
  return 0;
}


