// Fill out your copyright notice in the Description page of Project Settings.

#include "TCP/TargetBot.h"
#include "Common/TcpSocketBuilder.h"
#include "Engine/TargetPoint.h"
#include "GameFramework/Actor.h"
#include "DrawDebugHelpers.h"

ATargetBot::ATargetBot()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ATargetBot::BeginPlay()
{
	Super::BeginPlay();

	ConnectToServer();
	RequestPath();
}

void ATargetBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bPathReceived)
	{
		MoveAlongPath(DeltaTime);
	}
}

void ATargetBot::ConnectToServer()
{
	ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
	TSharedRef<FInternetAddr> Addr = SocketSubsystem->CreateInternetAddr();

	bool bIsValid;
	Addr->SetIp(*ServerIP, bIsValid);
	Addr->SetPort(ServerPort);

	Socket = FTcpSocketBuilder(TEXT("BotSocket")).AsBlocking().Build();

	if (Socket->Connect(*Addr))
	{
		bIsConnected = true;
		UE_LOG(LogTemp, Warning, TEXT("TCP Connected"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("TCP Connection failed"));
	}
}

void ATargetBot::RequestPath()
{
	if (!bIsConnected || !Socket) return;

	FVector Start = GetActorLocation();
	FVector TargetLocation = TargetPoint->GetActorLocation();
	
	FString JsonRequest = FString::Printf(TEXT(
		"{\"start\": [%f, %f, %f], \"end\": [%f, %f, %f]}"),
		Start.X, Start.Y, Start.Z,
		TargetLocation.X, TargetLocation.Y, TargetLocation.Z);

	FTCHARToUTF8 Converter(*JsonRequest);
	int32 Sent = 0;
	Socket->Send((uint8*)Converter.Get(), Converter.Length(), Sent);
	
	uint8 Data[8192];
	int32 Read = 0;
	FString Response;

	while (Socket->Recv(Data, sizeof(Data), Read))
	{
		Response += FString(UTF8_TO_TCHAR(reinterpret_cast<const char*>(Data)), Read);
		if (Read < sizeof(Data)) break;
	}
	
	TArray<TSharedPtr<FJsonValue>> JsonArray;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response);

	if (FJsonSerializer::Deserialize(Reader, JsonArray))
	{
		PathPoints.Empty();

		for (const TSharedPtr<FJsonValue>& PointValue : JsonArray)
		{
			if (!PointValue.IsValid() || PointValue->Type != EJson::Array)
			{
				UE_LOG(LogTemp, Error, TEXT("Invalid path point format"));
				continue;
			}

			const TArray<TSharedPtr<FJsonValue>>& Coords = PointValue->AsArray();
			if (Coords.Num() == 3 &&
				Coords[0]->Type == EJson::Number &&
				Coords[1]->Type == EJson::Number &&
				Coords[2]->Type == EJson::Number)
			{
				FVector Point(
					static_cast<float>(Coords[0]->AsNumber()),
					static_cast<float>(Coords[1]->AsNumber()),
					static_cast<float>(Coords[2]->AsNumber())
				);
				PathPoints.Add(Point);

				DrawDebugSphere(
					GetWorld(),
					Point,
					15.0f,
					12,
					FColor::Green,
					true,
					10.0f
				);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Path point has invalid coordinates"));
			}
		}

		bPathReceived = true;
		CurrentTargetIndex = 0;
		UE_LOG(LogTemp, Warning, TEXT("The pathway: %d point(s)"), PathPoints.Num());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("JSON Parse failed: %s"), *Response);
	}
}


void ATargetBot::MoveAlongPath(float DeltaTime)
{
	if (CurrentTargetIndex >= PathPoints.Num()) return;

	FVector Target = PathPoints[CurrentTargetIndex];
	FVector Current = GetActorLocation();
	FVector Direction = (Target - Current).GetSafeNormal();

	AddMovementInput(Direction, 1.0f);

	if (FVector::Dist(Current, Target) < 100.f)
	{
		CurrentTargetIndex++;
	}
}

