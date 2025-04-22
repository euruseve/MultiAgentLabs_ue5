#pragma once

#include "CoreMinimal.h"
#include "Sockets.h"
#include "SocketSubsystem.h"


class FTCPClient
{
public:
	bool Connect(const FString& IP, int32 Port);
	bool Send(const FString& Message);
	FString Receive();

private:
	FSocket* Socket;
};
