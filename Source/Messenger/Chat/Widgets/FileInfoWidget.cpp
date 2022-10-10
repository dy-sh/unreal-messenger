// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>


#include "FileInfoWidget.h"

#include "Messenger/Chat/Components/ChatComponent.h"


void UFileInfoWidget::ReceiveFile()
{
	if (!GetWorld()) return;

	if (auto* PC =GetWorld()->GetFirstPlayerController() )
	{
		if (auto* ChatComponent = PC->FindComponentByClass<UChatComponent>())
		{
			ChatComponent->ServerGetFile(FileInfo);
		}
	}
}
