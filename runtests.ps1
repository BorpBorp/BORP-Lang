param(
    [string]$Interpreter = ".\borp.exe",      # CHANGE ME: path to your C++ interpreter
    [string]$Compiler    = ".\borpc_c.exe",
    [string]$TestDir     = "tests",
    [string]$Runtime     = "runtime",
    [string]$Seed        = "test2borp.c",
    [string]$Source      = "src\borpc.borp",
    [switch]$RebuildCompiler
)

$outDir = Join-Path $TestDir "out"
New-Item -ItemType Directory -Force -Path $outDir | Out-Null
$env:ASAN_OPTIONS = "detect_leaks=0"
$clangArgs = @("-g", "-O0", "-fsanitize=address", "-fno-omit-frame-pointer",
               "-Wall", "-Wno-parentheses-equality", "-I$Runtime")

function Norm([string]$s) { return ($s -replace "`r`n", "`n").TrimEnd() }

$results = @()
function Record($name, $status, $detail) {
    $script:results += [pscustomobject]@{ Test = $name; Status = $status }
    $color = if ($status -eq "PASS") { "Green" } else { "Red" }
    Write-Host ("{0,-5} {1}  {2}" -f $status, $name, $detail) -ForegroundColor $color
}

# Optional: rebuild the compiler binary from the seed C file
if ($RebuildCompiler) {
    Write-Host "Rebuilding $Compiler from $Seed ..."
    & clang @clangArgs $Seed -o $Compiler 2>&1 | Out-File -Encoding utf8 (Join-Path $outDir "compiler_build.log")
    if ($LASTEXITCODE -ne 0) {
        Write-Host "Compiler build failed, see $outDir\compiler_build.log" -ForegroundColor Red
        exit 1
    }
}

# Fixed-point check: compiling the compiler's own source must reproduce the seed
$fp = Join-Path $outDir "fixedpoint.c"
Remove-Item $fp -ErrorAction SilentlyContinue
& $Compiler $Source $fp 2>&1 | Out-Null
if (-not (Test-Path $fp)) {
    Record "fixed-point" "FAIL" "compiler produced no output"
} elseif ((Norm (Get-Content $fp -Raw)) -eq (Norm (Get-Content $Seed -Raw))) {
    Record "fixed-point" "PASS" "identical to $Seed"
} else {
    Record "fixed-point" "FAIL" "output differs from $Seed"
}

# One test per .borp file in the test folder
Get-ChildItem $TestDir -Filter *.borp | Sort-Object Name | ForEach-Object {
    $name  = $_.BaseName
    $src   = $_.FullName
    $cfile = Join-Path $outDir "$name.c"
    $exe   = Join-Path $outDir "$name.exe"
    Remove-Item $cfile, $exe -ErrorAction SilentlyContinue

    # Expected output comes from the interpreter
    $interpText = (& $Interpreter $src 2>&1) | Out-String
    $marker = "Executing Program Output:"
    $pos = $interpText.IndexOf($marker)
    if ($pos -ge 0) { $interpText = $interpText.Substring($pos + $marker.Length) }
    $expected = (Norm $interpText).TrimStart("`r", "`n")

    # borp -> C
    & $Compiler $src $cfile 2>&1 | Out-File -Encoding utf8 (Join-Path $outDir "$name.compile.log")
    if ($LASTEXITCODE -ne 0 -or -not (Test-Path $cfile)) {
        Record $name "FAIL" "borp->C failed (see $name.compile.log)"
        return
    }

    # C -> exe
    & clang @clangArgs $cfile -o $exe 2>&1 | Out-File -Encoding utf8 (Join-Path $outDir "$name.build.log")
    if ($LASTEXITCODE -ne 0) {
        Record $name "FAIL" "C build failed (see $name.build.log)"
        return
    }

    # Run and compare (ASan errors land in stderr, so they cause a mismatch)
    $actual = Norm ((& $exe 2>&1) | Out-String)
    if ($actual -eq $expected) {
        Record $name "PASS" ""
    } else {
        $expected | Set-Content -Encoding utf8 (Join-Path $outDir "$name.expected.txt")
        $actual   | Set-Content -Encoding utf8 (Join-Path $outDir "$name.actual.txt")
        Record $name "FAIL" "output differs (see $name.expected.txt / $name.actual.txt)"
    }
}

$failed = @($results | Where-Object { $_.Status -ne "PASS" }).Count
Write-Host ""
Write-Host ("{0} passed, {1} failed" -f ($results.Count - $failed), $failed)
if ($failed -gt 0) { exit 1 }