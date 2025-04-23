#include "TCP/QBot.h"
#include "DrawDebugHelpers.h"

AQBot::AQBot()
{
    PrimaryActorTick.bCanEverTick = true;
    GridSize = 200.f; // Великий розмір сітки для кращої видимості
}

void AQBot::BeginPlay()
{
    Super::BeginPlay();

    GridPosition = FVector2D(0, 0);
    TargetWorldLocation = GetActorLocation();
    TcpClient.Connect(TEXT("127.0.0.1"), 6060);

    GenerateNewGoal();
    GetWorld()->GetTimerManager().SetTimer(DecisionHandle, this, &AQBot::MakeDecision, 1.0f, true);
}

void AQBot::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    DrawGrid(); // Спочатку малюємо сітку (фон)
    DrawFinalGoal(); // Потім фінальну ціль
    DrawNextMove(); // І нарешті наступний крок

    if (!IsCloseToTarget())
    {
        FVector Direction = (TargetWorldLocation - GetActorLocation()).GetSafeNormal();
        AddMovementInput(Direction);
        SetActorRotation(Direction.Rotation());
    }
}

bool AQBot::IsCloseToTarget() const
{
    return FVector::Dist2D(GetActorLocation(), TargetWorldLocation) < 5.0f;
}

void AQBot::MakeDecision()
{
    if (!IsCloseToTarget()) return;

    TSharedPtr<FJsonObject> Json = MakeShareable(new FJsonObject);
    TArray<TSharedPtr<FJsonValue>> StateArray;
    StateArray.Add(MakeShareable(new FJsonValueNumber(GridPosition.X)));
    StateArray.Add(MakeShareable(new FJsonValueNumber(GridPosition.Y)));
    Json->SetArrayField("state", StateArray);

    FString Out;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Out);
    FJsonSerializer::Serialize(Json.ToSharedRef(), Writer);

    TcpClient.Send(Out);
    FString Response = TcpClient.Receive();

    TSharedPtr<FJsonObject> RespJson;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response);
    if (FJsonSerializer::Deserialize(Reader, RespJson) && RespJson.IsValid())
    {
        int32 Action = RespJson->GetIntegerField("action");
        UE_LOG(LogTemp, Log, TEXT("[QBot] Received action: %d"), Action);
        MoveAgent(Action);
    }
}

void AQBot::MoveAgent(int32 Action)
{
    FVector2D OldPosition = GridPosition;

    switch (Action)
    {
    case 0: GridPosition.Y += 1; break; // Up
    case 1: GridPosition.Y -= 1; break; // Down
    case 2: GridPosition.X -= 1; break; // Left
    case 3: GridPosition.X += 1; break; // Right
    }

    bool bInvalid = GridPosition.X < -10 || GridPosition.X > 10 || GridPosition.Y < -10 || GridPosition.Y > 10;
    bool bGoal = GridPosition == GoalPosition;

    FVector2D NextState = GridPosition;
    float Reward = -0.01f;
    bool bDone = false;

    if (bGoal)
    {
        Reward = 1.0f;
        bDone = true;
        GenerateNewGoal();
    }
    else if (bInvalid)
    {
        Reward = -1.0f;
        GridPosition = OldPosition;
        NextState = GridPosition;
        bDone = true;
    }

    TargetWorldLocation = FVector(GridPosition.X * GridSize, GridPosition.Y * GridSize, GetActorLocation().Z);
    SendReward(Reward, NextState, bDone);
}

void AQBot::SendReward(float Reward, const FVector2D& NextState, bool bDone)
{
    TSharedPtr<FJsonObject> Json = MakeShareable(new FJsonObject);

    TArray<TSharedPtr<FJsonValue>> StateArray;
    StateArray.Add(MakeShareable(new FJsonValueNumber(GridPosition.X)));
    StateArray.Add(MakeShareable(new FJsonValueNumber(GridPosition.Y)));

    TArray<TSharedPtr<FJsonValue>> NextArray;
    NextArray.Add(MakeShareable(new FJsonValueNumber(NextState.X)));
    NextArray.Add(MakeShareable(new FJsonValueNumber(NextState.Y)));

    Json->SetArrayField("state", StateArray);
    Json->SetArrayField("next_state", NextArray);
    Json->SetNumberField("reward", Reward);
    Json->SetBoolField("done", bDone);

    FString Out;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Out);
    FJsonSerializer::Serialize(Json.ToSharedRef(), Writer);
    TcpClient.Send(Out);
}

void AQBot::GenerateNewGoal()
{
    int32 OffsetX = FMath::RandRange(-5, 5);
    int32 OffsetY = FMath::RandRange(-5, 5);
    GoalPosition = GridPosition + FVector2D(OffsetX, OffsetY);
    UE_LOG(LogTemp, Log, TEXT("New goal at (%d, %d)"), (int32)GoalPosition.X, (int32)GoalPosition.Y);
}

void AQBot::DrawFinalGoal()
{
    FVector GoalWorld = FVector(GoalPosition.X * GridSize, GoalPosition.Y * GridSize, GetActorLocation().Z + 50);
    DrawDebugBox(GetWorld(), GoalWorld, FVector(GridSize*0.4f), FColor::Green, false, -1, 0, 5.f);
    DrawDebugString(GetWorld(), GoalWorld + FVector(0,0,50), FString::Printf(TEXT("Final Goal")), nullptr, FColor::Green, 0, true);
}

void AQBot::DrawNextMove()
{
    if (IsCloseToTarget()) return; // Не показуємо наступний крок, якщо стоїмо на місці
    
    FVector Direction = (TargetWorldLocation - GetActorLocation()).GetSafeNormal();
    FVector NextMoveLocation = GetActorLocation() + Direction * GridSize;
    NextMoveLocation.Z = GetActorLocation().Z + 30;
    
    DrawDebugBox(GetWorld(), NextMoveLocation, FVector(GridSize*0.3f), FColor::Blue, false, -1, 0, 3.f);
    DrawDebugString(GetWorld(), NextMoveLocation + FVector(0,0,30), FString::Printf(TEXT("Next Move")), nullptr, FColor::Blue, 0, true);
}

void AQBot::DrawGrid()
{
    FVector Origin = FVector::ZeroVector;
    Origin.Z = GetActorLocation().Z;
    
    // Велика сітка (-10 до 10 по обох осях)
    for (int32 X = -10; X <= 10; ++X)
    {
        for (int32 Y = -10; Y <= 10; ++Y)
        {
            FVector CellLocation = Origin + FVector(X * GridSize, Y * GridSize, 0);
            
            // Малюємо тільки межі клітинок
            DrawDebugBox(GetWorld(), CellLocation, FVector(GridSize*0.5f), FColor::White, false, -1, 0, 0.5f);
            
            // Підписуємо координати
            if (FMath::Abs(X) == 10 || FMath::Abs(Y) == 10 || X == 0 || Y == 0)
            {
                DrawDebugString(GetWorld(), CellLocation + FVector(0,0,20), FString::Printf(TEXT("(%d,%d)"), X, Y), nullptr, FColor::Silver, 0, true);
            }
        }
    }
    
    FVector CurrentCell = Origin + FVector(GridPosition.X * GridSize, GridPosition.Y * GridSize, 0);
    DrawDebugBox(GetWorld(), CurrentCell, FVector(GridSize*0.45f), FColor::Yellow, false, -1, 0, 1.f);
}