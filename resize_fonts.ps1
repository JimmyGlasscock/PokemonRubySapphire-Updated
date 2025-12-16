Add-Type -AssemblyName System.Drawing

function Resize-Font {
    param($srcPath, $dstPath, $newWidth)
    
    $srcImg = $null
    $rgbImg = $null
    $dstImg = $null
    
    try {
        # Load source
        $srcImg = [System.Drawing.Bitmap]::new($srcPath)
        Write-Host "Loaded $srcPath - Size: $($srcImg.Width)x$($srcImg.Height)"
        
        # Calculate new height to maintain aspect ratio
        $aspectRatio = $srcImg.Height / $srcImg.Width
        $newHeight = [int]($newWidth * $aspectRatio)
        
        # Convert indexed to 24bpp RGB first
        $rgbImg = [System.Drawing.Bitmap]::new($srcImg.Width, $srcImg.Height, [System.Drawing.Imaging.PixelFormat]::Format24bppRgb)
        $g = [System.Drawing.Graphics]::FromImage($rgbImg)
        $g.DrawImage($srcImg, 0, 0)
        $g.Dispose()
        
        # Create resized 24bpp bitmap
        $resizedRgb = [System.Drawing.Bitmap]::new($newWidth, $newHeight, [System.Drawing.Imaging.PixelFormat]::Format24bppRgb)
        $g2 = [System.Drawing.Graphics]::FromImage($resizedRgb)
        $g2.InterpolationMode = [System.Drawing.Drawing2D.InterpolationMode]::NearestNeighbor
        $g2.PixelOffsetMode = [System.Drawing.Drawing2D.PixelOffsetMode]::Half
        $g2.DrawImage($rgbImg, 0, 0, $newWidth, $newHeight)
        $g2.Dispose()
        
        # Convert to 8bpp indexed
        $dstImg = [System.Drawing.Bitmap]::new($newWidth, $newHeight, [System.Drawing.Imaging.PixelFormat]::Format8bppIndexed)
        
        # Setup grayscale palette
        $palette = $dstImg.Palette
        for ($i = 0; $i -lt 256; $i++) {
            $palette.Entries[$i] = [System.Drawing.Color]::FromArgb(255, $i, $i, $i)
        }
        $dstImg.Palette = $palette
        
        # Lock bits and convert
        $rect = [System.Drawing.Rectangle]::new(0, 0, $newWidth, $newHeight)
        $srcData = $resizedRgb.LockBits($rect, [System.Drawing.Imaging.ImageLockMode]::ReadOnly, [System.Drawing.Imaging.PixelFormat]::Format24bppRgb)
        $dstData = $dstImg.LockBits($rect, [System.Drawing.Imaging.ImageLockMode]::WriteOnly, [System.Drawing.Imaging.PixelFormat]::Format8bppIndexed)
        
        for ($y = 0; $y -lt $newHeight; $y++) {
            $srcRow = [IntPtr]::Add($srcData.Scan0, $y * $srcData.Stride)
            $dstRow = [IntPtr]::Add($dstData.Scan0, $y * $dstData.Stride)
            
            for ($x = 0; $x -lt $newWidth; $x++) {
                $srcPixel = [IntPtr]::Add($srcRow, $x * 3)
                $dstPixel = [IntPtr]::Add($dstRow, $x)
                
                $b = [System.Runtime.InteropServices.Marshal]::ReadByte($srcPixel, 0)
                $g = [System.Runtime.InteropServices.Marshal]::ReadByte($srcPixel, 1)
                $r = [System.Runtime.InteropServices.Marshal]::ReadByte($srcPixel, 2)
                
                $gray = [byte](($r + $g + $b) / 3)
                [System.Runtime.InteropServices.Marshal]::WriteByte($dstPixel, 0, $gray)
            }
        }
        
        $resizedRgb.UnlockBits($srcData)
        $dstImg.UnlockBits($dstData)
        
        # Save
        $dstImg.Save($dstPath, [System.Drawing.Imaging.ImageFormat]::Png)
        Write-Host "Saved resized image to $dstPath - New size: $($dstImg.Width)x$($dstImg.Height)"
        
        $resizedRgb.Dispose()
        
        return $true
    }
    catch {
        Write-Host "Error: $_"
        return $false
    }
    finally {
        if ($srcImg) { $srcImg.Dispose() }
        if ($rgbImg) { $rgbImg.Dispose() }
        if ($dstImg) { $dstImg.Dispose() }
    }
}

$fonts = @(
    'font0_lat.png',
    'font1_lat.png',
    'font3_lat.png',
    'font4_lat.png'
)

$baseDir = 'c:\Users\Jimmy\Documents\decomps\pokeruby\graphics\fonts'

foreach ($font in $fonts) {
    $path = Join-Path $baseDir $font
    $tempPath = [System.IO.Path]::GetTempFileName() + ".png"
    
    Write-Host "`nProcessing $font..."
    
    # Read current image, resize to 128 width
    Copy-Item $path $tempPath
    Resize-Font $tempPath $path 128
    Remove-Item $tempPath -ErrorAction SilentlyContinue
}

Write-Host "`nDone! All fonts resized to 128px wide."
