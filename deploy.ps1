

param(
    $SteamVRPath = "C:\Program Files (x86)\Steam\steamapps\common\SteamVR",
    $Config      = "Release"
)

$driverName = "PepperVR"
$driverDest = "$SteamVRPath\drivers\$driverName\bin\win64"
$resDest    = "$SteamVRPath\drivers\$driverName"


New-Item -ItemType Directory -Force -Path $driverDest | Out-Null
New-Item -ItemType Directory -Force -Path "$resDest\resources\settings" | Out-Null


$dll = "bin\$Config\driver_fakevr.dll"
if (Test-Path $dll) {
    Copy-Item $dll "$driverDest\driver_fakevr.dll" -Force
    Write-Host "Copied DLL -> $driverDest"
} else {
    Write-Error "DLL not found at $dll — build the project"
    exit 1
}


Copy-Item "driver_files\driver.vrdrivermanifest" "$resDest\driver.vrdrivermanifest" -Force
Copy-Item "driver_files\resources\settings\default.vrsettings" `
          "$resDest\resources\settings\default.vrsettings" -Force

Write-Host ""
Write-Host "Deployment completed"
Write-Host "Driver installed at: $resDest"
Write-Host ""
Write-Host "Next steps:"
Write-Host "  1. Launch SteamVR"
Write-Host "  2. Check SteamVR > Developer > Driver Status for 'PepperVR'"
Write-Host "  3. Move the SteamVR window into focus for mouse capture"
