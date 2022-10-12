// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>


#include "FileInfoWidget.h"
#include "Messenger/Chat/Components/FileTransferComponent.h"


void UFileInfoWidget::DownloadFileFromServer()
{
	if (!GetWorld()) return;

	if (auto* PC =GetWorld()->GetFirstPlayerController() )
	{
		if (auto* FileTransferComponent = PC->FindComponentByClass<UFileTransferComponent>())
		{
			FileTransferComponent->DownloadFileFromServer(FileInfo);
		}
	}
}
