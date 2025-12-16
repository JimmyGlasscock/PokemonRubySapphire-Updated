# Load System.Drawing assembly
Add-Type -AssemblyName System.Drawing

$fonts = @(
    @{src='emerald_latin_small.png'; dst='font0_lat.png'},
    @{src='emerald_latin_normal.png'; dst='font1_lat.png'},
    @{src='emerald_latin_short.png'; dst='font3_lat.png'},
    @{src='emerald_latin_narrow.png'; dst='font4_lat.png'}
)

$tempDir = $env:TEMP
$baseDir = 'c:\Users\Jimmy\Documents\decomps\pokeruby\graphics\fonts'

foreach ($font in $fonts) {
    $srcPath = Join-Path $tempDir $font.src
    $dstPath = Join-Path $baseDir $font.dst
    
    if (Test-Path $srcPath) {
        try {
            # Load the source image
            $srcImg = [System.Drawing.Bitmap]::FromFile($srcPath)
            
            # Create intermediate 24-bit RGB bitmap
            $rgbBitmap = New-Object System.Drawing.Bitmap($srcImg.Width, $srcImg.Height, [System.Drawing.Imaging.PixelFormat]::Format24bppRgb)
            
            # Use Graphics to copy (handles indexed source)
            $g = [System.Drawing.Graphics]::FromImage($rgbBitmap)
            $g.DrawImage($srcImg, 0, 0)
            $g.Dispose()
            
            # Now convert RGB to 8-bit indexed grayscale
            $newBitmap = New-Object System.Drawing.Bitmap($srcImg.Width, $srcImg.Height, [System.Drawing.Imaging.PixelFormat]::Format8bppIndexed)
            
            # Set up a 256-color grayscale palette
            $palette = $newBitmap.Palette
            for ($i = 0; $i -lt 256; $i++) {
                $palette.Entries[$i] = [System.Drawing.Color]::FromArgb(255, $i, $i, $i)
            }
            $newBitmap.Palette = $palette
            
            # Lock bits for manual pixel manipulation
            $srcRect = New-Object System.Drawing.Rectangle(0, 0, $rgbBitmap.Width, $rgbBitmap.Height)
            $srcData = $rgbBitmap.LockBits($srcRect, [System.Drawing.Imaging.ImageLockMode]::ReadOnly, $rgbBitmap.PixelFormat)
            $dstData = $newBitmap.LockBits($srcRect, [System.Drawing.Imaging.ImageLockMode]::WriteOnly, $newBitmap.PixelFormat)
            
            $srcPtr = $srcData.Scan0
            $dstPtr = $dstData.Scan0
            $srcBytes = [Math]::Abs($srcData.Stride) * $srcImg.Height
            $dstBytes = [Math]::Abs($dstData.Stride) * $srcImg.Height
            $srcBuffer = New-Object byte[] $srcBytes
            $dstBuffer = New-Object byte[] $dstBytes
            
            [System.Runtime.InteropServices.Marshal]::Copy($srcPtr, $srcBuffer, 0, $srcBytes)
            
            # Convert RGB to grayscale
            for ($y = 0; $y -lt $srcImg.Height; $y++) {
                for ($x = 0; $x -lt $srcImg.Width; $x++) {
                    $srcIndex = $y * $srcData.Stride + $x * 3
                    $dstIndex = $y * $dstData.Stride + $x
                    $b = $srcBuffer[$srcIndex]
                    $g = $srcBuffer[$srcIndex + 1]
                    $r = $srcBuffer[$srcIndex + 2]
                    $gray = [byte](($r + $g + $b) / 3)
                    $dstBuffer[$dstIndex] = $gray
                }
            }
            
            [System.Runtime.InteropServices.Marshal]::Copy($dstBuffer, 0, $dstPtr, $dstBytes)
            
            $rgbBitmap.UnlockBits($srcData)
            $newBitmap.UnlockBits($dstData)
            
            # Save the new image
            $newBitmap.Save($dstPath, [System.Drawing.Imaging.ImageFormat]::Png)
            
            # Clean up
            $rgbBitmap.Dispose()
            
            # Clean up
            $newBitmap.Dispose()
            $img.Dispose()
            
            Write-Host "Converted $($font.src) -> $($font.dst)"
        }
        catch {
            Write-Host "Error converting $($font.src): $_"
        }
    }
}
