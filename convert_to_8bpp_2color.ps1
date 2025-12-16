Add-Type -AssemblyName System.Drawing

function Convert-To8bpp2Color {
    param($srcPath, $dstPath)
    
    $srcImg = $null
    $dstImg = $null
    
    try {
        # Load source
        $srcImg = [System.Drawing.Bitmap]::new($srcPath)
        Write-Host "Converting $srcPath to 8bpp 2-color..."
        
        # Create 8bpp indexed bitmap
        $dstImg = [System.Drawing.Bitmap]::new($srcImg.Width, $srcImg.Height, [System.Drawing.Imaging.PixelFormat]::Format8bppIndexed)
        
        # Setup palette with only 2 colors (black and white)
        $palette = $dstImg.Palette
        $palette.Entries[0] = [System.Drawing.Color]::FromArgb(255, 255, 255, 255) # White (background)
        $palette.Entries[1] = [System.Drawing.Color]::FromArgb(255, 0, 0, 0) # Black (text)
        # Fill rest with duplicates to avoid issues
        for ($i = 2; $i -lt 256; $i++) {
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
                # Read source pixel
                if ($srcImg.PixelFormat -eq [System.Drawing.Imaging.PixelFormat]::Format1bppIndexed) {
                    # For 1bpp, read bit from packed byte
                    $byteIndex = [int]($x / 8)
                    $bitIndex = 7 - ($x % 8)
                    $srcPixel = [IntPtr]::Add($srcRow, $byteIndex)
                    $byteVal = [System.Runtime.InteropServices.Marshal]::ReadByte($srcPixel, 0)
                    $bitVal = ($byteVal -shr $bitIndex) -band 1
                    $pixelValue = $bitVal
                } else {
                    # For 8bpp
                    $srcPixel = [IntPtr]::Add($srcRow, $x)
                    $pixelValue = [System.Runtime.InteropServices.Marshal]::ReadByte($srcPixel, 0)
                    # Threshold to get 0 or 1
                    $pixelValue = if ($pixelValue -gt 127) { 0 } else { 1 }
                }
                
                $dstPixel = [IntPtr]::Add($dstRow, $x)
                [System.Runtime.InteropServices.Marshal]::WriteByte($dstPixel, 0, [byte]$pixelValue)
            }
        }
        
        $srcImg.UnlockBits($srcData)
        $dstImg.UnlockBits($dstData)
        
        # Save
        $dstImg.Save($dstPath, [System.Drawing.Imaging.ImageFormat]::Png)
        Write-Host "Saved 8bpp 2-color image to $dstPath"
        
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

# Convert 1bpp fonts to 8bpp with only 2 colors
$fonts1bpp = @('font0_lat.png', 'font1_lat.png')

foreach ($font in $fonts1bpp) {
    $path = Join-Path $baseDir $font
    $tempPath = [System.IO.Path]::GetTempFileName() + ".png"
    
    Write-Host "`nProcessing $font..."
    Copy-Item $path $tempPath
    Convert-To8bpp2Color $tempPath $path
    Remove-Item $tempPath -ErrorAction SilentlyContinue
}

Write-Host "`nDone! Fonts converted to 8bpp with 2 colors."
