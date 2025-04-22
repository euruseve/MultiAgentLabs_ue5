#include "TCPClient.h"

bool FTCPClient::Connect(const FString& IP, int32 Port)
{
	ISocketSubsystem* Subsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
	TSharedRef<FInternetAddr> Addr = Subsystem->CreateInternetAddr();

	bool bIsValid;
	Addr->SetIp(*IP, bIsValid);
	Addr->SetPort(Port);

	Socket = Subsystem->CreateSocket(NAME_Stream, TEXT("TCPClient"), false);
	return bIsValid && Socket->Connect(*Addr);
}

bool FTCPClient::Send(const FString& Message)
{
	FTCHARToUTF8 Convert(*Message);
	int32 Sent;
	return Socket->Send((uint8*)Convert.Get(), Convert.Length(), Sent);
}

FString FTCPClient::Receive()
{
	TArray<uint8> Data;
	uint32 Size;
	FString Result;

	while (Socket->HasPendingData(Size))
	{
		Data.SetNumUninitialized(FMath::Min(Size, 65507u));
		int32 Read = 0;
		Socket->Recv(Data.GetData(), Data.Num(), Read);
		Result += FString(UTF8_TO_TCHAR(reinterpret_cast<const char*>(Data.GetData())));
	}

	return Result;
}