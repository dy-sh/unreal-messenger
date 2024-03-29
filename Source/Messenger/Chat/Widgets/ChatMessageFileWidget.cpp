﻿// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>


#include "ChatMessageFileWidget.h"
#include "Messenger/Chat/Components/FileTransferComponent.h"


void UChatMessageFileWidget::DownloadFileFromServer()
{
	if (!GetWorld()) return;

	if (auto* PC = GetWorld()->GetFirstPlayerController())
	{
		if (auto* FileTransferComponent = PC->FindComponentByClass<UFileTransferComponent>())
		{
			FileTransferComponent->DownloadFileFromServer(GetFileId());
		}
	}
}


void UChatMessageFileWidget::SaveDownloadedFile(FString Path)
{
	if (!GetWorld()) return;

	if (auto* PC = GetWorld()->GetFirstPlayerController())
	{
		if (auto* FileTransferComponent = PC->FindComponentByClass<UFileTransferComponent>())
		{
			FileTransferComponent->SaveDownloadedFile(FileInfo, Path);
		}
	}
}


const FString& UChatMessageFileWidget::GetFileId()
{
	return !Message.FileId.IsEmpty() ? Message.FileId : FileInfo.FileId;
}
