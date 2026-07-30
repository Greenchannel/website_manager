# webm.ps1
[Console]::OutputEncoding = [System.Text.Encoding]::UTF8
$exe = Join-Path $PSScriptRoot "webm.exe"
& $exe $args