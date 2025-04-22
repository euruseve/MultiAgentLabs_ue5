#include "TCP/NeuralBot.h"

#include "Json.h"
#include "JsonUtilities.h"

ANeuralBot::ANeuralBot()
{
    PrimaryActorTick.bCanEverTick = true;
}

void ANeuralBot::BeginPlay()
{
    Super::BeginPlay();

    if (!TcpClient.Connect(TEXT("127.0.0.1"), 5050))
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to connect to Python server"));
        return;
    }

    GetWorld()->GetTimerManager().SetTimer(BrainTickHandle, this, &ANeuralBot::SendStateAndReceiveDirection, 0.2f, true);
}

void ANeuralBot::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!LastMoveDirection.IsZero())
    {
        const float MoveScale = 100.0f;
        AddMovementInput(LastMoveDirection * MoveScale);

        FRotator TargetRotation = LastMoveDirection.Rotation();
        TargetRotation.Pitch = 0.0f;
        TargetRotation.Roll = 0.0f;

        SetActorRotation(FMath::RInterpTo(GetActorRotation(), TargetRotation, DeltaTime, 5.0f));
    }
}

void ANeuralBot::SendStateAndReceiveDirection()
{
    FVector Location = GetActorLocation();

    TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
    TArray<TSharedPtr<FJsonValue>> InputArray;

    InputArray.Add(MakeShareable(new FJsonValueNumber(Location.X)));
    InputArray.Add(MakeShareable(new FJsonValueNumber(Location.Y)));
    InputArray.Add(MakeShareable(new FJsonValueNumber(TargetLocation.X)));
    InputArray.Add(MakeShareable(new FJsonValueNumber(TargetLocation.Y)));

    JsonObject->SetArrayField("input", InputArray);

    FString OutputString;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
    FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

    if (!TcpClient.Send(OutputString))
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to send data to server."));
        return;
    }

    FString ResponseString = TcpClient.Receive();
    if (ResponseString.IsEmpty())
    {
        UE_LOG(LogTemp, Warning, TEXT("No data received from server."));
        return;
    }

    TSharedPtr<FJsonObject> ResponseJson;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseString);
    if (FJsonSerializer::Deserialize(Reader, ResponseJson) && ResponseJson.IsValid())
    {
        double MoveX = ResponseJson->GetNumberField("move_x");
        double MoveY = ResponseJson->GetNumberField("move_y");

        LastMoveDirection = FVector(MoveX, MoveY, 0.0f);
        UE_LOG(LogTemp, Log, TEXT("Direction received from model: X=%f Y=%f"), MoveX, MoveY);
    }
}