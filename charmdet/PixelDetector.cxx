//
//  PixelDetector.cxx
//

#include "PixelDetector.h"

#include "PixelDetectorPoint.h"
/*
#include "TGeoManager.h"
#include "FairRun.h"                    // for FairRun
#include "FairRuntimeDb.h"              // for FairRuntimeDb
#include <iosfwd>                      // for ostream
#include "TList.h"                      // for TListIter, TList (ptr only)
#include "TObjArray.h"                  // for TObjArray
#include "TString.h"                    // for TString

#include "TClonesArray.h"
#include "TVirtualMC.h"

#include "TGeoBBox.h"
#include "TGeoTrd1.h"
#include "TGeoCompositeShape.h"
#include "TGeoTube.h"
#include "TGeoArb8.h"
#include "TGeoMaterial.h"
#include "TGeoMedium.h"
#include "TParticle.h"
#include "TVector3.h"

#include "TParticle.h"
#include "TParticlePDG.h"
#include "TParticleClassPDG.h"
#include "TVirtualMCStack.h"

#include "FairVolume.h"
#include "FairGeoVolume.h"
#include "FairGeoNode.h"
#include "FairRootManager.h"
#include "FairGeoLoader.h"
#include "FairGeoInterface.h"
#include "FairGeoTransform.h"
#include "FairGeoMedia.h"
#include "FairGeoMedium.h"
#include "FairGeoBuilder.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "FairLogger.h"

#include "ShipDetectorList.h"
#include "ShipUnit.h"
#include "ShipStack.h"

#include "TGeoUniformMagField.h"
#include <stddef.h>                     // for NULL
#include <iostream>                     // for operator<<, basic_ostream,etc
#include <string.h>
*/

#include "TGeoManager.h"
#include "FairRun.h"                    // for FairRun
#include "FairRuntimeDb.h"              // for FairRuntimeDb
#include <iosfwd>                    // for ostream
#include "TList.h"                      // for TListIter, TList (ptr only)
#include "TObjArray.h"                  // for TObjArray
#include "TString.h"                    // for TString
#include "TClonesArray.h"
#include "TVirtualMC.h"

#include "TGeoBBox.h"
#include "TGeoTrd1.h"
#include "TGeoCompositeShape.h"
#include "TGeoTube.h"
#include "TGeoArb8.h"
#include "TGeoMaterial.h"
#include "TGeoMedium.h"
#include "TParticle.h"
#include "TVector3.h"

#include "FairVolume.h"
#include "FairGeoVolume.h"
#include "FairGeoNode.h"
#include "FairRootManager.h"
#include "FairGeoLoader.h"
#include "FairGeoInterface.h"
#include "FairGeoMedia.h"
#include "FairGeoBuilder.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"

#include "ShipDetectorList.h"
#include "ShipUnit.h"
#include "ShipStack.h"

#include "TGeoUniformMagField.h"
#include <stddef.h>                     // for NULL
#include <iostream>    

ClassImp(PixelDetector)

using std::cout;
using std::endl;

using namespace ShipUnit;

PixelDetector::PixelDetector() : FairDetector("PixelDetector", kTRUE, kPixelDetector),
fTrackID(-1),
fVolumeID(-1),
fPos(),
fMom(),
fTime(-1.),
fLength(-1.),
fELoss(-1.),
fPixelDetectorPointCollection(new TClonesArray("PixelDetectorPoint"))
{}

PixelDetector::PixelDetector(const char* name, const Double_t PX, const Double_t PY, const Double_t PZ,
             /*const Double_t zPixel,*/ Bool_t Active, const char* Title): FairDetector(name, Active, kPixelDetector),
  fTrackID(-1),
  fVolumeID(-1),
  fPos(),
  fMom(),
  fTime(-1.),
  fLength(-1.),
  fELoss(-1.),
  fPixelDetectorPointCollection(new TClonesArray("PixelDetectorPoint"))
{
    PixelX = PX;
    PixelY = PY;
    PixelZ = PZ;
    //ZPixelPosition = zPixel;
    //PixInterStationX = 10.;
    //PixInterStationY = 10.;
}


/*void PixelDetector::SetSiliconDetPositions(Double_t zSi0, Double_t zSi1, Double_t zSi2, Double_t zSi3, Double_t zSi4, Double_t zSi5, Double_t PairSiDistance)
{ 
 zs0 = zSi0;
 zs1 = zSi1;
 zs2 = zSi2;
 zs3 = zSi3;
 zs4 = zSi4;
 zs5 = zSi5;
 pairwisedistance = PairSiDistance;
}*/
//Done in the python geometry building

PixelDetector::~PixelDetector()
{
    if (fPixelDetectorPointCollection) {
        fPixelDetectorPointCollection->Delete();
        delete fPixelDetectorPointCollection;
    }
}

void PixelDetector::Initialize()
{
    FairDetector::Initialize();
}

void PixelDetector::SetPixelPlaneParam(Double_t PlX, Double_t PlY, Double_t PlZ, Double_t Dist1, Double_t Dist2 )
{
    PixInterStationX = Dist1;
    PixInterStationY = Dist2;
    PixPlaneX = PlX;
    PixPlaneY = PlY;
}


// -----   Private method InitMedium
Int_t PixelDetector::InitMedium(const char* name)
{
    static FairGeoLoader *geoLoad=FairGeoLoader::Instance();
    static FairGeoInterface *geoFace=geoLoad->getGeoInterface();
    static FairGeoMedia *media=geoFace->getMedia();
    static FairGeoBuilder *geoBuild=geoLoad->getGeoBuilder();
    
    FairGeoMedium *ShipMedium=media->getMedium(name);
    
    if (!ShipMedium) {
        Fatal("InitMedium","Material %s not defined in media file.", name);
        return -1111;
    }
    TGeoMedium* medium=gGeoManager->GetMedium(name);
    if (medium!=NULL) return ShipMedium->getMediumIndex();
    return geoBuild->createMedium(ShipMedium);
}



void PixelDetector::ConstructGeometry()
{
    
    fLogger=FairLogger::GetLogger();
    fLogger->SetLogToScreen(1);
    fLogger->SetLogScreenLevel("DEBUG4");
    fLogger->SetLogVerbosityLevel("HIGH");

    InitMedium("silicon");
    TGeoMedium *silicon = gGeoManager->GetMedium("silicon");

    InitMedium("copper");
    TGeoMedium *copper = gGeoManager->GetMedium("copper");

    InitMedium("lead");
    TGeoMedium *lead = gGeoManager->GetMedium("lead");
      
    InitMedium("vacuum");
    TGeoMedium *vacuum = gGeoManager->GetMedium("vacuum");

    //InitMedium("molybdenum");
    //TGeoMedium *molybdenum = gGeoManager->GetMedium("molybdenum");

    TGeoVolume *top= gGeoManager->GetTopVolume(); 


    Double_t pixStationThickness=0.02*cm;
    //Double_t PixInterStationx= PixInterStationX;  //VK 15.6*cm;
    //Double_t PixInterStationy= PixInterStationY;  //VK 26.9*cm;
    
    //Double_t CuSheetZ = 0.01*cm;
    //Double_t distCopperSilicon = 0.1*cm;

      //starting to simulate SHiP charm Pixel detector
   
    TGeoBBox   *bPixel   = new TGeoBBox("PixelDetector", PixelX/2, PixelY/2, PixelZ/2); 
    TGeoVolume *volPixel = new TGeoVolume("volPixel", bPixel, vacuum); 
    //volPixel->SetTransparency(1);
    volPixel->SetLineColor(kGreen);
    volPixel->SetVisibility(kTRUE);
    top->AddNode(volPixel,1,new TGeoTranslation(0,0,ZPixelPosition));
  
    Double_t thickTestTarget = 18.248*cm*1.0;
    TGeoBBox *bTestTarget     = new TGeoBBox("TestTarget", PixelX/2, PixelY/2, thickTestTarget/2.); 
    TGeoVolume *volTestTarget = new TGeoVolume("volTestTarget", bTestTarget, lead);

    TGeoBBox *bPixPlaneX = new TGeoBBox("PixelPlaneX", PixPlaneX/2, PixPlaneY/2, PixPlaneZ/2);
    TGeoBBox *bPixPlaneY = new TGeoBBox("PixelPlaneY", PixPlaneY/2, PixPlaneX/2, PixPlaneZ/2);
    TGeoBBox *bBigPlane= new TGeoBBox("BigPlane", 2.*cm, 2.*cm, PixPlaneZ/2);  //--VK for acceptance test
    //TGeoBBox *bCuSheetX  = new TGeoBBox("CuSheetX",    PixPlaneX/2, PixPlaneY/2, CuSheetZ/2);
    //TGeoBBox *bCuSheetY  = new TGeoBBox("CuSheetY",    PixPlaneY/2, PixPlaneX/2, CuSheetZ/2);
    TGeoVolume *volPixPlaneX = new TGeoVolume("volPixelPlaneX", bPixPlaneX, silicon);
    TGeoVolume *volPixPlaneY = new TGeoVolume("volPixelPlaneY", bPixPlaneY, silicon);
    TGeoVolume *volBigPlane  = new TGeoVolume("volBigPlane", bBigPlane, silicon); //--VK for acceptance test
    //TGeoVolume *volCuSheetX  = new TGeoVolume("volCuSheetX", bCuSheetX, copper);
    //TGeoVolume *volCuSheetY  = new TGeoVolume("volCuSheetY", bCuSheetY, copper);
    volPixPlaneX->SetLineColor(kRed);
    volPixPlaneX->SetVisibility(kTRUE);
    AddSensitiveVolume(volPixPlaneX);
    volPixPlaneY->SetLineColor(kMagenta);
    volPixPlaneY->SetVisibility(kTRUE);
    AddSensitiveVolume(volPixPlaneY);

    volBigPlane->SetLineColor(kOrange);
    volBigPlane->SetVisibility(kTRUE);
    AddSensitiveVolume(volBigPlane); //--VK for acceptance test
/*
    //Int_t npixelplane=2;        //VK Pixel plane replica counter

    //Double_t zPixPlanePos = -PixelZ/2+CuSheetZ/2.; //npixelplane++;
    //volPixel->AddNode(volCuSheetX, 0,  new TGeoTranslation(PixPlaneX/4.,0., zPixPlanePos));
    //zPixPlanePos+=distCopperSilicon+PixPlaneZ/2;
    //volPixel->AddNode(volPixPlaneX,0, new TGeoTranslation(PixPlaneX/4.,0., zPixPlanePos));
    //=====================================Test setup. Comment out after use
    //volPixel->AddNode(volTestTarget,0, new TGeoTranslation(0.,0., zPixPlanePos+thickTestTarget/2.));               //--VK for acceptance test
    //volPixel->AddNode(volBigPlane,0,   new TGeoTranslation(0.,0., zPixPlanePos+thickTestTarget+5.*cm+PixPlaneZ/2));//--VK for acceptance test
    //--
    //volPixel->AddNode(volBigPlane,0,   new TGeoTranslation(0.,0., zPixPlanePos+PixPlaneZ/2))         ;//--VK for acceptance test
    //volPixel->AddNode(volBigPlane,1,   new TGeoTranslation(0.,0., zPixPlanePos+PixPlaneZ/2 + 5.*cm)); //--VK for acceptance test
    //volPixel->AddNode(volBigPlane,2,   new TGeoTranslation(0.,0., zPixPlanePos+PixPlaneZ/2 +15.*cm)); //--VK for acceptance test npixelplane should be 2!!!
    //======================================================================
    //zPixPlanePos+=pixInterStationX;
    //volPixel->AddNode(volPixPlaneX, new TGeoTranslation(PixPlaneX/4.,0., zPixPlanePos));
    //zPixPlanePos+=distCopperSilicon+CuSheetZ/2;
    //volPixel->AddNode(volCuSheetX,  npixelplane, new TGeoTranslation(PixPlaneX/4.,0., zPixPlanePos));

    //zPixPlanePos+=pixStationThickness; 
    //volPixel->AddNode(volCuSheetY, new TGeoTranslation(0.,PixPlaneX/4., zPixPlanePos));
    //zPixPlanePos+=distCopperSilicon+PixPlaneZ/2;
    //volPixel->AddNode(volPixPlaneY, npixelplane, new TGeoTranslation(0.,PixPlaneX/4., zPixPlanePos));
    //====
    zPixPlanePos+=pixInterStationY;    
    volPixel->AddNode(volPixPlaneX, new TGeoTranslation(PixPlaneX/4.,0., zPixPlanePos));
    zPixPlanePos+=distCopperSilicon+CuSheetZ/2;
    volPixel->AddNode(volCuSheetX,  npixelplane, new TGeoTranslation(PixPlaneX/4.,0., zPixPlanePos));

    zPixPlanePos+=pixStationThickness; npixelplane++;
    volPixel->AddNode(volCuSheetY, new TGeoTranslation(0.,PixPlaneX/4., zPixPlanePos));    
    zPixPlanePos+=distCopperSilicon+PixPlaneZ/2;
    volPixel->AddNode(volPixPlaneY, new TGeoTranslation(0.,PixPlaneX/4., zPixPlanePos));    */
}
     
    





Bool_t  PixelDetector::ProcessHits(FairVolume* vol)
{
    /** This method is called from the MC stepping */
    //Set parameters at entrance of volume. Reset ELoss.
    if ( gMC->IsTrackEntering() ) {
        fELoss  = 0.;
        fTime   = gMC->TrackTime() * 1.0e09;
        fLength = gMC->TrackLength();
        gMC->TrackPosition(fPos);
        gMC->TrackMomentum(fMom);
    }
    // Sum energy loss for all steps in the active volume
    fELoss += gMC->Edep();
    
    // Create PixelDetectorPoint at exit of active volume
    if ( gMC->IsTrackExiting()    ||
        gMC->IsTrackStop()       ||
        gMC->IsTrackDisappeared()   ) {
        fTrackID  = gMC->GetStack()->GetCurrentTrackNumber();
        fVolumeID = vol->getMCid();
	Int_t detID=0;
	gMC->CurrentVolID(detID);

	if (fVolumeID == detID) { return kTRUE; }
	fVolumeID = detID;
	
	if (fELoss == 0. ) { return kFALSE; }
        TParticle* p=gMC->GetStack()->GetCurrentTrack();
	Int_t pdgCode = p->GetPdgCode();
	
        TLorentzVector Pos; 
        gMC->TrackPosition(Pos); 
        Double_t xmean = (fPos.X()+Pos.X())/2. ;      
        Double_t ymean = (fPos.Y()+Pos.Y())/2. ;      
        Double_t zmean = (fPos.Z()+Pos.Z())/2. ;     
        
	
	AddHit(fTrackID,fVolumeID, TVector3(xmean, ymean,  zmean),
               TVector3(fMom.Px(), fMom.Py(), fMom.Pz()), fTime, fLength,
               fELoss, pdgCode);

	
        // Increment number of Pixel det points in TParticle
        ShipStack* stack = (ShipStack*) gMC->GetStack();
        stack->AddPoint(kPixelDetector);
    }
    
    return kTRUE;
}



void PixelDetector::EndOfEvent()
{
    fPixelDetectorPointCollection->Clear();
}


void PixelDetector::Register()
{
    
    /** This will create a branch in the output tree called
     TargetPoint, setting the last parameter to kFALSE means:
     this collection will not be written to the file, it will exist
     only during the simulation.
     */
    
    FairRootManager::Instance()->Register("PixelDetectorPoint", "PixelDetector",
                                          fPixelDetectorPointCollection, kTRUE);
}
// -----   Public method to Decode volume info  -------------------------------------------
// -----   returns hpt, arm, rpc numbers -----------------------------------
void PixelDetector::DecodeVolumeID(Int_t detID,int &nHPT)
{
  nHPT = detID;
}

TClonesArray* PixelDetector::GetCollection(Int_t iColl) const
{
    if (iColl == 0) { return fPixelDetectorPointCollection; }
    else { return NULL; }
}

void PixelDetector::Reset()
{
    fPixelDetectorPointCollection->Clear();
}


PixelDetectorPoint* PixelDetector::AddHit(Int_t trackID,Int_t detID,
                           TVector3 pos, TVector3 mom,
                           Double_t time, Double_t length,
			    Double_t eLoss, Int_t pdgCode)
{
    TClonesArray& clref = *fPixelDetectorPointCollection;
    Int_t size = clref.GetEntriesFast();

    return new(clref[size]) PixelDetectorPoint(trackID,detID, pos, mom,
					time, length, eLoss, pdgCode);
}

ClassImp(PixelDetector)






