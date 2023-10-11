// $Id$
//
//    File: JEventProcessor_FCALLEDTree.cc
// Created: Fri May 19 12:39:24 EDT 2017
// Creator: mashephe (on Linux stanley.physics.indiana.edu 2.6.32-642.6.2.el6.x86_64 unknown)
//

#include "JEventProcessor_FCALLEDTree.h"

#include "FCAL/DFCALGeometry.h"
#include "FCAL/DFCALHit.h"
#include "FCAL/DFCALDigiHit.h"

#include "TTree.h"

// Routine used to create our JEventProcessor
extern "C"{
  void InitPlugin(JApplication *app){
    InitJANAPlugin(app);
    app->Add(new JEventProcessor_FCALLEDTree());
  }
} // "C"


//------------------
// JEventProcessor_FCALLEDTree (Constructor)
//------------------
JEventProcessor_FCALLEDTree::JEventProcessor_FCALLEDTree()
{
}

//------------------
// ~JEventProcessor_FCALLEDTree (Destructor)
//------------------
JEventProcessor_FCALLEDTree::~JEventProcessor_FCALLEDTree()
{

}

//------------------
// Init
//------------------
void JEventProcessor_FCALLEDTree::Init()
{
  auto app = GetApplication();

  btree = 0;
  app->SetDefaultParameter( "FCALLED:Tree", btree );

  lockService = app->GetService<JLockService>();

  // This is called once at program startup.
  lockService->RootWriteLock();
  
  if (btree == 1) {
  
    m_tree = new TTree( "fcalBlockHits", "FCAL Block Hits" );

    m_tree->Branch( "nHits", &m_nHits, "nHits/I" );
    m_tree->Branch( "chan", m_chan, "chan[nHits]/I" );
    m_tree->Branch( "x", m_x, "x[nHits]/F" );
    m_tree->Branch( "y", m_y, "y[nHits]/F" );
    m_tree->Branch( "E", m_E, "E[nHits]/F" );
    m_tree->Branch( "t", m_t, "t[nHits]/F" );
    m_tree->Branch( "integ", m_integ, "integ[nHits]/F" );
    m_tree->Branch( "ped", m_ped, "ped[nHits]/F" );
    m_tree->Branch( "peak", m_peak, "peak[nHits]/F" );
    
    m_tree->Branch( "run", &m_run, "run/I" );
    m_tree->Branch( "event", &m_event, "event/L" );
    m_tree->Branch( "eTot", &m_eTot, "eTot/F" );
  }
  
  lockService->RootUnLock();
}

//------------------
// BeginRun
//------------------
void JEventProcessor_FCALLEDTree::BeginRun(const std::shared_ptr<const JEvent>& event)
{

  // this is not thread safe and may lead to an incorrect run number for
  // a few events in the worst case scenario -- I don't think it is a major problem
  m_run = event->GetRunNumber();
}

//------------------
// Process
//------------------
void JEventProcessor_FCALLEDTree::Process(const std::shared_ptr<const JEvent>& event)
{
 
  vector< const DFCALHit* > hits;
  event->Get( hits );

  if( hits.size() > kMaxHits ) return;
  
  vector<const DFCALGeometry*> fcalGeomVect;
  event->Get( fcalGeomVect );
  if (fcalGeomVect.size() < 1)
      return;

  const DFCALGeometry& fcalGeom = *(fcalGeomVect[0]);
  
  lockService->RootFillLock(this);

  m_event = event->GetEventNumber();
  
  m_nHits = 0;
  m_eTot = 0;
  
  for( vector< const DFCALHit* >::const_iterator hit = hits.begin();
       hit != hits.end();
       ++hit ){
    
    vector< const DFCALDigiHit* > digiHits;
    (**hit).Get( digiHits );
    if( digiHits.size() != 1 ) std::cout << "ERROR:  wrong size!! " << std::endl;
    
    const DFCALDigiHit& dHit = *(digiHits[0]);

    m_chan[m_nHits] = fcalGeom.channel( (**hit).row, (**hit).column );
    m_x[m_nHits] = (**hit).x;
    m_y[m_nHits] = (**hit).y;
    m_E[m_nHits] = (**hit).E;
    m_t[m_nHits] = (**hit).t;
    
    m_eTot += (**hit).E;

    m_ped[m_nHits] = (float)dHit.pedestal/dHit.nsamples_pedestal;
    m_peak[m_nHits] = dHit.pulse_peak - m_ped[m_nHits];
    m_integ[m_nHits] = dHit.pulse_integral -
      (m_ped[m_nHits]*dHit.nsamples_integral);
  
    int row = fcalGeom.row((**hit).x);
    int col = fcalGeom.column((**hit).y);
    
    Fill2DHistogram("hv_scan","","XYGeo", row, col, ";row;column #;Counts", 59, 0, 59, 59, 0, 59);
    Fill2DWeightedHistogram("hv_scan","","XYGeo_w", row, col, (**hit).E, ";row;column #;E_{max}^{sum} [GeV]", 59, 0, 59, 59, 0, 59);
    Fill2DHistogram("hv_scan","","m_E", fcalGeom.channel((**hit).row, (**hit).column ), (**hit).E, ";channel;energy;Counts", 2800, 0, 2800, 1200, 0, 12.);
    Fill2DHistogram("hv_scan","","m_ped", fcalGeom.channel((**hit).row, (**hit).column ), (float)dHit.pedestal/dHit.nsamples_pedestal, ";channel;pedestal;Counts", 2800, 0, 2800, 4096, 0., 4096.);
    Fill2DHistogram("hv_scan","","m_peak", fcalGeom.channel((**hit).row, (**hit).column ), dHit.pulse_peak - m_ped[m_nHits], ";channel;peak;Counts", 2800, 0, 2800, 4096, 0., 4096.);
    Fill2DHistogram("hv_scan","","m_integ", fcalGeom.channel((**hit).row, (**hit).column ), dHit.pulse_integral - (m_ped[m_nHits]*dHit.nsamples_integral), ";channel;integ;Counts", 2800, 0, 2800, 4096, 0., 4096.);

    ++m_nHits;
  }

  if (btree == 1) {  
    m_tree->Fill();
  }
  
  lockService->RootFillUnLock(this);
}

//------------------
// EndRun
//------------------
void JEventProcessor_FCALLEDTree::EndRun()
{
  // This is called whenever the run number changes, before it is
  // changed to give you a chance to clean up before processing
  // events from the next run number.
}

//------------------
// Finish
//------------------
void JEventProcessor_FCALLEDTree::Finish()
{
  // Called before program exit after event processing is finished.
  lockService->RootWriteLock();
  m_tree->Write();
  lockService->RootUnLock();
}

