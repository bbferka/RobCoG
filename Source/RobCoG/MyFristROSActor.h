// Copyright 2017, Institute for Artificial Intelligence - University of Bremen

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Actor.h"
#include "Engine.h"
//#include "EditorEngine.h"
#include "CoreMinimal.h"
#include "ROSBridgeHandler.h"
#include "ROSBridgePublisher.h"
#include "ROSBridgeSrvServer.h"
#include "ROSBridgeSubscriber.h"
#include "RGBDCamera.h"

#include "rosgraph_msgs/Clock.h"
#include "tutorial_srvs/AddTwoInts.h"
#include "visualization_msgs/MarkerArray.h"

#include "UpdateObjectsSrv.h"
#include "ROSMarkerArraySubscriber.h"

#include "MyFristROSActor.generated.h"

typedef TPair < FString, FROSBridgeMsgGeometrymsgsPoseStamped > TNamePosePair;

class FROSUpdateRGBDActorPositionServer : public FROSBridgeSrvServer
{
public:
  bool Success;

  TQueue <TNamePosePair> message_queue;

  FROSUpdateRGBDActorPositionServer(FString Name):
    FROSBridgeSrvServer(Name, TEXT("iai_robosherlock_msgs/UpdateObjects"))
  {

  }

  TSharedPtr<FROSBridgeSrv::SrvRequest> FromJson(TSharedPtr<FJsonObject> JsonObject) const override
  {
    TSharedPtr<FROSBridgeSrvUpdateObjects::Request> Request_ =
      MakeShareable(new FROSBridgeSrvUpdateObjects::Request());
    Request_->FromJson(JsonObject);
    return TSharedPtr<FROSBridgeSrv::SrvRequest>(Request_);
  }

  TSharedPtr<FROSBridgeSrv::SrvResponse> CallBack(TSharedPtr<FROSBridgeSrv::SrvRequest> Request) override
  {
    TSharedPtr<FROSBridgeSrvUpdateObjects::Request> Request_ =
      StaticCastSharedPtr<FROSBridgeSrvUpdateObjects::Request>(Request);

    FString name = Request_->GetName().ToString();
    FROSBridgeMsgGeometrymsgsPoseStamped ps = Request_->GetPoseStamped();

    TNamePosePair posePair(name,ps);
    message_queue.Enqueue(posePair);
    bool ValidMarkers  = true;
    if(ValidMarkers)
      return MakeShareable<FROSBridgeSrv::SrvResponse>
             (new FROSBridgeSrvUpdateObjects::Response(true));
    else
      return MakeShareable<FROSBridgeSrv::SrvResponse>
             (new FROSBridgeSrvUpdateObjects::Response(false));
  }

  void setSucces(bool b)
  {
    Success = b;
  }
};

UCLASS()
class ROBCOG_API AMyFristROSActor : public AActor
{
  GENERATED_BODY()

public:
  // Sets default values for this actor's properties
  AMyFristROSActor();

  TSharedPtr<FROSBridgeHandler> Handler;

  TSharedPtr<FROSMarkerArraySubscriber> Subscriber;
  TSharedPtr<FROSUpdateRGBDActorPositionServer> AddNewObjectServiceServer;

  UPROPERTY()
  FString ROSBridgeServerIPAddr;

  UPROPERTY()
  uint32 ROSBridgeServerPort;


protected:
  // Called when the game starts or when spawned
  virtual void BeginPlay() override;
  // Called when the game ends
  virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
  // Called every frame
  virtual void Tick(float DeltaTime) override;



};
