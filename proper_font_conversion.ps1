Add-Type -AssemblyName System.Drawing

# Download pokeemerald fonts
$fonts = @{
    'latin_small.png' = @{dest='font0_lat.png'; targetWidth=128; targetHeight=256; colors=2}
    'latin_normal.png' = @{dest='font1_lat.png'; targetWidth=128; targetHeight=128; colors=2}
    'latin_short.png' = @{dest='font3_lat.png'; targetWidth=128; targetHeight=256; colors=3}
    'latin_narrow.png' = @{dest='font4_lat.png'; targetWidth=128; targetHeight=112; colors=3}
}

$tempDir = $env:TEMP
$destDir = 'c:\Users\Jimmy\Documents\decomps\pokeruby\graphics\fonts'

Write-Host "Downloading pokeemerald fonts..." -ForegroundColor Green

foreach ($srcName in $fonts.Keys) {
    $url = "https://raw.githubusercontent.com/pret/pokeemerald/master/graphics/fonts/$srcName"
    $tempPath = Join-Path $tempDir $srcName
    
    Write-Host "`nDownloading $srcName..."
    Invoke-WebRequest -Uri $url -OutFile $tempPath -UseBasicParsing
}

function Convert-Font {
    param($srcPath, $dstPath, $targetWidth, $targetHeight, $numColors)
    
    $srcImg = $null
    $rgbImg = $null
    $resizedImg = $null
    $dstImg = $null
    
    try {
        # Load source
        $srcImg = [System.Drawing.Bitmap]::new($srcPath)
        Write-Host "  Source: $($srcImg.Width)x$($srcImg.Height)"
        
        # Convert to RGB first
        $rgbImg = [System.Drawing.Bitmap]::new($srcImg.Width, $srcImg.Height, [System.Drawing.Imaging.PixelFormat]::Format24bppRgb)
        $g = [System.Drawing.Graphics]::FromImage($rgbImg)
        $g.DrawImage($srcImg, 0, 0)
        $g.Dispose()
        
        # Resize with nearest neighbor
        $resizedImg = [System.Drawing.Bitmap]::new($targetWidth, $targetHeight, [System.Drawing.Imaging.PixelFormat]::Format24bppRgb)
        $g2 = [System.Drawing.Graphics]::FromImage($resizedImg)
        $g2.InterpolationMode = [System.Drawing.Drawing2D.InterpolationMode]::NearestNeighbor
        $g2.PixelOffsetMode = [System.Drawing.Drawing2D.PixelOffsetMode]::Half
        $g2.DrawImage($rgbImg, 0, 0, $targetWidth, $targetHeight)
        $g2.Dispose()
        
        # Convert to 8bpp indexed with limited colors
        $dstImg = [System.Drawing.Bitmap]::new($targetWidth, $targetHeight, [System.Drawing.Imaging.PixelFormat]::Format8bppIndexed)
        
        # Setup palette
        $palette = $dstImg.Palette
        if ($numColors -eq 2) {
            $palette.Entries[0] = [System.Drawing.Color]::FromArgb(255, 248, 248, 248) # Near-white
            $palette.Entries[1] = [System.Drawing.Color]::FromArgb(255, 0, 0, 0) # Black
            for ($i = 2; $i -lt 256; $i++) {
                $palette.Entries[$i] = $palette.Entries[0]
            }
        } else {
            $palette.Entries[0] = [System.Drawing.Color]::FromArgb(255, 248, 248, 248) # Near-white
            $palette.Entries[1] = [System.Drawing.Color]::FromArgb(255, 144, 144, 144) # Gray
            $palette.Entries[2] = [System.Drawing.Color]::FromArgb(255, 0, 0, 0) # Black
            for ($i = 3; $i -lt 256; $i++) {
                $palette.Entries[$i] = $palette.Entries[0]
            }
        }
        $dstImg.Palette = $palette
        
        # Convert pixels
        $rect = [System.Drawing.Rectangle]::new(0, 0, $targetWidth, $targetHeight)
        $srcData = $resizedImg.LockBits($rect, [System.Drawing.Imaging.ImageLockMode]::ReadOnly, [System.Drawing.Imaging.PixelFormat]::Format24bppRgb)
        $dstData = $dstImg.LockBits($rect, [System.Drawing.Imaging.ImageLockMode]::WriteOnly, [System.Drawing.Imaging.PixelFormat]::Format8bppIndexed)
        
        for ($y = 0; $y -lt $targetHeight; $y++) {
            for ($x = 0; $x -lt $targetWidth; $x++) {
                $srcPixel = [IntPtr]::Add($srcData.Scan0, $y * $srcData.Stride + $x * 3)
                $b = [System.Runtime.InteropServices.Marshal]::ReadByte($srcPixel, 0)
                $g = [System.Runtime.InteropServices.Marshal]::ReadByte($srcPixel, 1)
                $r = [System.Runtime.InteropServices.Marshal]::ReadByte($srcPixel, 2)
                $gray = [int](($r + $g + $b) / 3)
                
                # Quantize
                if ($numColors -eq 2) {
                    $colorIdx = if ($gray -gt 127) { 0 } else { 1 }
                } else {
                    if ($gray -gt 200) {
                        $colorIdx = 0  # White
                    } elseif ($gray -gt 100) {
                        $colorIdx = 1  # Gray
                    } else {
                        $colorIdx = 2  # Black
                    }
                }
                
                $dstPixel = [IntPtr]::Add($dstData.Scan0, $y * $dstData.Stride + $x)
                [System.Runtime.InteropServices.Marshal]::WriteByte($dstPixel, 0, [byte]$colorIdx)
            }
        }
        
        $resizedImg.UnlockBits($srcData)
        $dstImg.UnlockBits($dstData)
        
        $dstImg.Save($dstPath, [System.Drawing.Imaging.ImageFormat]::Png)
        Write-Host "  Saved: $targetWidth x $targetHeight, $numColors colors" -ForegroundColor Green
        
        $resizedImg.Dispose()
        return $true
    }
    catch {
        Write-Host "  Error: $_" -ForegroundColor Red
        return $false
    }
    finally {
        if ($srcImg) { $srcImg.Dispose() }
        if ($rgbImg) { $rgbImg.Dispose() }
        if ($resizedImg) { $resizedImg.Dispose() }
        if ($dstImg) { $dstImg.Dispose() }
    }
}

Write-Host "`n`nConverting fonts..." -ForegroundColor Green

foreach ($srcName in $fonts.Keys) {
    $config = $fonts[$srcName]
    $srcPath = Join-Path $tempDir $srcName
    $dstPath = Join-Path $destDir $config.dest
    
    Write-Host "`n$srcName -> $($config.dest)"
    Convert-Font $srcPath $dstPath $config.targetWidth $config.targetHeight $config.colors
}

Write-Host "`n`nDone! All fonts converted." -ForegroundColor Green
