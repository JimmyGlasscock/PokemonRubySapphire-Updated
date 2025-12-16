# Download and convert Emerald fonts to Ruby format
Add-Type -AssemblyName System.Drawing

# Font specifications: name, URL, target width, target height, bpp (1=2colors, 4=3colors)
$fonts = @(
    @{Name='font0_lat'; Url='https://raw.githubusercontent.com/pret/pokeemerald/master/graphics/fonts/latin_small.png'; Width=128; Height=256; Bpp=1},
    @{Name='font1_lat'; Url='https://raw.githubusercontent.com/pret/pokeemerald/master/graphics/fonts/latin_normal.png'; Width=128; Height=128; Bpp=1},
    @{Name='font3_lat'; Url='https://raw.githubusercontent.com/pret/pokeemerald/master/graphics/fonts/latin_short.png'; Width=128; Height=256; Bpp=4},
    @{Name='font4_lat'; Url='https://raw.githubusercontent.com/pret/pokeemerald/master/graphics/fonts/latin_narrow.png'; Width=128; Height=112; Bpp=4}
)

function Convert-FontToRubyFormat {
    param(
        [string]$SourcePath,
        [string]$OutputPath,
        [int]$TargetWidth,
        [int]$TargetHeight,
        [int]$Bpp
    )
    
    Write-Host "Converting $SourcePath to ${TargetWidth}x${TargetHeight}..."
    
    # Load source image
    $srcImg = [System.Drawing.Image]::FromFile($SourcePath)
    
    # Create target bitmap with 8-bit indexed format
    $destImg = New-Object System.Drawing.Bitmap($TargetWidth, $TargetHeight, [System.Drawing.Imaging.PixelFormat]::Format8bppIndexed)
    
    # Set up palette based on bpp
    $palette = $destImg.Palette
    if ($Bpp -eq 1) {
        # 1bpp = 2 colors (transparent + white)
        $palette.Entries[0] = [System.Drawing.Color]::FromArgb(0, 0, 0, 0)
        $palette.Entries[1] = [System.Drawing.Color]::FromArgb(255, 255, 255, 255)
        for ($i = 2; $i -lt 256; $i++) {
            $palette.Entries[$i] = [System.Drawing.Color]::FromArgb(0, 0, 0, 0)
        }
    } else {
        # 4bpp = 3 colors (transparent + shadow + white)
        $palette.Entries[0] = [System.Drawing.Color]::FromArgb(0, 0, 0, 0)
        $palette.Entries[1] = [System.Drawing.Color]::FromArgb(255, 128, 128, 128)
        $palette.Entries[2] = [System.Drawing.Color]::FromArgb(255, 255, 255, 255)
        for ($i = 3; $i -lt 256; $i++) {
            $palette.Entries[$i] = [System.Drawing.Color]::FromArgb(0, 0, 0, 0)
        }
    }
    $destImg.Palette = $palette
    
    # Lock both bitmaps for direct pixel access
    $srcRect = New-Object System.Drawing.Rectangle(0, 0, $srcImg.Width, $srcImg.Height)
    $destRect = New-Object System.Drawing.Rectangle(0, 0, $TargetWidth, $TargetHeight)
    
    $srcData = $srcImg.LockBits($srcRect, [System.Drawing.Imaging.ImageLockMode]::ReadOnly, $srcImg.PixelFormat)
    $destData = $destImg.LockBits($destRect, [System.Drawing.Imaging.ImageLockMode]::WriteOnly, [System.Drawing.Imaging.PixelFormat]::Format8bppIndexed)
    
    # Calculate scale factors
    $scaleX = $srcImg.Width / $TargetWidth
    $scaleY = $srcImg.Height / $TargetHeight
    
    # Get bytes per pixel for source
    $srcBpp = [System.Drawing.Image]::GetPixelFormatSize($srcImg.PixelFormat) / 8
    
    # Copy and convert pixels
    $srcPtr = $srcData.Scan0
    $destPtr = $destData.Scan0
    
    for ($y = 0; $y -lt $TargetHeight; $y++) {
        for ($x = 0; $x -lt $TargetWidth; $x++) {
            # Get source coordinates (nearest neighbor)
            $srcX = [Math]::Floor($x * $scaleX)
            $srcY = [Math]::Floor($y * $scaleY)
            
            # Clamp to source bounds
            if ($srcX -ge $srcImg.Width) { $srcX = $srcImg.Width - 1 }
            if ($srcY -ge $srcImg.Height) { $srcY = $srcImg.Height - 1 }
            
            # Read source pixel
            $srcOffset = [IntPtr]::new($srcPtr.ToInt64() + ($srcY * $srcData.Stride) + ($srcX * $srcBpp))
            $b = [System.Runtime.InteropServices.Marshal]::ReadByte($srcOffset)
            $g = [System.Runtime.InteropServices.Marshal]::ReadByte([IntPtr]::new($srcOffset.ToInt64() + 1))
            $r = [System.Runtime.InteropServices.Marshal]::ReadByte([IntPtr]::new($srcOffset.ToInt64() + 2))
            $a = if ($srcBpp -eq 4) { [System.Runtime.InteropServices.Marshal]::ReadByte([IntPtr]::new($srcOffset.ToInt64() + 3)) } else { 255 }
            
            # Convert to palette index
            $paletteIndex = 0
            if ($a -lt 128) {
                # Transparent
                $paletteIndex = 0
            } else {
                # Calculate brightness
                $brightness = ($r + $g + $b) / 3
                
                if ($Bpp -eq 1) {
                    # 1bpp: threshold at 128
                    $paletteIndex = if ($brightness -gt 128) { 1 } else { 0 }
                } else {
                    # 4bpp: 3 levels
                    if ($brightness -lt 85) {
                        $paletteIndex = 0  # transparent/black
                    } elseif ($brightness -lt 170) {
                        $paletteIndex = 1  # shadow
                    } else {
                        $paletteIndex = 2  # white
                    }
                }
            }
            
            # Write destination pixel
            $destOffset = [IntPtr]::new($destPtr.ToInt64() + ($y * $destData.Stride) + $x)
            [System.Runtime.InteropServices.Marshal]::WriteByte($destOffset, $paletteIndex)
        }
    }
    
    $srcImg.UnlockBits($srcData)
    $destImg.UnlockBits($destData)
    $srcImg.Dispose()
    
    # Save result
    $destImg.Save($OutputPath, [System.Drawing.Imaging.ImageFormat]::Png)
    $destImg.Dispose()
    
    Write-Host "  Saved to $OutputPath"
}

# Process each font
foreach ($font in $fonts) {
    $tempPath = "temp_$($font.Name).png"
    $finalPath = "graphics/fonts/$($font.Name).png"
    
    Write-Host "`nProcessing $($font.Name)..."
    
    # Download
    Write-Host "  Downloading from $($font.Url)..."
    curl.exe -s -o $tempPath $font.Url
    
    if (Test-Path $tempPath) {
        # Convert
        Convert-FontToRubyFormat -SourcePath $tempPath -OutputPath $finalPath -TargetWidth $font.Width -TargetHeight $font.Height -Bpp $font.Bpp
        
        # Cleanup
        Remove-Item $tempPath
    } else {
        Write-Host "  ERROR: Failed to download!" -ForegroundColor Red
    }
}

Write-Host "`nDone! All fonts converted."
