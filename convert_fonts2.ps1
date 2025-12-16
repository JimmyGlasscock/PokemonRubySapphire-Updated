Add-Type -AssemblyName System.Drawing

function Convert-ToGrayscale8bit {
    param($srcPath, $dstPath)
    
    $srcImg = $null
    $rgbBitmap = $null
    $grayBitmap = $null
    
    try {
        # Load source
        $srcImg = [System.Drawing.Bitmap]::new($srcPath)
        Write-Host "Loaded $srcPath - Size: $($srcImg.Width)x$($srcImg.Height), Format: $($srcImg.PixelFormat)"
        
        # Convert to 24bpp RGB first
        $rgbBitmap = [System.Drawing.Bitmap]::new($srcImg.Width, $srcImg.Height, [System.Drawing.Imaging.PixelFormat]::Format24bppRgb)
        $g = [System.Drawing.Graphics]::FromImage($rgbBitmap)
        $g.DrawImage($srcImg, 0, 0)
        $g.Dispose()
        
        # Save as 24bpp temporarily then reload and convert
        $tempPath = [System.IO.Path]::GetTempFileName() + ".png"
        $rgbBitmap.Save($tempPath, [System.Drawing.Imaging.ImageFormat]::Png)
        $rgbBitmap.Dispose()
        
        # Reload
        $rgbBitmap = [System.Drawing.Bitmap]::new($tempPath)
        
        # Create 8bpp indexed grayscale
        $grayBitmap = [System.Drawing.Bitmap]::new($rgbBitmap.Width, $rgbBitmap.Height, [System.Drawing.Imaging.PixelFormat]::Format8bppIndexed)
        
        # Setup grayscale palette
        $palette = $grayBitmap.Palette
        for ($i = 0; $i -lt 256; $i++) {
            $palette.Entries[$i] = [System.Drawing.Color]::FromArgb(255, $i, $i, $i)
        }
        $grayBitmap.Palette = $palette
        
        # Lock bits
        $rect = [System.Drawing.Rectangle]::new(0, 0, $rgbBitmap.Width, $rgbBitmap.Height)
        $srcData = $rgbBitmap.LockBits($rect, [System.Drawing.Imaging.ImageLockMode]::ReadOnly, [System.Drawing.Imaging.PixelFormat]::Format24bppRgb)
        $dstData = $grayBitmap.LockBits($rect, [System.Drawing.Imaging.ImageLockMode]::WriteOnly, [System.Drawing.Imaging.PixelFormat]::Format8bppIndexed)
        
        # Copy and convert
        $height = $rgbBitmap.Height
        $width = $rgbBitmap.Width
        
        $srcStride = $srcData.Stride
        $dstStride = $dstData.Stride
        
        for ($y = 0; $y -lt $height; $y++) {
            $srcRow = [IntPtr]::Add($srcData.Scan0, $y * $srcStride)
            $dstRow = [IntPtr]::Add($dstData.Scan0, $y * $dstStride)
            
            for ($x = 0; $x -lt $width; $x++) {
                $srcPixel = [IntPtr]::Add($srcRow, $x * 3)
                $dstPixel = [IntPtr]::Add($dstRow, $x)
                
                $b = [System.Runtime.InteropServices.Marshal]::ReadByte($srcPixel, 0)
                $g = [System.Runtime.InteropServices.Marshal]::ReadByte($srcPixel, 1)
                $r = [System.Runtime.InteropServices.Marshal]::ReadByte($srcPixel, 2)
                
                $gray = [byte](($r + $g + $b) / 3)
                [System.Runtime.InteropServices.Marshal]::WriteByte($dstPixel, 0, $gray)
            }
        }
        
        $rgbBitmap.UnlockBits($srcData)
        $grayBitmap.UnlockBits($dstData)
        
        # Save
        $grayBitmap.Save($dstPath, [System.Drawing.Imaging.ImageFormat]::Png)
        Write-Host "Saved to $dstPath"
        
        # Cleanup
        Remove-Item $tempPath -ErrorAction SilentlyContinue
        
        return $true
    }
    catch {
        Write-Host "Error: $_"
        return $false
    }
    finally {
        if ($srcImg) { $srcImg.Dispose() }
        if ($rgbBitmap) { $rgbBitmap.Dispose() }
        if ($grayBitmap) { $grayBitmap.Dispose() }
    }
}

$fonts = @(
    @{src='emerald_latin_small.png'; dst='font0_lat.png'},
    @{src='emerald_latin_normal.png'; dst='font1_lat.png'},
    @{src='emerald_latin_short.png'; dst='font3_lat.png'},
    @{src='emerald_latin_narrow.png'; dst='font4_lat.png'}
)

foreach ($font in $fonts) {
    $srcPath = Join-Path $env:TEMP $font.src
    $dstPath = Join-Path 'c:\Users\Jimmy\Documents\decomps\pokeruby\graphics\fonts' $font.dst
    
    Write-Host "`nConverting $($font.src)..."
    Convert-ToGrayscale8bit $srcPath $dstPath
}
