Add-Type -AssemblyName System.Drawing

function Convert-To2Color {
    param($srcPath, $dstPath)
    
    $srcImg = $null
    $dstImg = $null
    
    try {
        # Load source
        $srcImg = [System.Drawing.Bitmap]::new($srcPath)
        Write-Host "Converting $srcPath to 2-color..."
        
        # Create 1bpp indexed bitmap (2 colors)
        $dstImg = [System.Drawing.Bitmap]::new($srcImg.Width, $srcImg.Height, [System.Drawing.Imaging.PixelFormat]::Format1bppIndexed)
        
        # Setup black and white palette
        $palette = $dstImg.Palette
        $palette.Entries[0] = [System.Drawing.Color]::FromArgb(255, 255, 255, 255) # White
        $palette.Entries[1] = [System.Drawing.Color]::FromArgb(255, 0, 0, 0) # Black
        $dstImg.Palette = $palette
        
        # Lock bits for manual conversion
        $rect = [System.Drawing.Rectangle]::new(0, 0, $srcImg.Width, $srcImg.Height)
        $srcData = $srcImg.LockBits($rect, [System.Drawing.Imaging.ImageLockMode]::ReadOnly, $srcImg.PixelFormat)
        $dstData = $dstImg.LockBits($rect, [System.Drawing.Imaging.ImageLockMode]::WriteOnly, [System.Drawing.Imaging.PixelFormat]::Format1bppIndexed)
        
        $height = $srcImg.Height
        $width = $srcImg.Width
        
        # Process each row
        for ($y = 0; $y -lt $height; $y++) {
            $srcRow = [IntPtr]::Add($srcData.Scan0, $y * $srcData.Stride)
            $dstRow = [IntPtr]::Add($dstData.Scan0, $y * $dstData.Stride)
            
            # For 1bpp, 8 pixels are packed into 1 byte
            for ($x = 0; $x -lt $width; $x += 8) {
                $byteVal = 0
                
                for ($bit = 0; $bit -lt 8 -and ($x + $bit) -lt $width; $bit++) {
                    $srcPixel = [IntPtr]::Add($srcRow, $x + $bit)
                    $pixelValue = [System.Runtime.InteropServices.Marshal]::ReadByte($srcPixel, 0)
                    
                    # Threshold: if pixel > 127, it's white (0), else black (1)
                    if ($pixelValue -le 127) {
                        $byteVal = $byteVal -bor (1 -shl (7 - $bit))
                    }
                }
                
                $dstPixel = [IntPtr]::Add($dstRow, [int]($x / 8))
                [System.Runtime.InteropServices.Marshal]::WriteByte($dstPixel, 0, [byte]$byteVal)
            }
        }
        
        $srcImg.UnlockBits($srcData)
        $dstImg.UnlockBits($dstData)
        
        # Save
        $dstImg.Save($dstPath, [System.Drawing.Imaging.ImageFormat]::Png)
        Write-Host "Saved 2-color image to $dstPath"
        
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

# Convert 1bpp fonts to 2-color
$fonts1bpp = @('font0_lat.png', 'font1_lat.png')

foreach ($font in $fonts1bpp) {
    $path = Join-Path $baseDir $font
    $tempPath = [System.IO.Path]::GetTempFileName() + ".png"
    
    Write-Host "`nProcessing $font..."
    Copy-Item $path $tempPath
    Convert-To2Color $tempPath $path
    Remove-Item $tempPath -ErrorAction SilentlyContinue
}

Write-Host "`nDone! 1bpp fonts converted to 2-color."
