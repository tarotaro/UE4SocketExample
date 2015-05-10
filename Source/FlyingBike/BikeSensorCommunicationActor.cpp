// Fill out your copyright notice in the Description page of Project Settings.

#include "FlyingBike.h"
#include "BikeSensorCommunicationActor.h"
#include <string>


// Sets default values
ABikeSensorCommunicationActor::ABikeSensorCommunicationActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABikeSensorCommunicationActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABikeSensorCommunicationActor::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void ABikeSensorCommunicationActor::EndPlay(const EEndPlayReason::Type EndPlayReason){
	if (ConnectionSocket != NULL){
		ConnectionSocket->Close();
	}
    Super::EndPlay(EndPlayReason);
}


bool ABikeSensorCommunicationActor::connectBikeSensor(const FString &IP,const int32 &port){
    ConnectionSocket = CreateTCPConnection(IP, port,1024);
    if(!ConnectionSocket){
        return false;
    }
    

    if(TimerHandle.IsValid() == false){
        GetWorldTimerManager().SetTimer(TimerHandle, this,
                                        &ABikeSensorCommunicationActor::TCPSocketListener, 1.0f / 60.0f, true);
    }
    
    
    return true;
}

bool ABikeSensorCommunicationActor::FormatIP4ToNumber(const FString& TheIP, uint8 (&Out)[4]){
    //IP Formatting
    TheIP.Replace( TEXT(" "), TEXT("") );
    
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //						   IP 4 Parts
    
    //String Parts
    TArray<FString> Parts;
    TheIP.ParseIntoArray( &Parts, TEXT("."), true );
    if ( Parts.Num() != 4 )
        return false;
    
    //String to Number Parts
    for ( int32 i = 0; i < 4; ++i )
    {
        Out[i] = FCString::Atoi( *Parts[i] );
    }
    
    return true;
}


FSocket* ABikeSensorCommunicationActor::CreateTCPConnection(const FString& TheIP, const int32 ThePort,const int32 ReceiveBufferSize)
{
	FSocket* retSocket = NULL;

	ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);

	if (SocketSubsystem != NULL)
	{
		retSocket = SocketSubsystem->CreateSocket(NAME_Stream, "SensorConnect", true);
	}

	FIPv4Address ip;
	FIPv4Address::Parse(TheIP, ip);
	auto addr = SocketSubsystem->CreateInternetAddr();
	addr->SetIp(ip.GetValue());//setIP
	addr->SetPort(ThePort);

	if (!retSocket->Connect(*addr)){
		return NULL;
	}
	
   
    return retSocket;
}



//Rama's String From Binary Array
//This function requires
//		#include <string>
FString ABikeSensorCommunicationActor::StringFromBinaryArray(const TArray<uint8>& BinaryArray)
{
    //Create a string from a byte array!
    std::string cstr( reinterpret_cast<const char*>(BinaryArray.GetData()), BinaryArray.Num() );
    return FString(cstr.c_str());
}

//Rama's TCP Socket Listener
void ABikeSensorCommunicationActor::TCPSocketListener()
{
    //~~~~~~~~~~~~~
    if(!ConnectionSocket) return;
    //~~~~~~~~~~~~~
    
    
    int32 sendType = 1;
    int sent;
    ConnectionSocket->Send((uint8 *)&sendType,sizeof(sendType),sent);
    
    //Binary Array!
    uint32 Size = 0;
    
    while (ConnectionSocket->HasPendingData(Size))
    {
        TArray<uint8> ReceivedData;
        ReceivedData.Init(FMath::Min(Size, 65507u));
        
        int32 Read = 0;
        ConnectionSocket->Recv(ReceivedData.GetData(), ReceivedData.Num(), Read);
        if(ReceivedData.Num() <= 0)
        {
        }else{
            const FString ReceivedUE4String = StringFromBinaryArray(ReceivedData);
            FormatReceiveDataToNumber(ReceivedUE4String,iBodyRate,iRotaion,fSpeed,iDirection);
        }
        
    }
    
}

bool ABikeSensorCommunicationActor::FormatReceiveDataToNumber(const FString& recvData, int32 &heart,int32 &rotation,float &speed,int32 &direction){
    //IP Formatting
    recvData.Replace( TEXT(" "), TEXT("") );
    
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //						   IP 4 Parts
    
    //String Parts
    TArray<FString> Parts;
    recvData.ParseIntoArray( &Parts, TEXT(","), true );
    if ( Parts.Num() != 4 )
        return false;
    
    heart = FCString::Atoi( *Parts[0] );
    rotation = FCString::Atoi( *Parts[1] );
    speed = FCString::Atof( *Parts[2] );
    direction = FCString::Atoi( *Parts[3] );

    
    return true;
}

float ABikeSensorCommunicationActor::GetBikeSpeed(){
    if(!ConnectionSocket)return 0.0f;
    return fSpeed;
}

int32  ABikeSensorCommunicationActor::GetBikeCadence(){
    if(!ConnectionSocket) return 0;
    return iRotaion;
}

int32  ABikeSensorCommunicationActor::GetBodyHeartRate(){
    if(!ConnectionSocket) return 0;
    return iBodyRate;
}

int32  ABikeSensorCommunicationActor::GetBikeDirection(){
    if(!ConnectionSocket) return 0;
    return iDirection;
}



