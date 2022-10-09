// Copyright 2022 Dmitry Savosh <d.savosh@gmail.com>

#pragma once
#pragma warning(disable : 4668)

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FileUtilsLibrary.generated.h"



// Flag Enum for saving multiple or single file's
UENUM(BlueprintType)
enum EEasyFileDialogFlags
{
	Single = 0x00, // No flags
	Multiple = 0x01  // Allow multiple file selections
};



UCLASS()
class FILEUTILS_API UFileUtilsLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	// Open File Dialog
	UFUNCTION(BlueprintCallable, Category = "EasyFileDialog")
		static bool OpenFileDialog(const FString& DialogTitle, const FString& DefaultPath, const FString& DefaultFile, const FString& FileTypes, EEasyFileDialogFlags Flags, TArray< FString >& OutFilenames);

	// Save File Dialog
	UFUNCTION(BlueprintCallable, Category = "EasyFileDialog")
		static bool SaveFileDialog(const FString& DialogTitle, const FString& DefaultPath, const FString& DefaultFile, const FString& FileTypeDescription, const FString& FileType, EEasyFileDialogFlags Flags, TArray< FString >& OutFilenames);

	// Open Folder Dialog
	UFUNCTION(BlueprintCallable, Category = "EasyFileDialog")
		static bool OpenFolderDialog(const FString& DialogTitle, const FString& DefaultPath, FString& OutFolderName);

	// Open file dialog core, called from the blueprint function library
	static bool OpenFileDialogCore(const FString& DialogTitle, const FString& DefaultPath, const FString& DefaultFile, const FString& FileTypes, uint32 Flags, TArray< FString >& OutFilenames);

	// Save file dialog core, called from the blueprint function library
	static bool SaveFileDialogCore(const FString& DialogTitle, const FString& DefaultPath, const FString& DefaultFile, const FString& FileTypes, uint32 Flags, TArray< FString >& OutFilenames);

	// Open folder dialog core, called from the blueprint function library
	static bool OpenFolderDialogCore(const FString& DialogTitle, const FString& DefaultPath, FString& OutFolderName);


private:
	// Both open file dialog and save file dialog using this same function with different save parameter.
	static bool FileDialogShared(bool bSave, const void* ParentWindowHandle, const FString& DialogTitle, const FString& DefaultPath, const FString& DefaultFile, const FString& FileTypes, uint32 Flags, TArray<FString>& OutFilenames, int32& OutFilterIndex);
	
	// The main Open folder dialog functionalities
	static bool OpenFolderDialogInner(const void* ParentWindowHandle, const FString& DialogTitle, const FString& DefaultPath, FString& OutFolderName);
};