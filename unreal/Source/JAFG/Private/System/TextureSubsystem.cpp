// Copyright 2024 mzoesch. All rights reserved.

#include "System/TextureSubsystem.h"

#include "ImageUtils.h"
#include "System/VoxelSubsystem.h"

UTextureSubsystem::UTextureSubsystem(void) : Super()
{
    /* PIE may not always clean up properly, so we need to do it ourselves. */
    this->ClearCached2DTextures();

    return;
}

void UTextureSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Collection.InitializeDependency<UVoxelSubsystem>();
    Super::Initialize(Collection);

    /* PIE may not always clean up properly, so we need to do it ourselves. */
    this->ClearCached2DTextures();

    this->TexSectionDivider = TCHAR_TO_UTF8(&this->TexSectionDividerChar);

    // Common paths.
    //////////////////////////////////////////////////////////////////////////
    this->GeneratedAssetsDirectoryRelative             = FPaths::ProjectSavedDir() / TEXT("Gen/");
    this->GeneratedAssetsDirectoryAbsolute             = FPaths::ConvertRelativePathToFull(this->GeneratedAssetsDirectoryRelative);

    this->RootAssetsDirectoryRelative                  = FPaths::ProjectContentDir() / "Assets/";
    this->RootAssetsDirectoryAbsolute                  = FPaths::ConvertRelativePathToFull(this->RootAssetsDirectoryRelative);

    this->RootTextureDirectoryRelative                 = this->RootAssetsDirectoryRelative / "Textures/";
    this->RootTextureDirectoryAbsolute                 = this->RootAssetsDirectoryAbsolute / "Textures/";

    this->VoxelTextureDirectoryRelative                = this->RootTextureDirectoryRelative / "Voxels/";
    this->VoxelTextureDirectoryAbsolute                = this->RootTextureDirectoryAbsolute / "Voxels/";

    this->BlendTextureDirectoryRelative                = this->VoxelTextureDirectoryRelative / "Alpha/";
    this->BlendTextureDirectoryAbsolute                = this->VoxelTextureDirectoryAbsolute / "Alpha/";

    // MISC
    //////////////////////////////////////////////////////////////////////////
    this->TextureFailureTextureFileName                = "TextureFailure.png";
    this->TextureFailureTextureFilePathAbsolute        = this->RootTextureDirectoryAbsolute / this->TextureFailureTextureFileName;
    this->TextureFailureHighResTextureFileName         = "TextureFailureHighRes.png";
    this->TextureFailureHighResTextureFilePathAbsolute = this->RootTextureDirectoryAbsolute / this->TextureFailureHighResTextureFileName;

    this->VoxelSubsystem = this->GetGameInstance()->GetSubsystem<UVoxelSubsystem>();
    check( this->VoxelSubsystem )

    this->TextureFailureTextureCacheKey                = "TFT";
    this->TextureFailureHighResTextureCacheKey         = "TFTHR";
    this->WorldTextureCachePrefix                      = "WTC";
    this->BlendTextureCachePrefix                      = "BTC";

    return;
}

void UTextureSubsystem::Deinitialize(void)
{
    Super::Deinitialize();

    /* PIE may not always clean up properly, so we need to do it ourselves. */
    this->ClearCached2DTextures();

    return;
}

FString UTextureSubsystem::CreatePath(const FString& InNameSpace, const ESubNameSpacePaths::Type InType) const
{
    return this->RootTextureDirectoryAbsolute / InNameSpace / LexToString(InType);
}

FString UTextureSubsystem::CreatePathFile(const FString& InNameSpace, const ESubNameSpacePaths::Type InType, const FString& InName) const
{
    return FString::Printf(TEXT("%s.png"), *(this->CreatePath(InNameSpace, InType) / InName));
}

UTexture2D* UTextureSubsystem::GetTexture2D(const FAccumulated& Accumulated)
{
    check( this->VoxelSubsystem )

#if WITH_EDITOR
if (Accumulated.AccumulatedIndex < this->VoxelSubsystem->GetCommonVoxelNum())
{
    LOG_FATAL(LogTextureSubsystem, "Texture requested for a common voxel. This is disallowed. Requested Accumulated: %s.", *Accumulated.ToString())
    return nullptr;
}
#endif /* WITH_EDITOR */

    if (this->Cached2DTextures.Contains(this->VoxelSubsystem->GetVoxelName(Accumulated.AccumulatedIndex)))
    {
        return this->Cached2DTextures[this->VoxelSubsystem->GetVoxelName(Accumulated.AccumulatedIndex)];
    }

    if (UTexture2D* Tex =
        UTextureSubsystem::LoadTexture2DFromDisk(
            FString::Printf(
                TEXT("%s.png"),
                * ( this->GeneratedAssetsDirectoryAbsolute / this->VoxelSubsystem->GetVoxelName(Accumulated.AccumulatedIndex) )
            )
        )
    )
    {
        this->Cached2DTextures.Add(this->VoxelSubsystem->GetVoxelName(Accumulated.AccumulatedIndex), Tex);
        /* Safety net. If everything worked accordingly. */
        return this->GetTexture2D(Accumulated);
    }

    /* Failed to find texture. Returning a placeholder to not immediately crash the game. */

    if (this->Cached2DTextures.Contains(this->TextureFailureTextureCacheKey))
    {
        return this->Cached2DTextures[this->TextureFailureTextureCacheKey];
    }

    if (UTexture2D* Tex = UTextureSubsystem::LoadTexture2DFromDisk(this->TextureFailureTextureFilePathAbsolute))
    {
        this->Cached2DTextures.Add(this->TextureFailureTextureCacheKey, Tex);
        /* Safety net. If everything worked accordingly. */
        return this->GetTexture2D(Accumulated);
    }

    LOG_ERROR(LogTextureSubsystem, "Failed to load texture for accumulated %s and failed to load the placeholder texture.", *Accumulated.ToString())

    return nullptr;
}

TArray<FString> UTextureSubsystem::LoadAllBlendTextureNames(void) const
{
    TArray<FString> BlendTextureNames;

    if (IFileManager& FileManager = IFileManager::Get(); FileManager.DirectoryExists(*this->BlendTextureDirectoryAbsolute))
    {
        FileManager.FindFiles(BlendTextureNames, *this->BlendTextureDirectoryAbsolute, TEXT("png"));
    }
    else
    {
        LOG_FATAL(LogTextureSubsystem, "Failed to find blend texture directory [%s].", *this->BlendTextureDirectoryAbsolute)
    }

    for (FString& BlendTextureName : BlendTextureNames)
    {
        BlendTextureName.RemoveFromEnd(TEXT(".png"));
    }

    LOG_VERBOSE(LogTextureSubsystem, "Found [%d] blend textures.", BlendTextureNames.Num())

    return BlendTextureNames;
}

TArray<FString> UTextureSubsystem::LoadAllDestructionTextureNames(const FString& NameSpace) const
{
    TArray<FString> Out = TArray<FString>();

    if (IFileManager& FileManager = IFileManager::Get(); FileManager.DirectoryExists(*this->CreatePath(NameSpace, ESubNameSpacePaths::Destruction)))
    {
        FileManager.FindFiles(Out, *this->CreatePath(NameSpace, ESubNameSpacePaths::Destruction), TEXT("png"));
    }
    else
    {
        LOG_FATAL(LogTextureSubsystem, "Failed to find destruction texture directory [%s].", *this->CreatePath(NameSpace, ESubNameSpacePaths::Destruction))
        return TArray<FString>();
    }

    for (FString& DestructionTextureName : Out)
    {
        DestructionTextureName.RemoveFromEnd(TEXT(".png"));
    }

    LOG_VERBOSE(LogTextureSubsystem, "Found [%d] destruction textures for namespace [%s].", Out.Num(), *NameSpace)

    return Out;
}

UTexture2D* UTextureSubsystem::GetBlendTexture2D(const FString& BlendName)
{
    const FString Key = FString::Printf(TEXT("%s_%s"), *this->BlendTextureCachePrefix, *BlendName);

    if (this->Cached2DTextures.Contains(Key))
    {
        return this->Cached2DTextures[Key];
    }

    if (UTexture2D* Tex =
        UTextureSubsystem::LoadTexture2DFromDisk(
            FString::Printf(
                TEXT("%s.png"),
                * (this->BlendTextureDirectoryAbsolute / BlendName)
            )
        )
    )
    {
        this->Cached2DTextures.Add(Key, Tex);
        /* Safety net. If everything worked accordingly. */
        return this->GetBlendTexture2D(BlendName);
    }

    /* Failed to find texture. Returning a placeholder to not immediately crash the game. */

    if (this->Cached2DTextures.Contains(this->TextureFailureTextureCacheKey))
    {
        return this->Cached2DTextures[this->TextureFailureTextureCacheKey];
    }

    if (UTexture2D* Tex = UTextureSubsystem::LoadTexture2DFromDisk(this->TextureFailureTextureFilePathAbsolute))
    {
        this->Cached2DTextures.Add(this->TextureFailureTextureCacheKey, Tex);
        /* Safety net. If everything worked accordingly. */
        return this->GetBlendTexture2D(BlendName);
    }

    LOG_ERROR(LogTextureSubsystem, "Failed to load blend texture [%s] and failed to load the placeholder texture.", *BlendName)

    return nullptr;
}

auto UTextureSubsystem::GetSafeBlendTexture2D(const FString& BlendName) -> UTexture2D*
{
    UTexture2D* Texture = this->GetBlendTexture2D(BlendName);
    if (Texture == nullptr)
    {
        LOG_FATAL(LogTextureSubsystem, "Failed to load blend texture: %s", *BlendName)
        return nullptr;
    }

    return Texture;
}

void UTextureSubsystem::LoadTextureNamesForNamespace(const FString& NameSpace)
{
    if (this->HasLoadedTextureNamesForNameSpace(NameSpace))
    {
        LOG_WARNING(LogTextureSubsystem, "World texture names already contains the namespace [%s].", *NameSpace)
        return;
    }

    if (IFileManager& FileManager = IFileManager::Get(); FileManager.DirectoryExists(*(this->VoxelTextureDirectoryAbsolute / NameSpace)))
    {
        TArray<FString> TextureNames;
        FileManager.FindFiles(TextureNames, * (this->VoxelTextureDirectoryAbsolute / NameSpace), TEXT("png"));

        for (FString& TextureName : TextureNames)
        {
            TextureName.RemoveFromEnd(TEXT(".png"));
        }

        this->WorldTextureNames.Add(FPrivateTexNames{NameSpace, TextureNames});

        return;
    }

    LOG_WARNING(LogTextureSubsystem, "Failed to find world texture directory [%s].", *(this->VoxelTextureDirectoryAbsolute / NameSpace))

    return;
}

int32 UTextureSubsystem::GetWorldTexture2DCount(const FString& NameSpace)
{
    if (this->HasLoadedTextureNamesForNameSpace(NameSpace))
    {
        return this->GetWorldTextureNamesForNamespace(NameSpace).Num();
    }

    this->LoadTextureNamesForNamespace(NameSpace);

    return this->GetWorldTexture2DCount(NameSpace);
}

const FString& UTextureSubsystem::GetWorldTexture2DNameByIndex(const FString& NameSpace, const int32 Index)
{
    return this->GetWorldTextureNamesForNamespace(NameSpace)[Index];
}

const TArray<FString>& UTextureSubsystem::GetWorldTextureNamesForNamespace(const FString& InNameSpace)
{
    if (this->HasLoadedTextureNamesForNameSpace(InNameSpace) == false)
    {
        this->LoadTextureNamesForNamespace(InNameSpace);
    }

    for (const auto& [NameSpace, TextureNames] : this->WorldTextureNames)
    {
        if (NameSpace == InNameSpace)
        {
            return TextureNames;
        }
    }

    return this->EmptyStringArray;
}

UTexture2D* UTextureSubsystem::GetWorldTexture2D(const FString& NameSpace, const FString& TextureName)
{
    const FString Key = FString::Printf(TEXT("%s_%s::%s"), *this->WorldTextureCachePrefix, *NameSpace, *TextureName);

    if (this->Cached2DTextures.Contains(Key))
    {
        return this->Cached2DTextures[Key];
    }

    if (UTexture2D* Tex =
        UTextureSubsystem::LoadTexture2DFromDisk(
            FString::Printf(
                TEXT("%s.png"),
                * (this->VoxelTextureDirectoryAbsolute / NameSpace / TextureName)
            )
        )
    )
    {
        this->Cached2DTextures.Add(Key, Tex);
        /* Safety net. If everything worked accordingly. */
        return this->GetWorldTexture2D(NameSpace, TextureName);
    }

    /* Failed to find texture. Returning a placeholder to not immediately crash the game. */

    if (this->Cached2DTextures.Contains(this->TextureFailureTextureCacheKey))
    {
        return this->Cached2DTextures[this->TextureFailureTextureCacheKey];
    }

    if (UTexture2D* Tex = UTextureSubsystem::LoadTexture2DFromDisk(this->TextureFailureTextureFilePathAbsolute))
    {
        this->Cached2DTextures.Add(this->TextureFailureTextureCacheKey, Tex);
        /* Safety net. If everything worked accordingly. */
        return this->GetWorldTexture2D(NameSpace, TextureName);
    }

    LOG_ERROR(LogTextureSubsystem, "Failed to load world texture [%s::%s] and failed to load the placeholder texture.", *NameSpace, *TextureName)

    return nullptr;
}

UTexture2D* UTextureSubsystem::GetWorldDestructionTexture2D(const FString& NameSpace, const FString& TextureName)
{
    const FString Key = FString::Printf(TEXT("%s_%s::%s"), *this->WorldTextureCachePrefix, *NameSpace, *TextureName);

    if (this->Cached2DTextures.Contains(Key))
    {
        return this->Cached2DTextures[Key];
    }

    if (UTexture2D* Tex = UTextureSubsystem::LoadTexture2DFromDisk(
        this->CreatePathFile(NameSpace, ESubNameSpacePaths::Destruction, TextureName)
    ))
    {
        this->Cached2DTextures.Add(Key, Tex);
        /* Safety net. If everything worked accordingly. */
        return this->GetWorldDestructionTexture2D(NameSpace, TextureName);
    }

    /* Failed to find texture. Returning a placeholder to not immediately crash the game. */

    if (this->Cached2DTextures.Contains(this->TextureFailureTextureCacheKey))
    {
        return this->Cached2DTextures[this->TextureFailureTextureCacheKey];
    }

    if (UTexture2D* Tex = UTextureSubsystem::LoadTexture2DFromDisk(this->TextureFailureTextureFilePathAbsolute))
    {
        this->Cached2DTextures.Add(this->TextureFailureTextureCacheKey, Tex);
        /* Safety net. If everything worked accordingly. */
        return this->GetWorldDestructionTexture2D(NameSpace, TextureName);
    }

    LOG_ERROR(LogTextureSubsystem, "Failed to load world texture [%s::%s] and failed to load the placeholder texture.", *NameSpace, *TextureName)

    return nullptr;
}

UTexture2D* UTextureSubsystem::GetSafeWorldDestructionTexture2D(const FString& NameSpace, const FString& TextureName)
{
    UTexture2D* Texture = this->GetWorldDestructionTexture2D(NameSpace, TextureName);
    if (Texture == nullptr)
    {
        LOG_FATAL(LogTextureSubsystem, "Failed to load world destruction texture: %s::%s", *NameSpace, *TextureName)
        return nullptr;
    }

    return Texture;
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

TArray<FString> UTextureSubsystem::SplitTextureName(const FString& TextureName) const
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
    }

    if (Current.Len() > 0)
    {
        Out.Add(Current);
    }

    return Out;
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
