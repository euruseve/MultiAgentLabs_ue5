// Fill out your copyright notice in the Description page of Project Settings.

#include "TCP/CriticBot.h"
#include "Json.h"
#include "JsonUtilities.h"
#include "DrawDebugHelpers.h"
#include "TimerManager.h"

ACriticBot::ACriticBot()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ACriticBot::BeginPlay()
{
    Super::BeginPlay();

    TcpClient.Connect(TEXT("127.0.0.1"), 6061);

    GridPosition = FVector2D(0, 0);
    GoalPosition = FVector2D(FMath::RandRange(-3, 3), FMath::RandRange(-3, 3));
    TargetWorldLocation = GetActorLocation();

    GetWorld()->GetTimerManager().SetTimer(DecisionTimer, this, &ACriticBot::MakeDecision, 0.25f, true);
}

void ACriticBot::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!IsCloseToTarget())
    {
        FVector Direction = (TargetWorldLocation - GetActorLocation()).GetSafeNormal();
        AddMovementInput(Direction);

        FRotator NewRotation = Direction.Rotation();
        SetActorRotation(NewRotation);
    }
    
    DrawPath();
}

bool ACriticBot::IsCloseToTarget() const
{
    return FVector::Dist2D(GetActorLocation(), TargetWorldLocation) < 10.f;
}

void ACriticBot::DrawPath()
{
    for (int32 i = 0; i < DebugPath.Num() - 1; ++i)
    {
        DrawDebugLine(GetWorld(), DebugPath[i], DebugPath[i + 1], FColor::Yellow, false, 1.0f, 0, 3.0f);
    }
}

void ACriticBot::MakeDecision()
{
    if (!IsCloseToTarget()) return;

    TSharedPtr<FJsonObject> Json = MakeShareable(new FJsonObject);
    TArray<TSharedPtr<FJsonValue>> StateArray;

    StateArray.Add(MakeShareable(new FJsonValueNumber(GridPosition.X)));
    StateArray.Add(MakeShareable(new FJsonValueNumber(GridPosition.Y)));
    StateArray.Add(MakeShareable(new FJsonValueNumber(GoalPosition.X)));
    StateArray.Add(MakeShareable(new FJsonValueNumber(GoalPosition.Y)));

    Json->SetArrayField("state", StateArray);

    FString Out;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Out);
    FJsonSerializer::Serialize(Json.ToSharedRef(), Writer);
    TcpClient.Send(Out);

    FString Response = TcpClient.Receive();

    TSharedPtr<FJsonObject> ResponseJson;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response);
    if (FJsonSerializer::Deserialize(Reader, ResponseJson) && ResponseJson.IsValid())
    {
        int32 Action = ResponseJson->GetIntegerField("action");
        UE_LOG(LogTemp, Log, TEXT("[ActorCriticBot] Action received: %d"), Action);
        MoveInDirection(Action);
    }
}

void ACriticBot::MoveInDirection(int32 Action)
{
    FVector2D OldPos = GridPosition;

    switch (Action)
    {
    case 0: GridPosition.Y += 1; break;
    case 1: GridPosition.Y -= 1; break;
    case 2: GridPosition.X -= 1; break;
    case 3: GridPosition.X += 1; break;
    }

    bool bInvalid = FMath::Abs(GridPosition.X) > 5 || FMath::Abs(GridPosition.Y) > 5;
    bool bGoal = GridPosition == GoalPosition;

    float Reward = 0.0f;
    bool bDone = false;

    if (bInvalid)
    {
        GridPosition = OldPos;
        Reward = -1.0f;
        bDone = true;
    }
    else if (bGoal)
    {
        Reward = 1.0f;
        bDone = true;
        GoalPosition = FVector2D(FMath::RandRange(-3, 3), FMath::RandRange(-3, 3));
        UE_LOG(LogTemp, Log, TEXT("[ActorCriticBot] New goal set at (%d, %d)"), (int32)GoalPosition.X, (int32)GoalPosition.Y);
    }
    else
    {
        Reward = -0.01f;
    }

    TargetWorldLocation = GetActorLocation() + FVector((GridPosition.X - OldPos.X) * GridSize, (GridPosition.Y - OldPos.Y) * GridSize, 0);
    DebugPath.Add(TargetWorldLocation);
    SendTrainingData(Reward, bDone);
}

void ACriticBot::SendTrainingData(float Reward, bool bDone)
{
    TSharedPtr<FJsonObject> Json = MakeShareable(new FJsonObject);
    TArray<TSharedPtr<FJsonValue>> StateArray;

    StateArray.Add(MakeShareable(new FJsonValueNumber(GridPosition.X)));
    StateArray.Add(MakeShareable(new FJsonValueNumber(GridPosition.Y)));
    StateArray.Add(MakeShareable(new FJsonValueNumber(GoalPosition.X)));
    StateArray.Add(MakeShareable(new FJsonValueNumber(GoalPosition.Y)));

    Json->SetArrayField("state", StateArray);
    Json->SetNumberField("reward", Reward);
    Json->SetBoolField("done", bDone);
    Json->SetBoolField("train", true);

    FString Out;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Out);
    FJsonSerializer::Serialize(Json.ToSharedRef(), Writer);
    TcpClient.Send(Out);
}


