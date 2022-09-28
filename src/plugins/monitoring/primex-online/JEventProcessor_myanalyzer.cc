/**************************************************************************                                                             
* HallD software                                                          * 
* Copyright(C) 2022       GlueX Collaboration                             * 
*                                                                         *                                                                
* Author: The GlueX Collaborations                                        *                                                                
* Contributors: Igal Jaegle                                               *                                                               
*                                                                         *
*                                                                         *   
* This software is provided "as is" without any warranty.                 *
**************************************************************************/

#include "JEventProcessor_myanalyzer.h"

extern "C"{
void InitPlugin(JApplication *app){
	InitJANAPlugin(app);
	app->AddProcessor(new JEventProcessor_myanalyzer());
}
} // "C"


//------------------
// init
//------------------
jerror_t JEventProcessor_myanalyzer::init(void)
{
  combi6 = new Combination (6);
  return NOERROR;
}

//------------------
// brun
//------------------
jerror_t JEventProcessor_myanalyzer::brun(JEventLoop *eventLoop, int32_t runnumber)
{
	
  DGeometry*   dgeom = NULL;
  DApplication* dapp = dynamic_cast< DApplication* >( eventLoop->GetJApplication() );
  if( dapp )   dgeom = dapp->GetDGeometry( runnumber );
  
  if( dgeom ){
    dgeom->GetTargetZ( m_beamZ );
    dgeom->GetFCALPosition( m_fcalX, m_fcalY, m_fcalZ );
    dgeom->GetCCALPosition( m_ccalX, m_ccalY, m_ccalZ );
  } else{
    cerr << "No geometry accessbile to compton_analysis plugin." << endl;
    return RESOURCE_UNAVAILABLE;
  }
  
  jana::JCalibration *jcalib = japp->GetJCalibration(runnumber);
  std::map<string, float> beam_spot;
  jcalib->Get("PHOTON_BEAM/beam_spot", beam_spot);
  m_beamX  =  beam_spot.at("x");
  m_beamY  =  beam_spot.at("y");
  
  return NOERROR;
}

//------------------
// evnt
//------------------
jerror_t JEventProcessor_myanalyzer::evnt(JEventLoop *eventLoop, uint64_t eventnumber)
{



  Bool_t good_eta_to_2g = false;
  Bool_t good_eta_to_3pi0 = false;
  Bool_t good_eta_to_2g2pi = false;
  
  TLorentzVector TargetP4(0, 0, 0, massHe4);
  vector<const DMCThrown*> locDMCThrown;	
  eventLoop->Get(locDMCThrown);  
  vector<const DL1Trigger *> locL1Triggers;
  eventLoop->Get(locL1Triggers);
  vector< const DFCALGeometry* > fcalGeomVec;
  eventLoop->Get( fcalGeomVec );

  
  //-----   Data Objects   -----//
  vector< const DBeamPhoton *> locBeamPhotons;
  vector<const DVertex *> locVerteces;
  vector<const DNeutralParticle *> locNeutralParticles;
  vector<const DTrackTimeBased *> locTrackTimeBased;
  vector<const DChargedTrack *> locChargedTracks;
  vector<const DFCALHit *> locFCALHits;
  
  eventLoop->Get(locFCALHits);
  eventLoop->Get(locChargedTracks);
  eventLoop->Get(locBeamPhotons);
  eventLoop->Get(locVerteces);
  eventLoop->Get(locNeutralParticles);
  eventLoop->Get(locTrackTimeBased);
  const DDetectorMatches *detMatches = nullptr;
  eventLoop->GetSingle(detMatches);
    

  
  //https://logbooks.jlab.org/entry/4039521
  /*
    GTP Trigger bits:
    ----------------------
    
    Bit 0: CCAL & FCAL (prescaled)
    Bit 1: FCAL (threshold about 3.5 GeV)
    Bit 2: FCAL (threshold 0.5 GeV)
    Bit 3: PS
    Bit 10: CCAL
  */
  
  //japp->RootFillLock(this);  
  
  //uint32_t locL1Trigger_fp = locL1Triggers.empty() ? 0.0 : locL1Triggers[0]->fp_trig_mask;
  //uint32_t locL1Trigger = locL1Triggers.empty() ? 0.0 : locL1Triggers[0]->trig_mask;
  
  int trig_bit[33];
  if (locL1Triggers.size() > 0) {
    for (unsigned int bit = 0; bit < 32; bit ++) {
      trig_bit[bit + 1] = (locL1Triggers[0]->trig_mask & (1 << bit)) ? 1 : 0;
      if(trig_bit[bit + 1] == 1) //htrig_bit->Fill(Float_t(bit+1));
	Fill1DHistogram("primex-online","","trig_bit", Float_t(bit+1), ";Trigger bit #;Count [a.u.]", 100, 0., 100.);
      
    }
  }
  
  //-----   Check Trigger   -----//
  uint32_t trigmask;
  uint32_t fp_trigmask;
  if (locDMCThrown.size() == 0) {
    const DL1Trigger *trig = NULL;
    try {
      eventLoop->GetSingle(trig);
    } catch (...) {
    }
    if (trig == NULL) { return NOERROR; }
        
    trigmask = trig->trig_mask;	
    fp_trigmask = trig->fp_trig_mask;
    for (int ibit = 0; ibit < 33; ibit++) {
      if(trigmask & (1 << ibit)) Fill1DHistogram("primex-online","","trig1", ibit, ";GTP trigger bit;Events [a.u.]", 33, -0.5, 32.5);  //hTrig->Fill(ibit);
      if(fp_trigmask & (1 << ibit)) Fill1DHistogram("primex-online","","fptrig1", ibit, ";FP trigger bit;Events [a.u.]", 33, -0.5, 32.5);  //hTrig->Fill(ibit);
    }
    if( trigmask==8 ) return NOERROR;
    if( fp_trigmask ) return NOERROR;
  } else {
    trigmask = 1;
    fp_trigmask = 1;
  }
  Fill1DHistogram("primex-online","","trig2", trigmask, ";GTP trigger bit;Events [a.u.]", 33, -0.5, 32.5);  //hTrig->Fill(ibit);
  Fill1DHistogram("primex-online","","fptrig2", fp_trigmask, ";FP trigger bit;Events [a.u.]", 33, -0.5, 32.5);  //hTrig->Fill(ibit);
  
  //-----   RF Bunch   -----//
  
  const DEventRFBunch *locRFBunch = NULL;
  try { 
    eventLoop->GetSingle( locRFBunch, "CalorimeterOnly" );
  } catch (...) { return NOERROR; }
  double locRFTime = locRFBunch->dTime;
  if( locRFBunch->dNumParticleVotes < 2 ) return NOERROR;
  
  
  //-----   Geometry   -----//
    
  if( fcalGeomVec.size() != 1 ) {
    cerr << "No FCAL geometry accessbile." << endl;
    return RESOURCE_UNAVAILABLE;
  }
  
  const DFCALGeometry *fcalGeom = fcalGeomVec[0];
  
  //Retrieve vertex info
  DVector3 vertex;
  vertex.SetXYZ( m_beamX, m_beamY, m_beamZ );
  
  double kinfitVertexX = m_beamX;
  double kinfitVertexY = m_beamY;
  double kinfitVertexZ = m_beamZ;
  double kinfitR = 0;
  
  for (unsigned int i = 0 ; i < locVerteces.size(); i++) {
    kinfitVertexX = locVerteces[i]->dSpacetimeVertex.X();
    kinfitVertexY = locVerteces[i]->dSpacetimeVertex.Y();
    kinfitVertexZ = locVerteces[i]->dSpacetimeVertex.Z();
    kinfitR = sqrt(pow(kinfitVertexX, 2) + pow(kinfitVertexY, 2));
    Fill1DHistogram("primex-online","","h_z", kinfitVertexZ, ";z [cm];Count [a.u.]", 1000, -1000., 1000.);
    Fill1DHistogram("primex-online","","h_r", kinfitR, ";z [cm];Count [a.u.]", 1000, 0., 1000.);
  }

  Bool_t target_vtx_z = (50.0 < kinfitVertexZ && kinfitVertexZ < 80);
  Bool_t target_vtx_r = sqrt(pow(kinfitVertexX, 2) + pow(kinfitVertexY, 2)) < 1;

  //For MC simu.
  double mc_eb = 0;
  if (locDMCThrown.size() > 0) {
    const DMCReaction* locMCReactions = NULL;
    eventLoop->GetSingle(locMCReactions);
    mc_eb = locMCReactions->beam.energy();
    Fill1DHistogram("primex-online","","mc_eb", mc_eb, ";E_{#gamma} [GeV]", 500, 3.0, 12.0);  
    for (unsigned int bit = 0; bit < 32; bit ++) {
      trig_bit[bit + 1] = 1;
    }
  }

  //Compute the FCAL energy sum for cell with an energy deposited above 150MeV 
  double BCAL_Esum = 0;
  double FCAL_Esum = 0;
  double CCAL_Esum = 0;
  int n_locBCALShowers = 0;
  int n_locFCALShowers = 0;
  int n_locFCALTOFShowers = 0;
  int n_locCCALShowers = 0;
  double FCAL_trg_Esum = 0;

  for (vector<const DFCALHit*>::const_iterator hit  = locFCALHits.begin(); hit != locFCALHits.end(); hit++ ) {
    if ((**hit).E > 0.150)
      FCAL_trg_Esum += (**hit).E;
  }
  if (FCAL_trg_Esum > 0) {
    Fill1DHistogram("primex-online","","FCAL_trg_Esum", FCAL_trg_Esum, ";E [GeV];Count [a.u.]", 12000, 0., 12.);
    if (trig_bit[1] == 1)
      Fill1DHistogram("primex-online","","FCAL_trg1_Esum", FCAL_trg_Esum, ";E [GeV];Count [a.u.]", 12000, 0., 12.);
    if (trig_bit[2] == 1)
      Fill1DHistogram("primex-online","","FCAL_trg2_Esum", FCAL_trg_Esum, ";E [GeV];Count [a.u.]", 12000, 0., 12.);
    if (trig_bit[3] == 1)
      Fill1DHistogram("primex-online","","FCAL_trg3_Esum", FCAL_trg_Esum, ";E [GeV];Count [a.u.]", 12000, 0., 12.);
  }

  //Loop over neutral particle list, showers matching a track a removed
  vector<const DNeutralParticleHypothesis*> PhotonsList;
  Bool_t InnerFCAL_ring = false;

  for (unsigned int i = 0; i < locNeutralParticles.size(); i++){
    
    const DNeutralParticleHypothesis * photon = locNeutralParticles[i]->Get_Hypothesis(Gamma);
    if (photon == nullptr) continue;
    
    const DNeutralShower * shower = photon->Get_NeutralShower();
    
    if (shower->dDetectorSystem == SYS_BCAL) { 
      
      DBCALShower *bcal_shower = (DBCALShower *) shower->dBCALFCALShower;
      double e = bcal_shower->E;
      double x = bcal_shower->x - vertex.X();
      double y = bcal_shower->y - vertex.Y();
      double z = bcal_shower->z - vertex.Z();
      DVector3 position(x, y, z);
      double r = position.Mag();
      double t = bcal_shower->t - (r / TMath::C() * 1e7);
      double px = e * sin(position.Theta()) * cos(position.Phi());
      double py = e * sin(position.Theta()) * sin(position.Phi());
      double pz = e * cos(position.Theta());
      TLorentzVector PhotonVec(px, py, pz, e);
      double diff_t = t - locRFTime;
      Fill2DHistogram("primex-online", "", "bcal_edep", diff_t, e, ";t_{#gamma}^{bcal} - t_{RF} [ns];E_{shower} [GeV];count [a.u.]", 2000, -40., 40., 2000, 0., 2.);
      if (fabs(diff_t) < BCAL_RF_CUT) {
	if (e > 0.25) PhotonsList.push_back(photon);
	n_locBCALShowers ++;
	BCAL_Esum += e;
      }
    }
    
    if (shower->dDetectorSystem == SYS_FCAL) { 
      
      DFCALShower *fcal_shower = (DFCALShower *) shower->dBCALFCALShower;
      double e = fcal_shower->getEnergy();
      DVector3 position = fcal_shower->getPosition_log() - vertex;
      //DVector3 position = fcal_shower->getPosition() - vertex;
      double r = position.Mag();
      double t = fcal_shower->getTime() - (r / TMath::C() * 1e7);
      double px = e * sin(position.Theta()) * cos(position.Phi());
      double py = e * sin(position.Theta()) * sin(position.Phi());
      double pz = e * cos(position.Theta());
      TLorentzVector PhotonVec(px, py, pz, e);
      double diff_t = t - locRFTime;
      double face_x = vertex.X() + (position.X() * (m_fcalZ - vertex.Z())/position.Z());
      double face_y = vertex.Y() + (position.Y() * (m_fcalZ - vertex.Z())/position.Z());
      int row = fcalGeom->row(static_cast<float>(face_y));
      int col = fcalGeom->column(static_cast<float>(face_x));
      //int layer = fcalLayer(row, col);
      const DFCALCluster *fcalCluster;
      fcal_shower->GetSingle(fcalCluster);
      //int channel = fcalCluster->getChannelEmax();
      //double emax = fcalCluster->getEmax();
      float TOF_FCAL_x_min = shower->dTOF_FCAL_x_min;
      float TOF_FCAL_y_min = shower->dTOF_FCAL_y_min;

      //bool in_insert = fcalGeom->inInsert(channel);
      double radius = sqrt(pow(position.X(), 2) + pow(position.Y(), 2));
      Fill1DHistogram("primex-online","","fcal_rf", diff_t, ";t_{ccal} - t_{RF} [ns];Count [a.u.]", 1000, -50., 50.);
      Fill2DHistogram("primex-online","","fcal_xy", face_x, face_y, ";row;column #;Counts", 500, -125, 125, 500, -125, 125);
      Fill2DHistogram("primex-online","","fcal_rc", row, col, ";row;column #;Counts", 59, 0, 59, 59, 0, 59);
      Fill2DHistogram("primex-online","","fcal_tof_dx", TOF_FCAL_x_min, TOF_FCAL_y_min, ";#Deltax;#Deltay;Counts", 200, 0, 50, 200, 0, 50);

      if (fabs(diff_t) < FCAL_RF_CUT) {
	if (radius < 10) InnerFCAL_ring = true;
	if (e > 0.25) PhotonsList.push_back(photon);
	n_locFCALShowers ++;
	if (TOF_FCAL_x_min > 6 && TOF_FCAL_y_min > 6) {
	  n_locFCALTOFShowers ++;
	}
	FCAL_Esum += e;
      }
    }

    
    if (shower->dDetectorSystem == SYS_CCAL) {
      
      DCCALShower *ccal_shower = (DCCALShower *) shower->dBCALFCALShower;
      double e = ccal_shower->E;
      double x = ccal_shower->x1 - vertex.X();
      double y = ccal_shower->y1 - vertex.Y();
      double z = ccal_shower->z - vertex.Z();
      DVector3 position(x, y, z);
      double r = position.Mag();
      double t = ccal_shower->time - (r / TMath::C() * 1e7);
      double px = e * sin(position.Theta()) * cos(position.Phi());
      double py = e * sin(position.Theta()) * sin(position.Phi());
      double pz = e * cos(position.Theta());
      TLorentzVector PhotonVec(px, py, pz, e);
      double diff_t = t - locRFTime;
      //double phi_ccal = PhotonVec.Phi();
      //double theta_ccal = PhotonVec.Theta();
      //double delta_phi_min = 1000.;
      Fill1DHistogram("primex-online","","ccal_rf", diff_t, ";t_{ccal} - t_{RF} [ns];Count [a.u.]", 1000, -50., 50.);
      if (fabs(diff_t) < CCAL_RF_CUT) {
	//ShowersList.push_back(PhotonVec);
	n_locCCALShowers++;
	CCAL_Esum += e;
      }
    }
  }

  //Checking showers energy sum
  if (n_locBCALShowers > 0)
    Fill1DHistogram("primex-online","","Esum_bcal", BCAL_Esum, ";E_{dep} [GeV];Count [a.u.]", 12000, 0., 12.);
  if (n_locFCALShowers > 0) {
    Fill1DHistogram("primex-online","","Esum_fcal", FCAL_Esum, ";E_{dep} [GeV];Count [a.u.]", 12000, 0., 12.);
    if (trig_bit[1] == 1) {
      Fill1DHistogram("primex-online","","Esum_fcal_trg1", FCAL_Esum, ";E_{dep} [GeV];Count [a.u.]", 12000, 0., 12.);
    }
    if (trig_bit[2] == 1) {
      Fill1DHistogram("primex-online","","Esum_fcal_trg2", FCAL_Esum, ";E_{dep} [GeV];Count [a.u.]", 12000, 0., 12.);
    }
    if (trig_bit[3] == 1) {
      Fill1DHistogram("primex-online","","Esum_fcal_trg3", FCAL_Esum, ";E_{dep} [GeV];Count [a.u.]", 12000, 0., 12.);
    }
  }
  if (n_locCCALShowers > 0) {
    Fill1DHistogram("primex-online","","Esum_ccal", CCAL_Esum, ";E_{dep} [GeV];Count [a.u.]", 12000, 0., 12.);
    if (trig_bit[1] == 1) {
      Fill1DHistogram("primex-online","","Esum_ccal_trg1", CCAL_Esum, ";E_{dep} [GeV];Count [a.u.]", 12000, 0., 12.);
    }
    if (trig_bit[2] == 1) {
      Fill1DHistogram("primex-online","","Esum_ccal_trg2", CCAL_Esum, ";E_{dep} [GeV];Count [a.u.]", 12000, 0., 12.);
    }
    if (trig_bit[3] == 1) {
      Fill1DHistogram("primex-online","","Esum_ccal_trg3", CCAL_Esum, ";E_{dep} [GeV];Count [a.u.]", 12000, 0., 12.);
    }
  }
  
  //Retrieve tracks info and assign it a pi hypo
  vector <const DTrackTimeBased *> pimsList;
  vector <const DTrackTimeBased *> pipsList;
  FillParticleVectors(locChargedTracks, pimsList, pipsList);
  DLorentzVector EtaP4;
  DLorentzVector gP4[6];
  DLorentzVector pipP4;
  DLorentzVector pimP4;
  
  //Looking at event with 2 unmatched showers
  if (PhotonsList.size() == 2) {
    
    gP4[0] = PhotonsList[0]->lorentzMomentum();
    gP4[1] = PhotonsList[1]->lorentzMomentum();
    DLorentzVector ggP4 = gP4[0] + gP4[1];
    good_eta_to_2g = true;
    EtaP4 = ggP4;
    Fill1DHistogram("primex-online","","mgg_all", ggP4.M(), ";m_{#gamma#gamma} [GeV/#it{c}^{2}];Count [a.u.]", 2000, 0., 2.);
    
    if (n_locFCALShowers == 2)
      Fill1DHistogram("primex-online","","mgg_fcal", ggP4.M(), ";m_{#gamma#gamma} [GeV/#it{c}^{2}];Count [a.u.]", 2000, 0., 2.);
    
    if (n_locFCALTOFShowers == 2)
      Fill1DHistogram("primex-online","","mgg_fcaltof", ggP4.M(), ";m_{#gamma#gamma} [GeV/#it{c}^{2}];Count [a.u.]", 2000, 0., 2.);
    
    Bool_t GoodPi0 = (0.11 < ggP4.M() && ggP4.M() < 0.16);

    //Looking at event with 2 unmatched showers and a pair of opposite charged tracks
    if (pimsList.size() == 1 && pipsList.size() == 1) {
      
      pipP4 = pimsList[0]->lorentzMomentum();
      pimP4 = pipsList[0]->lorentzMomentum();
      DLorentzVector ggpipiP4 = ggP4 + pipP4 + pimP4;
      
      Fill1DHistogram("primex-online","","mgg", ggP4.M(), ";m_{#gamma#gamma} [GeV/#it{c}^{2}];Count [a.u.]", 2000, 0., 2.);
      Fill1DHistogram("primex-online","","mggpipi", ggpipiP4.M(), ";m_{#gamma#gamma#pi^{+}#pi^{-}} [GeV/#it{c}^{2}];Count [a.u.]", 3000, 0., 3.);
      
      if (GoodPi0) {
	good_eta_to_2g2pi = true;
	EtaP4 = ggpipiP4;
	Fill1DHistogram("primex-online","","mpipipi", ggpipiP4.M(), ";m_{#gamma#gamma#pi^{+}#pi^{-}} [GeV/#it{c}^{2}];Count [a.u.]", 3000, 0., 3.);
	if (trig_bit[2] == 1) {
	  Fill1DHistogram("primex-online","","mpipipi_trg2", ggpipiP4.M(), ";m_{#gamma#gamma#pi^{+}#pi^{-}} [GeV/#it{c}^{2}];Count [a.u.]", 3000, 0., 3.);
	}
	if (trig_bit[3] == 1) {
	  Fill1DHistogram("primex-online","","mpipipi_trg3", ggpipiP4.M(), ";m_{#gamma#gamma#pi^{+}#pi^{-}} [GeV/#it{c}^{2}];Count [a.u.]", 3000, 0., 3.);
	}
      }
    }
  }

  //Looking at event with 6 unmatched showers
  if (PhotonsList.size() == 6) {
    
    Double_t bestRawChi2Eta = 1.0e30;
    Double_t bestRawChi2EtaPrim = 1.0e30;
    vector <int> Index6gList;Index6gList.clear();
    vector <int> Indexprim6gList;Indexprim6gList.clear();
    
    //Find the best combo
    Combined6g(PhotonsList,
	       bestRawChi2Eta,
	       bestRawChi2EtaPrim,
	       Index6gList,
	       Indexprim6gList);

    //Select the best combo
    if (Index6gList.size() == 6) {
      for (int i = 0; i < 6; i ++) {
	gP4[i] = PhotonsList[Index6gList[i]]->lorentzMomentum();
      }
      
      double m0 = (PhotonsList[Index6gList[0]]->lorentzMomentum() + PhotonsList[Index6gList[1]]->lorentzMomentum()).M();
      double m1 = (PhotonsList[Index6gList[2]]->lorentzMomentum() + PhotonsList[Index6gList[3]]->lorentzMomentum()).M();
      double m2 = (PhotonsList[Index6gList[4]]->lorentzMomentum() + PhotonsList[Index6gList[5]]->lorentzMomentum()).M();
      
      DLorentzVector ggggggP4 = 
	PhotonsList[Index6gList[0]]->lorentzMomentum() + PhotonsList[Index6gList[1]]->lorentzMomentum() + 
	PhotonsList[Index6gList[2]]->lorentzMomentum() + PhotonsList[Index6gList[3]]->lorentzMomentum() + 
	PhotonsList[Index6gList[4]]->lorentzMomentum() + PhotonsList[Index6gList[5]]->lorentzMomentum();
      
      DLorentzVector pi0pi0pi0P4 = 
	m_pi0 / m0 * (PhotonsList[Index6gList[0]]->lorentzMomentum() + PhotonsList[Index6gList[1]]->lorentzMomentum()) +
	m_pi0 / m1 * (PhotonsList[Index6gList[2]]->lorentzMomentum() + PhotonsList[Index6gList[3]]->lorentzMomentum()) +
	m_pi0 / m2 * (PhotonsList[Index6gList[4]]->lorentzMomentum() + PhotonsList[Index6gList[5]]->lorentzMomentum());
      
      int pi0_nb = 0;
      for (int i = 0; i < 3; i ++) {
	DLorentzVector g1P4 = PhotonsList[Index6gList[2 * i]]->lorentzMomentum();
	DLorentzVector g2P4 = PhotonsList[Index6gList[2 * i + 1]]->lorentzMomentum();
	DLorentzVector ggP4 = g1P4 + g2P4;
	Fill1DHistogram("primex-online","","mgg_all15", ggP4.M(), ";m_{#gamma#gamma} [GeV/#it{c}^{2}];Count [a.u.]", 2000, 0., 2.);
	Bool_t GoodPi0 = (0.11 < ggP4.M() && ggP4.M() < 0.16);
	if (GoodPi0) pi0_nb ++;
      }
      
      Fill1DHistogram("primex-online","","m6g_all", ggggggP4.M(), ";m_{#gamma#gamma#gamma#gamma#gamma#gamma} [GeV/#it{c}^{2}];Count [a.u.]", 3000, 0., 3.);
      Fill1DHistogram("primex-online","","m3pi0_all", pi0pi0pi0P4.M(), ";m_{#pi^{0}#pi^{0}#pi^{0}} [GeV/#it{c}^{2}];Count [a.u.]", 3000, 0., 3.);
      if (pi0_nb == 3) {
	good_eta_to_3pi0 = true;
	EtaP4 = pi0pi0pi0P4;
	Fill1DHistogram("primex-online","","m6g_pi0", ggggggP4.M(), ";m_{#gamma#gamma#gamma#gamma#gamma#gamma} [GeV/#it{c}^{2}];Count [a.u.]", 3000, 0., 3.);
	Fill1DHistogram("primex-online","","m3pi0_pi0", pi0pi0pi0P4.M(), ";m_{#pi^{0}#pi^{0}#pi^{0}} [GeV/#it{c}^{2}];Count [a.u.]", 3000, 0., 3.);
	if (trig_bit[2] == 1) {
	  Fill1DHistogram("primex-online","","m6g_trg2", ggggggP4.M(), ";m_{#gamma#gamma#gamma#gamma#gamma#gamma} [GeV/#it{c}^{2}];Count [a.u.]", 3000, 0., 3.);
	  Fill1DHistogram("primex-online","","m3pi0_trg2", pi0pi0pi0P4.M(), ";m_{#pi^{0}#pi^{0}#pi^{0}} [GeV/#it{c}^{2}];Count [a.u.]", 3000, 0., 3.);
	}
	if (trig_bit[3] == 1) {
	  Fill1DHistogram("primex-online","","m6g_trg3", ggggggP4.M(), ";m_{#gamma#gamma#gamma#gamma#gamma#gamma} [GeV/#it{c}^{2}];Count [a.u.]", 3000, 0., 3.);
	  Fill1DHistogram("primex-online","","m3pi0_trg3", pi0pi0pi0P4.M(), ";m_{#pi^{0}#pi^{0}#pi^{0}} [GeV/#it{c}^{2}];Count [a.u.]", 3000, 0., 3.);
	}
      }
    }
  }
  
  //Basic selection criteria
  Bool_t Prim2g = good_eta_to_2g * (locChargedTracks.size() == 0 && n_locBCALShowers == 0 && n_locFCALTOFShowers == 2);
  Bool_t Prim3pi0 = good_eta_to_3pi0 * (locChargedTracks.size() == 0);
  Bool_t Prim2g2pi = good_eta_to_2g2pi * target_vtx_z * target_vtx_r * (locChargedTracks.size() == 2);
  
  //Loop over beam-photon if basic selection criteria is fullfilled
  if ((Prim2g || Prim3pi0 || Prim2g2pi) 
      && 
      (trig_bit[2] == 1 || trig_bit[3] == 1)
      &&
      !InnerFCAL_ring) {
    
    if (Prim2g) {
      Fill1DHistogram("primex-online","","m2g_sc", EtaP4.M(), ";m_{#gamma#gamma} [GeV/#it{c}^{2}];Count [a.u.]", 300, 0., 3.);
      if (trig_bit[2] == 1)
	Fill1DHistogram("primex-online","","m2g_sc_trg2", EtaP4.M(), ";m_{#gamma#gamma} [GeV/#it{c}^{2}];Count [a.u.]", 300, 0., 3.);
      if (trig_bit[3] == 1)
	Fill1DHistogram("primex-online","","m2g_sc_trg3", EtaP4.M(), ";m_{#gamma#gamma} [GeV/#it{c}^{2}];Count [a.u.]", 300, 0., 3.);
    }
    if (Prim3pi0) {
      Fill1DHistogram("primex-online","","m6g_sc", EtaP4.M(), ";m_{#gamma#gamma#gamma#gamma#gamma#gamma} [GeV/#it{c}^{2}];Count [a.u.]", 300, 0., 3.);
      if (trig_bit[2] == 1)
	Fill1DHistogram("primex-online","","m6g_sc_trg2", EtaP4.M(), ";m_{#gamma#gamma} [GeV/#it{c}^{2}];Count [a.u.]", 300, 0., 3.);
      if (trig_bit[3] == 1)
	Fill1DHistogram("primex-online","","m6g_sc_trg3", EtaP4.M(), ";m_{#gamma#gamma} [GeV/#it{c}^{2}];Count [a.u.]", 300, 0., 3.);
    }
    if (Prim2g2pi) {
      Fill1DHistogram("primex-online","","m2g2pi_sc", EtaP4.M(), ";m_{#gamma#gamma#pi^{+}#pi^{-}} [GeV/#it{c}^{2}];Count [a.u.]", 300, 0., 3.);
      if (trig_bit[2] == 1)
	Fill1DHistogram("primex-online","","m2g2pi_sc_trg2", EtaP4.M(), ";m_{#gamma#gamma} [GeV/#it{c}^{2}];Count [a.u.]", 300, 0., 3.);
      if (trig_bit[3] == 1)
	Fill1DHistogram("primex-online","","m2g2pi_sc_trg3", EtaP4.M(), ";m_{#gamma#gamma} [GeV/#it{c}^{2}];Count [a.u.]", 300, 0., 3.);
    }

    if (FCAL_trg_Esum > 0) {
      if (Prim2g) {
	Fill1DHistogram("primex-online","","FCAL2g_trg_Esum_sc", FCAL_trg_Esum, ";E [GeV];Count [a.u.]", 120, 0., 12.);
	if (trig_bit[1] == 1)
	  Fill1DHistogram("primex-online","","FCAL2g_trg1_Esum_sc", FCAL_trg_Esum, ";E [GeV];Count [a.u.]", 120, 0., 12.);
	if (trig_bit[2] == 1)
	  Fill1DHistogram("primex-online","","FCAL2g_trg2_Esum_sc", FCAL_trg_Esum, ";E [GeV];Count [a.u.]", 120, 0., 12.);
	if (trig_bit[3] == 1)
	  Fill1DHistogram("primex-online","","FCAL2g_trg3_Esum_sc", FCAL_trg_Esum, ";E [GeV];Count [a.u.]", 120, 0., 12.);
	if (0.5 < EtaP4.M() && EtaP4.M() < 0.6) {
	  Fill1DHistogram("primex-online","","FCALetato2g_trg_Esum_sc", FCAL_trg_Esum, ";E [GeV];Count [a.u.]", 120, 0., 12.);
	  if (trig_bit[1] == 1)
	    Fill1DHistogram("primex-online","","FCALetato2g_trg1_Esum_sc", FCAL_trg_Esum, ";E [GeV];Count [a.u.]", 120, 0., 12.);
	  if (trig_bit[2] == 1)
	    Fill1DHistogram("primex-online","","FCALetato2g_trg2_Esum_sc", FCAL_trg_Esum, ";E [GeV];Count [a.u.]", 120, 0., 12.);
	  if (trig_bit[3] == 1)
	    Fill1DHistogram("primex-online","","FCALetato2g_trg3_Esum_sc", FCAL_trg_Esum, ";E [GeV];Count [a.u.]", 120, 0., 12.);
	}
      }
      if (Prim3pi0) {
	Fill1DHistogram("primex-online","","FCAL3pi0_trg_Esum_sc", FCAL_trg_Esum, ";E [GeV];Count [a.u.]", 120, 0., 12.);
	if (trig_bit[1] == 1)
	  Fill1DHistogram("primex-online","","FCAL3pi0_trg1_Esum_sc", FCAL_trg_Esum, ";E [GeV];Count [a.u.]", 120, 0., 12.);
	if (trig_bit[2] == 1)
	  Fill1DHistogram("primex-online","","FCAL3pi0_trg2_Esum_sc", FCAL_trg_Esum, ";E [GeV];Count [a.u.]", 120, 0., 12.);
	if (trig_bit[3] == 1)
	  Fill1DHistogram("primex-online","","FCAL3pi0_trg3_Esum_sc", FCAL_trg_Esum, ";E [GeV];Count [a.u.]", 120, 0., 12.);
	if (0.5 < EtaP4.M() && EtaP4.M() < 0.6) {
	  Fill1DHistogram("primex-online","","FCALetato3pi0_trg_Esum_sc", FCAL_trg_Esum, ";E [GeV];Count [a.u.]", 120, 0., 12.);
	  if (trig_bit[1] == 1)
	    Fill1DHistogram("primex-online","","FCALetato3pi0_trg1_Esum_sc", FCAL_trg_Esum, ";E [GeV];Count [a.u.]", 120, 0., 12.);
	  if (trig_bit[2] == 1)
	    Fill1DHistogram("primex-online","","FCALetato3pi0_trg2_Esum_sc", FCAL_trg_Esum, ";E [GeV];Count [a.u.]", 120, 0., 12.);
	  if (trig_bit[3] == 1)
	    Fill1DHistogram("primex-online","","FCALetato3pi0_trg3_Esum_sc", FCAL_trg_Esum, ";E [GeV];Count [a.u.]", 120, 0., 12.);
	}
      }
      if (Prim2g2pi) {
	Fill1DHistogram("primex-online","","FCAL2g2pi_trg_Esum_sc", FCAL_trg_Esum, ";E [GeV];Count [a.u.]", 120, 0., 12.);
	if (trig_bit[1] == 1)
	  Fill1DHistogram("primex-online","","FCAL2g2pi_trg1_Esum_sc", FCAL_trg_Esum, ";E [GeV];Count [a.u.]", 120, 0., 12.);
	if (trig_bit[2] == 1)
	  Fill1DHistogram("primex-online","","FCAL2g2pi_trg2_Esum_sc", FCAL_trg_Esum, ";E [GeV];Count [a.u.]", 120, 0., 12.);
	if (trig_bit[3] == 1)
	  Fill1DHistogram("primex-online","","FCAL2g2pi_trg3_Esum_sc", FCAL_trg_Esum, ";E [GeV];Count [a.u.]", 120, 0., 12.);
	if (0.5 < EtaP4.M() && EtaP4.M() < 0.6) {
	  Fill1DHistogram("primex-online","","FCALetato2g2pi_trg_Esum_sc", FCAL_trg_Esum, ";E [GeV];Count [a.u.]", 120, 0., 12.);
	  if (trig_bit[1] == 1)
	    Fill1DHistogram("primex-online","","FCALetato2g2pi_trg1_Esum_sc", FCAL_trg_Esum, ";E [GeV];Count [a.u.]", 120, 0., 12.);
	  if (trig_bit[2] == 1)
	    Fill1DHistogram("primex-online","","FCALetato2g2pi_trg2_Esum_sc", FCAL_trg_Esum, ";E [GeV];Count [a.u.]", 120, 0., 12.);
	  if (trig_bit[3] == 1)
	    Fill1DHistogram("primex-online","","FCALetato2g2pi_trg3_Esum_sc", FCAL_trg_Esum, ";E [GeV];Count [a.u.]", 120, 0., 12.);
	}
      }
    }
    
    //Loop over beam-photon
    for (unsigned int i = 0; i < locBeamPhotons.size(); i ++) {
      
      const DBeamPhoton * ebeam = locBeamPhotons[i]; 
      double eb = ebeam->lorentzMomentum().E();	
      DetectorSystem_t sys = ebeam->dSystem;
      int counter = ebeam->dCounter;
      //int tagm_ctr = -1;
      //int tagh_ctr = -1;
      if (sys == SYS_TAGH) {
	Fill2DHistogram("primex-online","","tagh", eb, counter, ";E_{#gamma} [GeV];TAGH counter;count [a.u.]", 6000, 5.0, 12.0, 500, -0.5, 499.5);     
	//tagh_ctr = counter;
      } else if (sys == SYS_TAGM) {
	Fill2DHistogram("primex-online","","tagm", eb, counter, ";E_{#gamma} [GeV];TAGM counter;t_{tagger} - t_{RF} [ns];count [a.u.]", 6000, 5.0, 12.0, 500, -0.5, 499.5);
	//tagm_ctr = counter;
      }
      if (eb < 8.0) continue;
      double tb = ebeam->time();
      double zb = ebeam->position().Z();
      TLorentzVector PhotonBeamP4(0, 0, eb, eb);
      TLorentzVector ISP4 = TargetP4 + PhotonBeamP4;
      double locDeltaTRF = tb - (locRFTime + (zb - m_beamZ) / 29.9792458);
      Fill2DHistogram("primex-online","","TaggerTiming_vs_eg", eb, locDeltaTRF, ";E_{#gamma} [GeV];RF-tagger;t_{tagger} - t_{RF} [ns];count [a.u.]", 500, 3.0, 12.0, 2000, -200, 200);
      double weight = 0;
      if (fabs(locDeltaTRF) <= 2.004) {
	weight = 1;
      } else if ( ( -(2.004 + 6.0 * 4.008) <= locDeltaTRF && locDeltaTRF <= -(2.004 + 4.008) ) || 
		  ( (2.004 + 4.008) <= locDeltaTRF && locDeltaTRF <= (2.004 + 6.0 * 4.008) ) ) {
	weight = -0.1;
      } else {
	continue;
      }
      Fill2DHistogram("primex-online","","TaggerTiming_vs_egcut", eb, locDeltaTRF, ";E_{#gamma} [GeV];RF-tagger;t_{tagger} - t_{RF} [ns];count [a.u.]", 500, 3.0, 12.0, 2000, -200, 200);
      
      //double DE = eb - EtaP4.E();
      double M = EtaP4.M();
      //double rM = M * cos(45.0 * TMath::RadToDeg()) - DE * sin(45.0 * TMath::RadToDeg());
      double RatioE = EtaP4.E() / eb;
      double RatiorE = (m_eta / M * EtaP4).E() / eb;
      double RatioM = EtaP4.M() / m_eta; 
      //double RatiorE = RatioM * cos(45.0 * TMath::RadToDeg()) - RatioE * sin(45.0 * TMath::RadToDeg());; 
      double theta_eta = EtaP4.Theta() * TMath::RadToDeg();
      if (Prim2g) {
	Fill2DWeightedHistogram("primex-online","","Primakoff_2g", RatioM, RatioE, weight, 
				";m_{#gamma#gamma} / m _{#eta};(E_{#gamma}^{1} + E_{#gamma}^{2}) / E_{#gamma}^{beam};count [a.u.]", 
				500, 0.7, 2.0, 500, 0.3, 1.4);
	Fill2DWeightedHistogram("primex-online","","Primakoff_2g_r", RatioM, RatiorE, weight, 
				";m_{#gamma#gamma} / m _{#eta};(E_{#gamma}^{1} + E_{#gamma}^{2}) / E_{#gamma}^{beam};count [a.u.]", 
				500, 0.7, 2.0, 500, 0.3, 1.4);
      }
      if (Prim3pi0) {
	Fill2DWeightedHistogram("primex-online","","Primakoff_6g", RatioM, RatioE, weight, 
				";m_{#gamma#gamma#gamma#gamma#gamma#gamma} / m _{#eta};(E_{#gamma}^{1} + E_{#gamma}^{2} + E_{#gamma}^{3} + E_{#gamma}^{4} + E_{#gamma}^{5} + E_{#gamma}^{6}) / E_{#gamma}^{beam};count [a.u.]", 
				500, 0.7, 2.0, 500, 0.3, 1.4);
	Fill2DWeightedHistogram("primex-online","","Primakoff_6g_r", RatioM, RatiorE, weight, 
				";m_{#gamma#gamma#gamma#gamma#gamma#gamma} / m _{#eta};(E_{#gamma}^{1} + E_{#gamma}^{2} + E_{#gamma}^{3} + E_{#gamma}^{4} + E_{#gamma}^{5} + E_{#gamma}^{6}) / E_{#gamma}^{beam};count [a.u.]", 
				500, 0.7, 2.0, 500, 0.3, 1.4);
      }
      if (Prim2g2pi) {
	Fill2DWeightedHistogram("primex-online","","Primakoff_2g2pi", RatioM, RatioE, weight, 
				";m_{#gamma#gamma#pi^{+}#pi^{-}} / m _{#eta};(E_{#gamma}^{1} + E_{#gamma}^{2} + E_{#pi^{+}} + E_{#pi^{-}}) / E_{#gamma}^{beam};count [a.u.]", 
				500, 0.7, 2.0, 500, 0.3, 1.4);
	Fill2DWeightedHistogram("primex-online","","Primakoff_2g2pi_r", RatioM, RatiorE, weight, 
				";m_{#gamma#gamma#pi^{+}#pi^{-}} / m _{#eta};(E_{#gamma}^{1} + E_{#gamma}^{2} + E_{#pi^{+}} + E_{#pi^{-}}) / E_{#gamma}^{beam};count [a.u.]", 
				500, 0.7, 2.0, 500, 0.3, 1.4);
      }
      Bool_t elasticity = (0.9 < RatioE && RatioE < 1.1);
      Bool_t good_eta = (0.9 < RatioM && RatioM < 1.1);
      if (elasticity) {	
	if (Prim2g) {
	  Fill1DWeightedHistogram("primex-online","","m2g_sc_w", EtaP4.M(), weight, ";m_{#gamma#gamma} [GeV/#it{c}^{2}];Count [a.u.]", 3000, 0., 3.);
	  if (trig_bit[2] == 1)
	    Fill1DWeightedHistogram("primex-online","","m2g_sc_w_trg2", EtaP4.M(), weight, ";m_{#gamma#gamma} [GeV/#it{c}^{2}];Count [a.u.]", 3000, 0., 3.);
	  if (trig_bit[3] == 1)
	    Fill1DWeightedHistogram("primex-online","","m2g_sc_w_trg3", EtaP4.M(), weight, ";m_{#gamma#gamma} [GeV/#it{c}^{2}];Count [a.u.]", 3000, 0., 3.);
	}
	if (Prim3pi0) {
	  Fill1DWeightedHistogram("primex-online","","m6g_sc_w", EtaP4.M(), weight, ";m_{#gamma#gamma#gamma#gamma#gamma#gamma} [GeV/#it{c}^{2}];Count [a.u.]", 3000, 0., 3.);
	  if (trig_bit[2] == 1)
	    Fill1DWeightedHistogram("primex-online","","m6g_sc_w_trg2", EtaP4.M(), weight, ";m_{#gamma#gamma} [GeV/#it{c}^{2}];Count [a.u.]", 3000, 0., 3.);
	  if (trig_bit[3] == 1)
	    Fill1DWeightedHistogram("primex-online","","m6g_sc_w_trg3", EtaP4.M(), weight, ";m_{#gamma#gamma} [GeV/#it{c}^{2}];Count [a.u.]", 3000, 0., 3.);
	}
	if (Prim2g2pi) {
	  Fill1DWeightedHistogram("primex-online","","m2g2pi_sc_w", EtaP4.M(), weight, ";m_{#gamma#gamma#pi^{+}#pi^{-}} [GeV/#it{c}^{2}];Count [a.u.]", 3000, 0., 3.);
	  if (trig_bit[2] == 1)
	    Fill1DWeightedHistogram("primex-online","","m2g2pi_sc_w_trg2", EtaP4.M(), weight, ";m_{#gamma#gamma} [GeV/#it{c}^{2}];Count [a.u.]", 3000, 0., 3.);
	  if (trig_bit[3] == 1)
	    Fill1DWeightedHistogram("primex-online","","m2g2pi_sc_w_trg3", EtaP4.M(), weight, ";m_{#gamma#gamma} [GeV/#it{c}^{2}];Count [a.u.]", 3000, 0., 3.);
	}
	
	if (FCAL_trg_Esum > 0) {
	  if (Prim2g) {
	    Fill1DWeightedHistogram("primex-online","","FCAL2g_trg_Esum_sc_w", FCAL_trg_Esum, weight, ";E [GeV];Count [a.u.]", 12000, 0., 12.);
	    if (trig_bit[1] == 1)
	      Fill1DWeightedHistogram("primex-online","","FCAL2g_trg1_Esum_sc_w", FCAL_trg_Esum, weight, ";E [GeV];Count [a.u.]", 12000, 0., 12.);
	    if (trig_bit[2] == 1)
	      Fill1DWeightedHistogram("primex-online","","FCAL2g_trg2_Esum_sc_w", FCAL_trg_Esum, weight, ";E [GeV];Count [a.u.]", 12000, 0., 12.);
	    if (trig_bit[3] == 1)
	      Fill1DWeightedHistogram("primex-online","","FCAL2g_trg3_Esum_sc_w", FCAL_trg_Esum, weight, ";E [GeV];Count [a.u.]", 12000, 0., 12.);
	    if (0.5 < EtaP4.M() && EtaP4.M() < 0.6) {
	      Fill1DWeightedHistogram("primex-online","","FCALetato2g_trg_Esum_sc_w", FCAL_trg_Esum, weight, ";E [GeV];Count [a.u.]", 12000, 0., 12.);
	      if (trig_bit[1] == 1)
		Fill1DWeightedHistogram("primex-online","","FCALetato2g_trg1_Esum_sc_w", FCAL_trg_Esum, weight, ";E [GeV];Count [a.u.]", 12000, 0., 12.);
	      if (trig_bit[2] == 1)
		Fill1DWeightedHistogram("primex-online","","FCALetato2g_trg2_Esum_sc_w", FCAL_trg_Esum, weight, ";E [GeV];Count [a.u.]", 12000, 0., 12.);
	      if (trig_bit[3] == 1)
		Fill1DWeightedHistogram("primex-online","","FCALetato2g_trg3_Esum_sc_w", FCAL_trg_Esum, weight, ";E [GeV];Count [a.u.]", 12000, 0., 12.);
	    }
	  }
	  if (Prim3pi0) {
	    Fill1DWeightedHistogram("primex-online","","FCAL3pi0_trg_Esum_sc_w", FCAL_trg_Esum, weight, ";E [GeV];Count [a.u.]", 12000, 0., 12.);
	    if (trig_bit[1] == 1)
	      Fill1DWeightedHistogram("primex-online","","FCAL3pi0_trg1_Esum_sc_w", FCAL_trg_Esum, weight, ";E [GeV];Count [a.u.]", 12000, 0., 12.);
	    if (trig_bit[2] == 1)
	      Fill1DWeightedHistogram("primex-online","","FCAL3pi0_trg2_Esum_sc_w", FCAL_trg_Esum, weight, ";E [GeV];Count [a.u.]", 12000, 0., 12.);
	    if (trig_bit[3] == 1)
	      Fill1DWeightedHistogram("primex-online","","FCAL3pi0_trg3_Esum_sc_w", FCAL_trg_Esum, weight, ";E [GeV];Count [a.u.]", 12000, 0., 12.);
	    if (0.5 < EtaP4.M() && EtaP4.M() < 0.6) {
	      Fill1DWeightedHistogram("primex-online","","FCALetato3pi0_trg_Esum_sc_w", FCAL_trg_Esum, weight, ";E [GeV];Count [a.u.]", 12000, 0., 12.);
	      if (trig_bit[1] == 1)
		Fill1DWeightedHistogram("primex-online","","FCALetato3pi0_trg1_Esum_sc_w", FCAL_trg_Esum, weight, ";E [GeV];Count [a.u.]", 12000, 0., 12.);
	      if (trig_bit[2] == 1)
		Fill1DWeightedHistogram("primex-online","","FCALetato3pi0_trg2_Esum_sc_w", FCAL_trg_Esum, weight, ";E [GeV];Count [a.u.]", 12000, 0., 12.);
	      if (trig_bit[3] == 1)
		Fill1DWeightedHistogram("primex-online","","FCALetato3pi0_trg3_Esum_sc_w", FCAL_trg_Esum, weight, ";E [GeV];Count [a.u.]", 12000, 0., 12.);
	    }
	  }
	  if (Prim2g2pi) {
	    Fill1DWeightedHistogram("primex-online","","FCAL2g2pi_trg_Esum_sc_w", FCAL_trg_Esum, weight, ";E [GeV];Count [a.u.]", 12000, 0., 12.);
	    if (trig_bit[1] == 1)
	      Fill1DWeightedHistogram("primex-online","","FCAL2g2pi_trg1_Esum_sc_w", FCAL_trg_Esum, weight, ";E [GeV];Count [a.u.]", 12000, 0., 12.);
	    if (trig_bit[2] == 1)
	      Fill1DWeightedHistogram("primex-online","","FCAL2g2pi_trg2_Esum_sc_w", FCAL_trg_Esum, weight, ";E [GeV];Count [a.u.]", 12000, 0., 12.);
	    if (trig_bit[3] == 1)
	      Fill1DWeightedHistogram("primex-online","","FCAL2g2pi_trg3_Esum_sc_w", FCAL_trg_Esum, weight, ";E [GeV];Count [a.u.]", 12000, 0., 12.);
	    if (0.5 < EtaP4.M() && EtaP4.M() < 0.6) {
	      Fill1DWeightedHistogram("primex-online","","FCALetato2g2pi_trg_Esum_sc_w", FCAL_trg_Esum, weight, ";E [GeV];Count [a.u.]", 12000, 0., 12.);
	      if (trig_bit[1] == 1)
		Fill1DWeightedHistogram("primex-online","","FCALetato2g2pi_trg1_Esum_sc_w", FCAL_trg_Esum, weight, ";E [GeV];Count [a.u.]", 12000, 0., 12.);
	      if (trig_bit[2] == 1)
		Fill1DWeightedHistogram("primex-online","","FCALetato2g2pi_trg2_Esum_sc_w", FCAL_trg_Esum, weight, ";E [GeV];Count [a.u.]", 12000, 0., 12.);
	      if (trig_bit[3] == 1)
		Fill1DWeightedHistogram("primex-online","","FCALetato2g2pi_trg3_Esum_sc_w", FCAL_trg_Esum, weight, ";E [GeV];Count [a.u.]", 12000, 0., 12.);
	    }
	  }
	}
      }
      if (good_eta && elasticity) {
	if (Prim2g)
	  Fill1DWeightedHistogram("primex-online","","theta_2g", theta_eta, weight, 
				  ";#theta_{#eta #rightarrow 2#gamma};count [a.u.]", 
				  65, 0.0, 6.5);
	if (Prim3pi0)
	  Fill1DWeightedHistogram("primex-online","","theta_6g", theta_eta, weight, 
				  ";#theta_{#eta #rightarrow 3#pi^{0}};count [a.u.]", 
				  65, 0.0, 6.5);
	
	if (Prim2g2pi)
	  Fill1DWeightedHistogram("primex-online","","theta_2g2pi", theta_eta, weight, 
				  ";#theta_{#eta #rightarrow 2#gamma#pi^{+}#pi^{-}};count [a.u.]", 
				  65, 0.0, 6.5);
      }
    }
  }
  //japp->RootFillUnLock(this);
  //
  return NOERROR;
}

//------------------
// erun
//------------------
jerror_t JEventProcessor_myanalyzer::erun(void)
{
	
	return NOERROR;
}

//------------------
// fini
//------------------
jerror_t JEventProcessor_myanalyzer::fini(void)
{
	
	return NOERROR;
}

jerror_t JEventProcessor_myanalyzer::FillParticleVectors(vector<const DChargedTrack *>&locChargedTracks,
							 vector<const DTrackTimeBased *>&pims,
							 vector<const DTrackTimeBased *>&pips) {
  
  for (unsigned int j = 0; j < locChargedTracks.size(); j ++) {    
  
    const DTrackTimeBased * pion_track = NULL;
    const DChargedTrackHypothesis * hyp = NULL;
    bool got_piplus = false;
    double pion_prob = 0.;
    //double pion_dEdx = 0.;
    //double pion_bg = 0.;
    
    // Look at pions
    hyp = locChargedTracks[j]->Get_Hypothesis(PiPlus);
    if (hyp != NULL) {
      shared_ptr<const DTOFHitMatchParams> tofparms = hyp->Get_TOFHitMatchParams();
      double dt=1000.;
      pion_track = hyp->Get_TrackTimeBased();
      double prob = TMath::Prob(pion_track->chisq, pion_track->Ndof);
      if (prob > 1.e-6) {
	double p = pion_track->momentum().Mag();
	double betagamma = p / pion_track->mass();
	double dEdxSigma = 0.289 / (betagamma * betagamma) - 0.349 / betagamma + 0.321;
	double dEdx = 1e6 * pion_track->dEdx();
	double sum_chi2 = 0;
	int num_dof = 0;
	if (dEdx > 0) {
	  double bg2 = betagamma * betagamma;
	  double dEdxmean=0.061 / (bg2*bg2) + 1.15 / bg2 + 1.21;
	  double ratio = (dEdx - dEdxmean) / dEdxSigma;
	  sum_chi2 += ratio * ratio;
	  num_dof ++;
	}
	if (tofparms != NULL) {
	  dt = tofparms->dHitTime-tofparms->dFlightTime-pion_track->t0();
	  double SigmaT=0.4; //??
	  double ratio= dt / SigmaT;
	  sum_chi2 += ratio * ratio;
	  num_dof ++;
	}
	pion_prob = TMath::Prob(sum_chi2, num_dof);
	//pion_bg = betagamma;
	//pion_dEdx = dEdx;
      }
    }
    if (pion_track != NULL && pion_prob > 1e-6) {
      got_piplus=true;
      pips.push_back(pion_track);
    }
    // Next deal with negative particles
    if (got_piplus == false){
      pion_prob = 0.;
      pion_track = NULL;
      //pion_dEdx = 0.;
      //pion_bg = 0.;
      
      hyp = locChargedTracks[j]->Get_Hypothesis(PiMinus);
      if (hyp != NULL) {
	shared_ptr<const DTOFHitMatchParams> tofparms = hyp->Get_TOFHitMatchParams();
	double dt = 1000.;//,dt_bcal=1000.;
	
	pion_track = hyp->Get_TrackTimeBased();
	double prob = TMath::Prob(pion_track->chisq, pion_track->Ndof);
	if (prob > 1.e-6) {
	  double p = pion_track->momentum().Mag();
	  double betagamma = p / pion_track->mass();
	  double dEdxSigma = 0.289 / (betagamma * betagamma) - 0.349 / betagamma + 0.321;
	  double dEdx = 1e6 * pion_track->dEdx();
	  double sum_chi2 = 0;
	  int num_dof = 0;
	  if (dEdx > 0) {
	    double bg2 = betagamma * betagamma;
	    double dEdxmean = 0.061 / (bg2 * bg2) + 1.15 / bg2 + 1.21;
	    double ratio = (dEdx - dEdxmean) / dEdxSigma;
	    sum_chi2 += ratio * ratio;
	    num_dof ++;
	  }
	  if (tofparms != NULL) {
	    dt = tofparms->dHitTime - tofparms->dFlightTime-pion_track->t0();
	    double SigmaT = 0.4; //??
	    double ratio = dt / SigmaT;
	    sum_chi2 += ratio * ratio;
	    num_dof ++;
	  }
	  
	  pion_prob = TMath::Prob(sum_chi2, num_dof);
	  //pion_bg = betagamma;
	  //pion_dEdx = dEdx;
	}
      }
      
      if (pion_track!=NULL && pion_prob > 1e-6) {
	
	pims.push_back(pion_track);
      }
    }
  }
  
  return NOERROR;
}

void JEventProcessor_myanalyzer::Combined6g(vector<const DNeutralParticleHypothesis *>&EMList,
    Double_t &bestChi0Eta,
    Double_t &bestChi0EtaPrim,
    vector<int>&Index6gList,
    vector<int>&Indexprim6gList)
{	  
  bestChi0EtaPrim   = 1.0e30;
  bestChi0Eta       = 1.0e30;
  if(EMList.size() == 6) {
    for (int i_comb = 0; i_comb < (*combi6).getMaxCombi(); i_comb ++) {
      combi6->getCombi(i_comb);

      double Esum = 0.0;		  
      for (int i = 0; i < 6; i ++) {
        Esum += (EMList[combi6->combi[i]]->lorentzMomentum()).E();
      }

      double Chi2_pi0Mass[3];
      double Chi2_etaMass[3];
      vector<DLorentzVector>GG;GG.clear();
      vector<DLorentzVector>Pi0Cor;Pi0Cor.clear();
      vector<DLorentzVector>EtaCor;EtaCor.clear();

      for (int i = 0; i < 3; i ++) {
        GG.push_back(EMList[combi6->combi[2*i]]->lorentzMomentum() + EMList[combi6->combi[2*i+1]]->lorentzMomentum());
      }
      for (int i = 0; i < 3; i ++) {
        Pi0Cor.push_back( m_pi0 / GG[i].M() * GG[i] );
        EtaCor.push_back( m_eta / GG[i].M() * GG[i] );
        Chi2_pi0Mass[i] = TMath::Power((GG[i].M() - m_pi0) / 8.0e-3,2.0);
        Chi2_etaMass[i] = TMath::Power((GG[i].M() - m_eta) / 22.0e-3,2.0);
      }

      double Chi2_3pi0      = Chi2_pi0Mass[0] + Chi2_pi0Mass[1] + Chi2_pi0Mass[2]; 
      double Chi2_2pi0eta_0 = Chi2_pi0Mass[0] + Chi2_pi0Mass[1] + Chi2_etaMass[2];
      double Chi2_2pi0eta_1 = Chi2_pi0Mass[0] + Chi2_etaMass[1] + Chi2_pi0Mass[2];
      double Chi2_2pi0eta_2 = Chi2_etaMass[0] + Chi2_pi0Mass[1] + Chi2_pi0Mass[2];

      if (Esum > 500.0e-3) {
        DLorentzVector EtaVec = Pi0Cor[0] + Pi0Cor[1] + Pi0Cor[2];
        bool AnEta     = false; 
        if (GG[0].M() > 110.0e-3 && 
            GG[1].M() > 110.0e-3 && 
            GG[2].M() > 110.0e-3 
            && 
            GG[0].M() < 160.0e-3 && 
            GG[1].M() < 160.0e-3 && 
            GG[2].M() < 160.0e-3)
          AnEta = true;

        bool AnEtaPrim  = false; 
        bool AnEtaPrim1 = false; 
        bool AnEtaPrim2 = false; 
        bool AnEtaPrim3 = false; 
        if (GG[0].M() > 110.0e-3 && 
            GG[1].M() > 110.0e-3 && 
            GG[2].M() > 500.0e-3 
            && 
            GG[0].M() < 160.0e-3 && 
            GG[1].M() < 160.0e-3 && 
            GG[2].M() < 600.0e-3)
          AnEtaPrim1 = true;
        if (GG[0].M() > 110.0e-3 && 
            GG[1].M() > 500.0e-3 && 
            GG[2].M() > 110.0e-3 
            && 
            GG[0].M() < 160.0e-3 && 
            GG[1].M() < 600.0e-3 && 
            GG[2].M() < 160.0e-3)
          AnEtaPrim2 = true;
        if (GG[0].M() > 500.0e-3 && 
            GG[1].M() > 110.0e-3 && 
            GG[2].M() > 110.0e-3 
            && 
            GG[0].M() < 600.0e-3 && 
            GG[1].M() < 160.0e-3 && 
            GG[2].M() < 160.0e-3)
          AnEtaPrim3 = true;
        if (AnEtaPrim1 || AnEtaPrim2 || AnEtaPrim3)
          AnEtaPrim = true;

        if (AnEta && !AnEtaPrim && Esum > 500.0e-3) {
          if (Chi2_3pi0<bestChi0Eta) {
            bestChi0Eta = Chi2_3pi0;
            Index6gList.clear();
            Index6gList.push_back(combi6->combi[0]);
            Index6gList.push_back(combi6->combi[1]);
            Index6gList.push_back(combi6->combi[2]);
            Index6gList.push_back(combi6->combi[3]);
            Index6gList.push_back(combi6->combi[4]);
            Index6gList.push_back(combi6->combi[5]);
          }
        }

        if (!AnEta && AnEtaPrim && Esum > 900.0e-3) {
          double Esum2gg1 = EMList[combi6->combi[4]]->lorentzMomentum().E() + EMList[combi6->combi[5]]->lorentzMomentum().E();
          if (AnEtaPrim1 && Esum2gg1 > 500.0e-3) {
            if (Chi2_2pi0eta_0 < bestChi0EtaPrim) {
              bestChi0EtaPrim = Chi2_2pi0eta_0;
              Indexprim6gList.clear();
              Indexprim6gList.push_back(combi6->combi[0]);
              Indexprim6gList.push_back(combi6->combi[1]);
              Indexprim6gList.push_back(combi6->combi[2]);
              Indexprim6gList.push_back(combi6->combi[3]);
              Indexprim6gList.push_back(combi6->combi[4]);
              Indexprim6gList.push_back(combi6->combi[5]);
            }
          }
	  double Esum2gg2 = EMList[combi6->combi[2]]->lorentzMomentum().E() + EMList[combi6->combi[3]]->lorentzMomentum().E();
          if (AnEtaPrim2 && Esum2gg2 > 500.0e-3) {
	    if (Chi2_2pi0eta_1 < bestChi0EtaPrim) {
	      bestChi0EtaPrim = Chi2_2pi0eta_1;
              Indexprim6gList.clear();
              Indexprim6gList.push_back(combi6->combi[0]);
              Indexprim6gList.push_back(combi6->combi[1]);
              Indexprim6gList.push_back(combi6->combi[4]);
              Indexprim6gList.push_back(combi6->combi[5]);
              Indexprim6gList.push_back(combi6->combi[2]);
              Indexprim6gList.push_back(combi6->combi[3]);
            }
          }
	  double Esum2gg3 = EMList[combi6->combi[0]]->lorentzMomentum().E() + EMList[combi6->combi[1]]->lorentzMomentum().E();
          if (AnEtaPrim3 && Esum2gg3 > 500.0e-3) {
	    if (Chi2_2pi0eta_2 < bestChi0EtaPrim) {
              bestChi0EtaPrim = Chi2_2pi0eta_2;
	      Indexprim6gList.clear();
              Indexprim6gList.push_back(combi6->combi[2]);
              Indexprim6gList.push_back(combi6->combi[3]);
              Indexprim6gList.push_back(combi6->combi[4]);
              Indexprim6gList.push_back(combi6->combi[5]);
              Indexprim6gList.push_back(combi6->combi[0]);
              Indexprim6gList.push_back(combi6->combi[1]);
            }
          }
        }
      }
    }
  }
  if(Index6gList.size()>0)
    Indexprim6gList.clear();
}
