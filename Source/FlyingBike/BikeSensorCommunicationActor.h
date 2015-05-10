// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Networking.h"
#include "BikeSensorCommunicationActor.generated.h"

UCLASS()
class FLYINGBIKE_API ABikeSensorCommunicationActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABikeSensorCommunicationActor();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;
    
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    

    
    UFUNCTION(BlueprintCallable, BlueprintPure, Category="BikeSensor")
    float GetBikeSpeed();
    UFUNCTION(BlueprintCallable, BlueprintPure, Category="BikeSensor")
    int32  GetBikeCadence();
    UFUNCTION(BlueprintCallable, BlueprintPure, Category="BikeSensor")
    int32  GetBodyHeartRate();
    UFUNCTION(BlueprintCallable, BlueprintPure, Category="BikeSensor")
    int32  GetBikeDirection();
    
    UFUNCTION(BlueprintCallable, Category = "BikeSensor")
    bool connectBikeSensor(const FString &IP,const int32 &port);
    
    
private:
    
   
    FSocket *ConnectionSocket;
    FIPv4Endpoint RemoteAddressForConnection;
    FTimerHandle TimerHandle;

    float fSpeed;
    int32 iRotaion;
    int32 iBodyRate;
    int32 iDirection;
    
    bool FormatIP4ToNumber(const FString& TheIP, uint8 (&Out)[4]);
    bool FormatReceiveDataToNumber(const FString& recvData, int32 &heart,int32 &rotation,float &speed,int32 &direction);
    FSocket* CreateTCPConnection(const FString& TheIP, const int32 ThePort,const int32 ReceiveBufferSize);
    
    FString StringFromBinaryArray(const TArray<uint8>& BinaryArray);
    void TCPSocketListener();
	
	
};
