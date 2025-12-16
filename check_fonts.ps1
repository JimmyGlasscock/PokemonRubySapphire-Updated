Add-Type -AssemblyName System.Drawing

function Get-ImageInfo {
    param($path)
    $img = [System.Drawing.Bitmap]::new($path)
    $info = @{
        Width = $img.Width
        Height = $img.Height
        PixelFormat = $img.PixelFormat
        PaletteSize = $img.Palette.Entries.Count
    }
    
    # Count unique colors
    $colors = @{}
    for ($y = 0; $y -lt $img.Height; $y++) {
        for ($x = 0; $x -lt $img.Width; $x++) {
            $pixel = $img.GetPixel($x, $y)
            $key = "$($pixel.R),$($pixel.G),$($pixel.B)"
            $colors[$key] = $true
        }
    }
    $info.UniqueColors = $colors.Count
    
    $img.Dispose()
    return $info
}

Write-Host "=== Original PokeRuby Fonts ===" -ForegroundColor Green
$fonts = @('font0_lat.png', 'font1_lat.png', 'font3_lat.png', 'font4_lat.png')
foreach ($font in $fonts) {
    $path = "graphics\fonts\$font"
    if (Test-Path $path) {
        $info = Get-ImageInfo $path
        Write-Host "`n$font :"
        Write-Host "  Size: $($info.Width)x$($info.Height)"
        Write-Host "  Format: $($info.PixelFormat)"
        Write-Host "  Unique Colors: $($info.UniqueColors)"
    }
}
