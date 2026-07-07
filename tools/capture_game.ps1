# Launch GeneralsZH, bring it to the foreground, and screenshot the actual
# displayed pixels (CopyFromScreen — works for D3D/bgfx swapchains, unlike
# PrintWindow which returns black for GPU backbuffers). Reusable verify loop.
param(
  [int]$BootWait = 14,     # seconds to wait for boot -> main menu
  [int]$Shots    = 3,      # number of screenshots
  [int]$Interval = 4,      # seconds between shots
  [string]$Tag   = "run"   # filename tag
)
$ErrorActionPreference = 'Continue'
$installDir = 'C:\TheLab\Installs\Command and Conquer Generals Zero Hour'
$exe        = Join-Path $installDir 'GeneralsZH.exe'
$shotDir    = 'C:\TheLab\Development\Generals-Modern\tools\shots'
New-Item -ItemType Directory -Force -Path $shotDir | Out-Null

Get-Process GeneralsZH -ErrorAction SilentlyContinue | Stop-Process -Force -ErrorAction SilentlyContinue

Add-Type @"
using System;
using System.Runtime.InteropServices;
public class W {
  [DllImport("user32.dll")] public static extern bool SetForegroundWindow(IntPtr h);
  [DllImport("user32.dll")] public static extern bool GetWindowRect(IntPtr h, out RECT r);
  [DllImport("user32.dll")] public static extern bool ShowWindow(IntPtr h, int n);
  [StructLayout(LayoutKind.Sequential)] public struct RECT { public int Left, Top, Right, Bottom; }
}
"@
Add-Type -AssemblyName System.Drawing
Add-Type -AssemblyName System.Windows.Forms

$p = Start-Process -FilePath $exe -WorkingDirectory $installDir -PassThru
Write-Output "LAUNCHED pid=$($p.Id), waiting $BootWait s..."
Start-Sleep -Seconds $BootWait

for ($i=1; $i -le $Shots; $i++) {
  $proc = Get-Process -Id $p.Id -ErrorAction SilentlyContinue
  if (-not $proc) { Write-Output "GAME EXITED before shot $i (crash/segfault)"; break }
  $h = $proc.MainWindowHandle
  $rect = New-Object W+RECT
  $haveRect = $false
  if ($h -ne [IntPtr]::Zero) {
    [W]::ShowWindow($h, 9) | Out-Null       # SW_RESTORE
    [W]::SetForegroundWindow($h) | Out-Null
    Start-Sleep -Milliseconds 700
    $haveRect = [W]::GetWindowRect($h, [ref]$rect)
  }
  if ($haveRect -and ($rect.Right - $rect.Left) -gt 0) {
    $wdt = $rect.Right - $rect.Left; $hgt = $rect.Bottom - $rect.Top
    $x = $rect.Left; $y = $rect.Top
  } else {
    $b = [System.Windows.Forms.Screen]::PrimaryScreen.Bounds
    $wdt = $b.Width; $hgt = $b.Height; $x = 0; $y = 0
  }
  $bmp = New-Object System.Drawing.Bitmap($wdt, $hgt)
  $g = [System.Drawing.Graphics]::FromImage($bmp)
  $g.CopyFromScreen($x, $y, 0, 0, $bmp.Size)
  $out = Join-Path $shotDir ("{0}_{1}.png" -f $Tag, $i)
  $bmp.Save($out, [System.Drawing.Imaging.ImageFormat]::Png)
  $g.Dispose(); $bmp.Dispose()
  Write-Output "SHOT $i -> $out  (hwnd=$h ${wdt}x${hgt} @ $x,$y)"
  if ($i -lt $Shots) { Start-Sleep -Seconds $Interval }
}

Get-Process GeneralsZH -ErrorAction SilentlyContinue | Stop-Process -Force -ErrorAction SilentlyContinue
Write-Output "DONE (game terminated)"
