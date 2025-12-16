Add-Type -AssemblyName System.Drawing

function Convert-To8bpp3Color {
    param($srcPath, $dstPath)
    
    $srcImg = $null
    $dstImg = $null
    
    try {
        # Load source
        $srcImg = [System.Drawing.Bitmap]::new($srcPath)
        Write-Host "Converting $srcPath to 8bpp 3-color..."
        
        # Create 8bpp indexed bitmap
        $dstImg = [System.Drawing.Bitmap]::new($srcImg.Width, $srcImg.Height, [System.Drawing.Imaging.PixelFormat]::Format8bppIndexed)
        
        # Setup palette with only 3 colors for 4bpp fonts
        $palette = $dstImg.Palette
        $palette.Entries[0] = [System.Drawing.Color]::FromArgb(255, 255, 255, 255) # White (background)
        $palette.Entries[1] = [System.Drawing.Color]::FromArgb(255, 128, 128, 128) # Gray (shadow)
        $palette.Entries[2] = [System.Drawing.Color]::FromArgb(255, 0, 0, 0) # Black (text)
        # Fill rest with background color
        for ($i = 3; $i -lt 256; $i++) {
            $palette.Entries[$i] = [System.Drawing.Color]::FromArgb(255, 255, 255, 255)
        }
        $dstImg.Palette = $palette
        
        # Lock bits
        $rect = [System.Drawing.Rectangle]::new(0, 0, $srcImg.Width, $srcImg.Height)
        $srcData = $srcImg.LockBits($rect, [System.Drawing.Imaging.ImageLockMode]::ReadOnly, $srcImg.PixelFormat)
        $dstData = $dstImg.LockBits($rect, [System.Drawing.Imaging.ImageLockMode]::WriteOnly, [System.Drawing.Imaging.PixelFormat]::Format8bppIndexed)
        
        $height = $srcImg.Height
        $width = $srcImg.Width
        
        for ($y = 0; $y -lt $height; $y++) {
            $srcRow = [IntPtr]::Add($srcData.Scan0, $y * $srcData.Stride)
            $dstRow = [IntPtr]::Add($dstData.Scan0, $y * $dstData.Stride)
            
            for ($x = 0; $x -lt $width; $x++) {
                $srcPixel = [IntPtr]::Add($srcRow, $x)
                $pixelValue = [System.Runtime.InteropServices.Marshal]::ReadByte($srcPixel, 0)
                
                # Quantize to 3 colors
                # 0-84: black (2)
                # 85-170: gray (1)
                # 171-255: white (0)
                $colorIndex = 0
                if ($pixelValue -lt 85) {
                    $colorIndex = 2  # Black
                } elseif ($pixelValue -lt 171) {
                    $colorIndex = 1  # Gray
                } else {
                    $colorIndex = 0  # White
                }
                
                $dstPixel = [IntPtr]::Add($dstRow, $x)
                [System.Runtime.InteropServices.Marshal]::WriteByte($dstPixel, 0, [byte]$colorIndex)
            }
        }
        
        $srcImg.UnlockBits($srcData)
        $dstImg.UnlockBits($dstData)
        
        # Save
        $dstImg.Save($dstPath, [System.Drawing.Imaging.ImageFormat]::Png)
        Write-Host "Saved 8bpp 3-color image to $dstPath"
        
        return $true
    }
    catch {
        Write-Host "Error: $_"
        Write-Host $_.Exception.StackTrace
        return $false
    }
    finally {
        if ($srcImg) { $srcImg.Dispose() }
        if ($dstImg) { $dstImg.Dispose() }
    }
}

$baseDir = 'c:\Users\Jimmy\Documents\decomps\pokeruby\graphics\fonts'

# Convert 4bpp fonts to 8bpp with only 3 colors
$fonts4bpp = @('font3_lat.png', 'font4_lat.png')

foreach ($font in $fonts4bpp) {
    $path = Join-Path $baseDir $font
    $tempPath = [System.IO.Path]::GetTempFileName() + ".png"
    
    Write-Host "`nProcessing $font..."
    Copy-Item $path $tempPath
    Convert-To8bpp3Color $tempPath $path
    Remove-Item $tempPath -ErrorAction SilentlyContinue
}

Write-Host "`nDone! 4bpp fonts converted to 8bpp with 3 colors."
