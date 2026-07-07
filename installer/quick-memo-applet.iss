#define AppName "Quick Memo Applet"
#define AppPublisher "MOS"
#define AppExeName "Quick_Memo_Applet.exe"

#ifndef AppVersion
#define AppVersion "1.0.0"
#endif

#ifndef SourceDir
#define SourceDir "..\build\release-package\QuickMemoApplet-v1.0.0-win64"
#endif

#ifndef OutputDir
#define OutputDir "..\build\release-package"
#endif

[Setup]
AppId={{BD4D3BE5-9D74-44B7-94FD-A6B1B22997F8}
AppName={#AppName}
AppVersion={#AppVersion}
AppPublisher={#AppPublisher}
DefaultDirName={localappdata}\Programs\QuickMemoApplet
DefaultGroupName={#AppName}
DisableProgramGroupPage=yes
PrivilegesRequired=lowest
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible
SetupIconFile=..\resources\app.ico
UninstallDisplayIcon={app}\{#AppExeName}
LicenseFile=..\LICENSE
OutputDir={#OutputDir}
OutputBaseFilename=QuickMemoApplet-v{#AppVersion}-win64-setup
Compression=lzma
SolidCompression=yes
WizardStyle=modern

[Languages]
Name: "default"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "创建桌面快捷方式"; GroupDescription: "附加任务："; Flags: unchecked

[Files]
Source: "{#SourceDir}\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs

[Icons]
Name: "{group}\{#AppName}"; Filename: "{app}\{#AppExeName}"
Name: "{userdesktop}\{#AppName}"; Filename: "{app}\{#AppExeName}"; Tasks: desktopicon

[Run]
Filename: "{app}\{#AppExeName}"; Description: "启动 {#AppName}"; Flags: nowait postinstall skipifsilent
