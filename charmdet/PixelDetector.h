#ifndef Pixel_H
#define Pixel_H

#include "FairModule.h"                 // for FairModule
#include "FairDetector.h"

#include "Rtypes.h"                     // for ShipMuonShield::Class, Bool_t, etc

#include <string>                       // for string

#include "TVector3.h"
#include "TLorentzVector.h"

class PixelPoint;
class FairVolume;
class TClonesArray;

class PixelDetector : {

public:

bool debug = False;

PixelDetector();
PixelDetector(int NPixelPlanes);

class Pixel : public FairDetector
{
public:
  Pixel(const char* name, const Double_t PX, const Double_t PY, const Double_t PZ, const Double_t zPixel, Bool_t Active, const char* Title = "Pixel");
    Pixel();
    virtual ~Pixel();
    
    /**      Create the detector geometry        */
    void ConstructGeometry();  
    void SetPixelPlaneParam(Double_t X, Double_t Y, Double_t Z, Double_t Dist1, Double_t Dist2);
    void SetSiliconDZ(Double_t SiliconDZ);
    void SetSiliconDetPositions(Double_t zSi, Double_t PairSiDistance);
    void SetSiliconDetAngles(Double_t Si_ang_zx, Double_t Si_ang_yz, Si_ang_xz);
    /**      Initialization of the detector is done here    */
    virtual void Initialize();
    /**  Method called for each step during simulation (see FairMCApplication::Stepping()) */
    virtual Bool_t ProcessHits( FairVolume* v=0);
    /**       Registers the produced collections in FAIRRootManager.     */
    virtual void   Register();
    /** Gets the produced collections */
    virtual TClonesArray* GetCollection(Int_t iColl) const ;
    /**      has to be called after each event to reset the containers      */
    virtual void   Reset();
    /**      How to add your own point of type PixelPoint to the clones array */
    PixelPoint* AddHit(Int_t trackID, Int_t detID, TVector3 pos, TVector3 mom,
		     Double_t time, Double_t length, Double_t eLoss, Int_t pdgCode);
	    /** Initialization of the Pixel plane */
    Int_t InitMedium(const char* name);
    TGeoVolume *sensPlane = gGeoManager->MakeBox("PixPLane",silicon,Px,Py,Pz); // half thicknesses
    volPixel->AddNode(Pixel, 1, new TGeoTranslation(0, 0, zLoc));
    AddSensitiveVolume(Pixel);
    virtual void   CopyClones( TClonesArray* cl1,  TClonesArray* cl2 , Int_t offset) {;}
    virtual void   SetSpecialPhysicsCuts() {;}
    virtual void   EndOfEvent();
    virtual void   FinishPrimary() {;}
    virtual void   FinishRun() {;}
    virtual void   BeginPrimary() {;}
    virtual void   PostTrack() {;}
    virtual void   PreTrack() {;}
    virtual void   BeginEvent() {;}
    void DecodeVolumeID(Int_t detID,int &nHPT);
    
private:
    
    /** Track information to be stored until the track leaves the active volume. */
    Int_t          fTrackID;           //!  track index
    Int_t          fVolumeID;          //!  volume id
    TLorentzVector fPos;               //!  position at entrance
    TLorentzVector fMom;               //!  momentum at entrance
    Double32_t     fTime;              //!  time
    Double32_t     fLength;            //!  length
    Double32_t     fELoss;             //!  energy loss
    
    /** container for data points */
    TClonesArray*  fPixelPointCollection;
    
protected:

 

    //Pixel detector
    
    Double_t   PixelX;
    Double_t   PixelY;
    Double_t   XPixelPosition;
    Double_t   YPixelPosition;
    Double_t   ZPixelPosition;
    
    Int_t InitMedium(const char* name);


  
         
    //attributes for the pixel plane
    Double_t PixPlaneX;
    Double_t PixPlaneY;
    Double_t PixPlaneZ;
    Double_t Pixel_ang_xy;
    Double_t Pixel_ang_yz;
    Double_t Pixel_ang_zx;
    Double_t PixIntersectionX;
    Double_t PixIntersectionY;
    Int_t NPixelModulesperPLane;
    Pixel(const Pixel&);
    Pixel& operator=(const Pixel&);


};


	
	
	int getNPixelPlanes(){
		return NPixelPlanes;}
		
	void setNPixelPlanes(int nb){
		NPixelPlanes = nb;}
	
	private:
	int NPixelPlanes;
	
	ClassDef(PixelDetector,1)
	
	}

#endif

