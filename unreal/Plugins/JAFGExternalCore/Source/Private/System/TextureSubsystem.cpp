// Copyright 2024 mzoesch. All rights reserved.

#include "System/TextureSubsystem.h"

#include "ImageUtils.h"
#include "System/VoxelSubsystem.h"

#define MAKE_CACHE_KEY(Prefix, Name) \
    FString::Printf(TEXT("%s%s"), *Prefix, *Name)
#define DECLARE_CACHE_KEY(Prefix, Name) \
    const FString CacheKey = MAKE_CACHE_KEY(Prefix, Name);

UTextureSubsystem::UTextureSubsystem(void) : Super()
{
#if WITH_EDITOR
    /* PIE may not always clean up properly, so we need to do it ourselves. */
    this->ClearCached2DTextures();
#endif /* WITH_EDITOR */

    return;
}

void UTextureSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Collection.InitializeDependency<UVoxelSubsystem>();
    Super::Initialize(Collection);

#if WITH_EDITOR
    /* PIE may not always clean up properly, so we need to do it ourselves. */
    this->ClearCached2DTextures();
#endif /* WITH_EDITOR */


    this->GeneratedAssetsDirectoryRelative             = FPaths::ProjectSavedDir() / TEXT("gen/");
    this->GeneratedAssetsDirectoryAbsolute             = FPaths::ConvertRelativePathToFull(this->GeneratedAssetsDirectoryRelative);
    this->RootTextureDirectoryRelative                 = FPaths::ProjectContentDir() / "Assets/" / "Textures/";
    this->RootTextureDirectoryAbsolute                 = FPaths::ConvertRelativePathToFull(this->RootTextureDirectoryRelative);
    this->TextureFailureTextureFilePathAbsolute        = this->RootTextureDirectoryAbsolute / this->TextureFailureTextureFileName;
    this->TextureFailureHighResTextureFilePathAbsolute = this->RootTextureDirectoryAbsolute / this->TextureFailureHighResTextureFileName;

    this->VoxelSubsystem = this->GetGameInstance()->GetSubsystem<UVoxelSubsystem>(); jcheck( this->VoxelSubsystem )

    this->UsedTextureNamespaces.Empty();
    this->UsedTextureNamespaces.Add("JAFG");

    return;
}

void UTextureSubsystem::Deinitialize(void)
{
    Super::Deinitialize();

    /* PIE may not always clean up properly, so we need to do it ourselves. */
    this->ClearCached2DTextures();

    return;
}

bool UTextureSubsystem::CreatePathToFile(const FString& InNamespace, const ESubNameSpacePaths::Type InType, const FString& InFileName, FString& OutAbsolutePath) const
{
    OutAbsolutePath =
        FString::Printf(TEXT("%s.%s"),
        * ( this->RootTextureDirectoryAbsolute / InNamespace / LexToString(InType) / InFileName ),
        * this->FileExtension
    );

    return IFileManager::Get().FileExists(*OutAbsolutePath);
}

bool UTextureSubsystem::CreatePathToFile(const ESubNameSpacePaths::Type InType, const FString& InFileName, FString& OutAbsolutePath) const
{
    if (InType == ESubNameSpacePaths::Generated)
    {
        OutAbsolutePath = FString::Printf(TEXT("%s.%s"), *(this->GeneratedAssetsDirectoryAbsolute / InFileName), *this->FileExtension);
        return IFileManager::Get().FileExists(*OutAbsolutePath);
    }

    for (const FString& NameSpace : this->UsedTextureNamespaces)
    {
        if (this->CreatePathToFile(NameSpace, InType, InFileName, OutAbsolutePath))
        {
            return true;
        }

        continue;
    }

    return false;
}

FString UTextureSubsystem::GetFirstNamespaceForFile(const ESubNameSpacePaths::Type InType, const FString& InFileName) const
{
    for (const FString& NameSpace : this->UsedTextureNamespaces)
    {
        if (FString Path; this->CreatePathToFile(NameSpace, InType, InFileName, Path))
        {
            return NameSpace;
        }

        continue;
    }

    return TEXT("???");
}

bool UTextureSubsystem::CreatePathToDirectory(const FString& InNamespace, const ESubNameSpacePaths::Type InType, FString& OutAbsolutePath) const
{
    OutAbsolutePath = this->RootTextureDirectoryAbsolute / InNamespace / LexToString(InType);
    return IFileManager::Get().DirectoryExists(*OutAbsolutePath);
}

UTexture2D* UTextureSubsystem::GetGUITexture2D(const FString& TextureName)
{
    FString Path;
    if (this->CreatePathToFile(ESubNameSpacePaths::GUI, TextureName, Path) == false)
    {
        return nullptr;
    }

    return this->GetAndCacheTexture2D(TextureName, Path);
}

UTexture2D* UTextureSubsystem::GetSafeGUITexture2D(const FString& TextureName)
{
    if (UTexture2D* Texture = this->GetGUITexture2D(TextureName))
    {
        return Texture;
    }

    LOG_FATAL(LogTextureSubsystem, "Failed to load GUI texture: %s.", *TextureName);

    return nullptr;
}

UTexture2D* UTextureSubsystem::GetPreviewTexture2D(const voxel_t AccumulatedIndex)
{
    if (AccumulatedIndex < ECommonVoxels::Num)
    {
        return nullptr;
    }

    DECLARE_CACHE_KEY(this->PreviewCachePrefix, this->VoxelSubsystem->GetVoxelName(AccumulatedIndex))
    if (UTexture2D* CachedTexture = this->Cached2DTextures.FindRef(CacheKey); CachedTexture != nullptr)
    {
        return CachedTexture;
    }

    FString Path;
    if (this->CreatePathToFile(ESubNameSpacePaths::Generated, this->VoxelSubsystem->GetVoxelName(AccumulatedIndex), Path) == false)
    {
        LOG_WARNING(
            LogTextureSubsystem,
            "Failed to load preview texture for accumulated: %s. Expected path: %s",
            *this->VoxelSubsystem->GetVoxelName(AccumulatedIndex), *Path
        )
        return this->GetAndCacheTexture2D(this->TextureFailureTextureFileName, this->TextureFailureTextureFilePathAbsolute);
    }

    return this->GetAndCacheTexture2D(CacheKey, Path);
}

UTexture2D* UTextureSubsystem::GetSafePreviewTexture2D(const voxel_t AccumulatedIndex)
{
    if (UTexture2D* Texture = this->GetPreviewTexture2D(AccumulatedIndex))
    {
        return Texture;
    }

    LOG_FATAL(LogTextureSubsystem, "Failed to load preview texture for accumulated: %s.", *this->VoxelSubsystem->GetVoxelName(AccumulatedIndex))

    return nullptr;
}

UTexture2D* UTextureSubsystem::GetVoxelTexture2D(const FString& TextureName)
{
    if (UTexture2D* CachedTexture = this->Cached2DTextures.FindRef(TextureName); CachedTexture != nullptr)
    {
        return CachedTexture;
    }

    FString Path;
    if (this->CreatePathToFile(ESubNameSpacePaths::Voxels, TextureName, Path) == false)
    {
        return nullptr;
    }

    return this->GetAndCacheTexture2D(TextureName, Path);
}

UTexture2D* UTextureSubsystem::GetSafeVoxelTexture2D(const FString& TextureName)
{
    if (UTexture2D* Texture = this->GetVoxelTexture2D(TextureName))
    {
        return Texture;
    }

    LOG_FATAL(LogTextureSubsystem, "Failed to load voxel texture: %s.", *TextureName)

    return nullptr;
}

TArray<FString> UTextureSubsystem::LoadAllBlendTextureNames(void) const
{
    TArray<FString> BlendTextureNames = TArray<FString>();

    for (const FString& Namespace : this->UsedTextureNamespaces)
    {
        FString Path;
        if (this->CreatePathToDirectory(Namespace, ESubNameSpacePaths::Blends, Path) == false)
        {
            continue;
        }

        TArray<FString> CurrentBlendTextureNames = TArray<FString>();
        IFileManager::Get().FindFiles(CurrentBlendTextureNames, *Path, *this->FileExtension);

        for (FString& BlendTextureName : CurrentBlendTextureNames)
        {
            BlendTextureName.RemoveFromEnd(FString::Printf(TEXT(".%s"), *this->FileExtension));
            BlendTextureNames.AddUnique(BlendTextureName);
        }

        continue;
    }

    LOG_VERBOSE(LogTextureSubsystem, "Found [%d] blend textures.", BlendTextureNames.Num())

    return BlendTextureNames;
}

UTexture2D* UTextureSubsystem::GetBlendTexture2D(const FString& BlendName)
{
    FString Path;
    if (this->CreatePathToFile(ESubNameSpacePaths::Blends, BlendName, Path) == false)
    {
        return nullptr;
    }

    return this->GetAndCacheTexture2D(BlendName, Path);
}

UTexture2D* UTextureSubsystem::GetSafeBlendTexture2D(const FString& BlendName)
{
    if (UTexture2D* Texture = this->GetBlendTexture2D(BlendName))
    {
        return Texture;
    }

    LOG_FATAL(LogTextureSubsystem, "Failed to load blend texture: %s.", *BlendName)

    return nullptr;
}

TArray<FString> UTextureSubsystem::LoadAllDestructionTextureNames(void) const
{
    TArray<FString> Out = TArray<FString>();

    for (const FString& Namespace : this->UsedTextureNamespaces)
    {
        FString Path;
        if (this->CreatePathToDirectory(Namespace, ESubNameSpacePaths::Destruction, Path) == false)
        {
            continue;
        }

        IFileManager::Get().FindFiles(Out, *Path, *this->FileExtension);

        for (FString& DestructionTextureName : Out)
        {
            DestructionTextureName.RemoveFromEnd(FString::Printf(TEXT(".%s"), *this->FileExtension));
        }

        if (Out.Num() > 0)
        {
            break;
        }

        continue;
    }

    LOG_VERBOSE(LogTextureSubsystem, "Found [%d] destruction textures.", Out.Num())

    return Out;
}

UTexture2D* UTextureSubsystem::GetDestructionTexture2D(const FString& TextureName)
{
    FString Path;
    if (this->CreatePathToFile(ESubNameSpacePaths::Destruction, TextureName, Path) == false)
    {
        return nullptr;
    }

    return this->GetAndCacheTexture2D(TextureName, Path);
}

UTexture2D* UTextureSubsystem::GetSafeDestructionTexture2D(const FString& TextureName)
{
    if (UTexture2D* Texture = this->GetDestructionTexture2D(TextureName))
    {
        return Texture;
    }

    LOG_FATAL(LogTextureSubsystem, "Failed to load destruction texture: %s.", *TextureName)

    return nullptr;
}

TArray<FString> UTextureSubsystem::LoadAllVoxelTextureNamesForNamespace(const FString& Namespace) const
{
    FString AbsolutePath;
    if (this->CreatePathToDirectory(Namespace, ESubNameSpacePaths::Voxels, AbsolutePath) == false)
    {
        return TArray<FString>();
    }

    IFileManager& FileManager = IFileManager::Get();
    TArray<FString> Out = TArray<FString>();

    FileManager.FindFiles(Out, *AbsolutePath, *this->FileExtension);

    for (FString& VoxelTextureName : Out)
    {
        VoxelTextureName.RemoveFromEnd(FString::Printf(TEXT(".%s"), *this->FileExtension));
    }

    return Out;
}

TArray<FString> UTextureSubsystem::SplitVoxelTextureName(const FString& TextureName) const
{
    TArray<FString> Out;

    FString Current = "";
    for (const TCHAR& Char : TextureName)
    {
        if (Char == this->TexSectionDividerChar)
        {
            Out.Add(Current);
            Current = "";
        }
        else
        {
            Current.AppendChar(Char);
        }

        continue;
    }

    if (Current.Len() > 0)
    {
        Out.Add(Current);
    }

    return Out;
}

int64 UTextureSubsystem::GetBytesPerPixel(const ERawImageFormat::Type Format)
{
    /*
     * Shamelessly copied from IMAGECORE_API ERawImageFormat::GetBytesPerPixel(ERawImageFormat::Type).
     *
     * This method is rewritten because of a linker error. If this can be resolved, the original method should be used.
     *
     * error LNK2019: unresolved external symbol
     *      "__declspec(dllimport) public: void __cdecl FImage::Init(int,int,enum ERawImageFormat::Type,enum EGammaSpace)"
     *      (__imp_?Init@FImage@@QEAAXHHW4Type@ERawImageFormat@@W4EGammaSpace@@@Z) referenced in function "public: virtual void __cdecl UTextureSubsystem::MyMethod(void)" (?MyMethod@UTextureSubsystem@@UEAAXM@Z)
     *      [...]\JAFG\unreal\Binaries\Win64\UnrealEditor-JAFG.patch_YXZ.exe : fatal error LNK1120: 1 unresolved externals
     */

    int64 OutBytesPerPixel = 0;

    switch (Format)
    {

    case ERawImageFormat::G8:
    {
        OutBytesPerPixel = 1;
        break;
    }

    case ERawImageFormat::G16:
    case ERawImageFormat::R16F:
    {
        OutBytesPerPixel = 2;
        break;
    }

    case ERawImageFormat::R32F:
    case ERawImageFormat::BGRA8:
    case ERawImageFormat::BGRE8:
    {
        OutBytesPerPixel = 4;
        break;
    }

    case ERawImageFormat::RGBA16:
    case ERawImageFormat::RGBA16F:
    {
        OutBytesPerPixel = 8;
        break;
    }

    case ERawImageFormat::RGBA32F:
    {
        OutBytesPerPixel = 16;
        break;
    }

    default:
    {
        checkNoEntry()
        break;
    }

    }

    return OutBytesPerPixel;
}

UTexture2D* UTextureSubsystem::GetAndCacheTexture2D(const FString& CacheKey, const FString& FallbackAbsoluteFilePath)
{
    if (UTexture2D* CachedTexture = this->Cached2DTextures.FindRef(CacheKey); CachedTexture != nullptr)
    {
        return CachedTexture;
    }

    if (UTexture2D* Tex = UTextureSubsystem::LoadTexture2DFromDisk(FallbackAbsoluteFilePath))
    {
        this->Cached2DTextures.Add(CacheKey, Tex);
        return Tex;
    }

    if (this->Cached2DTextures.Contains(this->TextureFailureTextureFileName))
    {
        return this->Cached2DTextures[this->TextureFailureTextureFileName];
    }

    if (UTexture2D* Tex = UTextureSubsystem::LoadTexture2DFromDisk(this->TextureFailureTextureFilePathAbsolute))
    {
        this->Cached2DTextures.Add(this->TextureFailureTextureFileName, Tex);
        return Tex;
    }

    LOG_FATAL(LogTextureSubsystem, "Failed to load and cache texture for [%s] and failed to load the placeholder texture.", *CacheKey)

    return nullptr;
}

UTexture2D* UTextureSubsystem::LoadTexture2DFromDisk(const FString& AbsolutePath)
{
    LOG_VERBOSE(LogTextureSubsystem, "Loading texture from disk: %s.", *AbsolutePath)

    if (!FPaths::FileExists(AbsolutePath))
    {
        LOG_ERROR(LogTextureSubsystem, "File does not exist: %s.", *AbsolutePath)
        return nullptr;
    }

    if (AbsolutePath.Len() <= 0)
    {
        LOG_ERROR(LogTextureSubsystem, "File path is empty.")
        return nullptr;
    }

    FString NormalizedPath = AbsolutePath;

    NormalizedPath.ReplaceInline(TEXT("\\"), TEXT("/"), ESearchCase::CaseSensitive);
    NormalizedPath.ReplaceInline(TEXT("//"), TEXT("/"), ESearchCase::CaseSensitive);
    NormalizedPath.RemoveFromStart(TEXT("/"));
    NormalizedPath.RemoveFromEnd(TEXT("/"));
    FPlatformMisc::NormalizePath(NormalizedPath);

    UTexture2D* Tex          = FImageUtils::ImportFileAsTexture2D(NormalizedPath);

    /* We always want sharp textures and not blur them as we are working with pixel art. */
    Tex->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;
    Tex->SRGB                = false;
    Tex->Filter              = TextureFilter::TF_Nearest;
    Tex->LODGroup            = TextureGroup::TEXTUREGROUP_Pixels2D;

    return Tex;
}
