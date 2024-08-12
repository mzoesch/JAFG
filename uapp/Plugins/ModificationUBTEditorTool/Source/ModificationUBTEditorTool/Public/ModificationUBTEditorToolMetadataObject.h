// Copyright 2024 mzoesch. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "IDetailCustomization.h"
#include "UObject/Object.h"

#include "ModificationUBTEditorToolMetadataObject.generated.h"

struct FPluginDescriptor;
struct FPluginReferenceDescriptor;

USTRUCT(NotBlueprintable)
struct FPluginDependencyDescriptorData final
{
    GENERATED_BODY()

    FPluginDependencyDescriptorData(void);

    /**
     * Reference to another JAFG Game Plugin that this plugin depends on.
     * Do not include Public Dependency Module Names here, as they are not used for dependency resolution within the
     * JAFG Game Plugin ecosystem. Let the Automation Tool handle those dependencies during the build process.
     */
    UPROPERTY(EditAnywhere, Category = "Details")
    FString GamePluginName;

    /** Compatible version range of the JAFG Game Plugin dependency. */
    UPROPERTY(EditAnywhere, Category = "Details")
    FString VersionRange;

    /** All dependencies must be enabled at runtime. */
    UPROPERTY(VisibleAnywhere, Category = "Details")
    bool bEnabled;

    void PopulateFromDescriptor(const FPluginReferenceDescriptor& InDescriptor);
    void CopyIntoDescriptor(FPluginReferenceDescriptor& OutDescriptor) const;
};

UCLASS(NotBlueprintable)
class MODIFICATIONUBTEDITORTOOL_API UModificationUBTEditorToolMetadataObject final : public UObject
{
    GENERATED_BODY()

public:

    explicit UModificationUBTEditorToolMetadataObject(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    void PopulateFromDescriptor(const FPluginDescriptor& InDescriptor);
    void CopyIntoDescriptor(FPluginDescriptor& OutDescriptor) const;

    FString TargetPluginIconPath;

    /**
     * Version of this plugin descriptor file.
     * Note that this really is the descriptor file version, not the plugin version.
     * This is used for backwards compatibility as new features are added to the plugin system, and should usually set
     * to the highest version that is allowed by the engine. The latest version is currently 3, and is the version
     * documented here.
     * We do not expect this version to change very frequently. In the source code, you can look up the
     * EProjectDescriptorVersion namespace and read through the enumerated values to check the actual value.
     * If you require maximum compatibility with older versions of the engine, then you can use an older version of
     * the format, but it is not recommended.
     */
    UPROPERTY(VisibleAnywhere, Category = "Version")
    int32 FileVersion { };

    /**
     * The default Unreal Engine Version field.
     * Current version number of this build of your plugin. This value should always increase with future versions.
     * This version number is not intended to be displayed to end users.
     */
    UPROPERTY(VisibleAnywhere, Category = "Version")
    int32 Version { };

    /**
     * Version of the plugin displayed in the editor.
     * This is not the actual version number as far as the engine and editor are concerned, and is not used for
     * version checks. This can be in whatever format you prefer, although a simple Major.Minor format is preferred.
     * You should always update the VersionName whenever the Version number has increased.
     */
    UPROPERTY(EditAnywhere, Category = "Version")
    FString VersionName;

    /** Version range of the game this plugin was built (or was tested) against. */
    UPROPERTY(EditAnywhere, Category = "Version", DisplayName = "JAFG Version Range")
    FString JAFGVersionRange;

    /** Version number of the engine this plugin was built against. */
    UPROPERTY(VisibleAnywhere, Category = "Version")
    FString EngineVersion;

    /**
     * If this plugin is only needed to be installed on the client. This includes also servers of type "listen server"
     * AND single player games.
     */
    UPROPERTY(EditAnywhere, Category = "Remote Version", meta = (EditCondition = "!bServerOnly"))
    bool bRequiredOnClient { };

    /**
     * If this plugin is only needed to be installed on the server. This includes also servers of type "listen server"
     * AND single player games (as the engine treats singleplayer games like local servers where no other game instance
     * can connect to).
     * If this is true, only the remote clients that connect to an authoritative server will not need to have this
     * plugin installed.
     */
    UPROPERTY(EditAnywhere, Category = "Remote Version", meta = (EditCondition = "!bClientOnly"))
    bool bRequiredOnServer { };

    /**
     * A range of versions (from this plugin only) that this plugin is compatible with.
     * Used if the plugin installed on the server is different from the plugin on a remote client.
     * If a client plugin version is not within this range, the client will not be able to connect to the server and
     * will be forced to disconnect.
     */
    UPROPERTY(EditAnywhere, Category = "Remote Version", meta = (EditCondition = "!bClientOnly && !bServerOnly"))
    FString RemoteVersionRange;

    /**
     * The name of the plugin that will be displayed in the editor. If not specified, the name will default to the
     * .uplugin file name.
     */
    UPROPERTY(EditAnywhere, Category = "Details")
    FString FriendlyName;

    /** The description of the plugin that will be displayed in the editor. */
    UPROPERTY(EditAnywhere, Category = "Details")
    FString Description;

   /**
     * The category of the plugin that will be displayed in the editor. This is used to group plugins together.
     * It is set to DEFAULT_JAFG_GAME_PLUGIN_CATEGORY by default.
    */
    UPROPERTY(VisibleAnywhere, Category = "Details")
    FString Category;

    /**
     * The name of the individual or company that created this plugin.
     * This will be displayed in the editor's plugin browser, or other locations in the UI.
    */
    UPROPERTY(EditAnywhere, Category = "Details")
    FString CreatedBy;

    /**
     * A link to the individual or company that created this plugin.
     * Will be shown on the editor's plugin browser if specified.
     */
    UPROPERTY(EditAnywhere, Category = "Details")
    FString CreatedByURL;

    /**
     * A link to the plugin's documentation. Will be shown on the editor's Plugin browser if specified.
     * An in-game mod list will offer a button to users labeled "Open Documentation" that will open this URL.
     * Can be an internal link to a local file (should be located in the plugin's content directory), or a web URL.
     */
    UPROPERTY(EditAnywhere, Category = "Details")
    FString DocsURL;

    /**
     * A link to the plugin's marketplace page. Will be shown on the editor's Plugin browser if specified.
     * Not used by JAFG Game Plugins.
     */
    UPROPERTY(VisibleAnywhere, Category = "Details")
    FString MarketplaceURL;

    /**
     * A link to the support page for the plugin. Will be shown on the Editor's Plugin browser if specified.
     * An in-game mod list will offer a button to users labeled "Get Support" that will open this URL.
     */
    UPROPERTY(EditAnywhere, Category = "Details")
    FString SupportURL;

    /** A plugin is optional if it can be removed or added at any time without breaking existing world data. */
    UPROPERTY(EditAnywhere, Category = "Behaviour")
    bool bOptional { };

    UPROPERTY(EditAnywhere, Category = "Dependencies")
    TArray<FPluginDependencyDescriptorData> Dependencies;
};

class FModificationUBTEditorToolMetadataCustomization final : public IDetailCustomization
{
public:

    static TSharedRef<IDetailCustomization> MakeInstance(void);

    virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;
};
