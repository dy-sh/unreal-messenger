// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>


#include "ChatMessageFileWidget.h"
#include "Messenger/Chat/Components/FileTransferComponent.h"


void UChatMessageFileWidget::DownloadFileFromServer()
{
	if (!GetWorld()) return;
	if (FileInfo.State != ETransferringFileState::None && FileInfo.State != ETransferringFileState::Uploaded) return;

	if (auto* PC = GetWorld()->GetFirstPlayerController())
	{
		if (auto* FileTransferComponent = PC->FindComponentByClass<UFileTransferComponent>())
		{
			FileTransferComponent->DownloadFileFromServer(Message.FileId);
		}
	}
}


void UChatMessageFileWidget::SaveDownloadedFile(FString Path)
{
	if (!GetWorld()) return;
	if (FileInfo.State != ETransferringFileState::Downloaded) return;

	if (auto* PC = GetWorld()->GetFirstPlayerController())
	{
		if (auto* FileTransferComponent = PC->FindComponentByClass<UFileTransferComponent>())
		{
			FileTransferComponent->SaveDownloadedFile(FileInfo, Path);
		}
	}
}
