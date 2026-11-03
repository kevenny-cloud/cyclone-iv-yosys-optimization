param(
 [Parameter(Mandatory=$true)][string]$Config,
 [Parameter(Mandatory=$true)][string]$MatrixPointer,
 [Parameter(Mandatory=$true)][string]$Label,
 [string[]]$Cases=@('test1','test2','test3','test4'),
 [string]$EnvironmentNote='Background and antivirus condition not independently certified'
)
$ErrorActionPreference='Stop'
. "$PSScriptRoot/NativePeak.ps1"
$utf8=[System.Text.UTF8Encoding]::new($false)
$cfg=Get-Content -LiteralPath $Config -Raw -Encoding UTF8 | ConvertFrom-Json
$ptr=Get-Content -LiteralPath $MatrixPointer -Raw -Encoding UTF8 | ConvertFrom-Json
$fixtures=Get-Content -LiteralPath (Join-Path $ptr.results 'summary.json') -Raw -Encoding UTF8 | ConvertFrom-Json
if(($Cases|Select-Object -Unique).Count -ne $Cases.Count){throw 'Duplicate cases'}
foreach($v in @('baseline','candidate')) {
 foreach($item in @('yosys','abc')) {
  $p=$cfg.$v.$item
  if((Get-FileHash -LiteralPath $p -Algorithm SHA256).Hash.ToLower() -ne $cfg.$v.($item+'_sha256')) {throw "$v $item hash mismatch"}
 }
 foreach($file in $cfg.$v.runtime_sha256.PSObject.Properties) {
  $runtimeFile=Join-Path $cfg.$v.data $file.Name
  if((Get-FileHash -LiteralPath $runtimeFile -Algorithm SHA256).Hash.ToLower() -ne $file.Value){throw "$v runtime hash mismatch: $($file.Name)"}
 }
}
if($cfg.baseline.abc_sha256 -ne $cfg.candidate.abc_sha256){throw 'ABC dependency mismatch'}
$run=Join-Path $PSScriptRoot ('runs/'+$Label+'-'+(Get-Date -Format 'yyyyMMdd_HHmmss_fff'))
[void](New-Item -ItemType Directory -Path $run)
$report=[ordered]@{
 status='RUNNING';schema='RULE132_V1';label=$Label;config=$cfg;cases=@($Cases)
 rule_contract_sha256=(Get-FileHash "$PSScriptRoot/acceptance-rules-20261029/contract.json" -Algorithm SHA256).Hash.ToLower()
 observer_sha256=(Get-FileHash -LiteralPath $PSCommandPath -Algorithm SHA256).Hash.ToLower()
 native_peak_sha256=(Get-FileHash "$PSScriptRoot/NativePeak.ps1" -Algorithm SHA256).Hash.ToLower()
 config_sha256=(Get-FileHash -LiteralPath $Config -Algorithm SHA256).Hash.ToLower()
 fixture_summary_sha256=(Get-FileHash -LiteralPath (Join-Path $ptr.results 'summary.json') -Algorithm SHA256).Hash.ToLower()
 timing='Measure-Command whole-process elapsed; no overhead subtraction'
 memory='retained main-process peak working set from same run'
 warmup_baseline_per_case=10;primary_order=@('baseline','candidate');review_count=5
 review_aggregation='ratio of arithmetic means of five fresh pairs; primary excluded'
 environment_note=$EnvironmentNote;results=@();error=$null
}
function Save-Report {
 [System.IO.File]::WriteAllText((Join-Path $run 'summary.json'),($report|ConvertTo-Json -Depth 30),$utf8)
}
function Measure-One([string]$Variant,[string]$Folder,[string]$Tag) {
 $entry=$cfg.$Variant;$proc=$null
 try {
  $elapsed=Measure-Command {
   $proc=Start-Process -FilePath $entry.yosys -ArgumentList @('-Q','-s','measure.ys') -WorkingDirectory $Folder -WindowStyle Hidden -PassThru -RedirectStandardOutput (Join-Path $Folder "$Tag.log") -RedirectStandardError (Join-Path $Folder "$Tag.err")
   $handle=$proc.Handle
   if(!$proc.WaitForExit(65000)){$proc.Kill();$proc.WaitForExit();throw 'Synthesis timeout; candidate not accepted'}
   $proc.WaitForExit()
  }
  $peak=[long][FpgaNativePeak]::Read($handle)
  $r=[ordered]@{variant=$Variant;tag=$Tag;exit_code=$proc.ExitCode;elapsed_seconds=$elapsed.TotalSeconds;main_peak_working_set_bytes=$peak;main_cpu_seconds_excluding_abc=$proc.TotalProcessorTime.TotalSeconds}
  if($r.exit_code -ne 0 -or $peak -le 0){throw "Measurement failed: $Tag"}
  return $r
 } finally {if($proc){$proc.Dispose()}}
}
$oldTemp=$env:TEMP;$oldTmp=$env:TMP;$oldDiag=$env:FPGA_DIAG_FILE
$oldToolEnv=@{}
foreach($e in Get-ChildItem Env:) {
 if($e.Name -like 'YOSYS*' -or $e.Name -eq 'ABC') {$oldToolEnv[$e.Name]=$e.Value}
}
try {
 foreach($name in $oldToolEnv.Keys){Remove-Item -LiteralPath "Env:$name"}
 $temp=Join-Path $run 'temp';[void](New-Item -ItemType Directory -Path $temp)
 $env:TEMP=$temp;$env:TMP=$temp;Remove-Item Env:FPGA_DIAG_FILE -ErrorAction SilentlyContinue
 Save-Report
 foreach($id in $Cases) {
  if($id -notmatch '^[a-zA-Z0-9_-]+$'){throw 'Invalid case identifier'}
  $c=$fixtures.cases.$id;if(!$c){throw "Unknown fixture $id"}
  $folder=Join-Path $run $id;[void](New-Item -ItemType Directory -Path $folder)
  foreach($name in $c.input_sha256.PSObject.Properties.Name) {
   $src=Join-Path (Join-Path $ptr.results $id) $name
   if((Get-FileHash -LiteralPath $src -Algorithm SHA256).Hash.ToLower() -ne $c.input_sha256.$name){throw 'Input hash mismatch'}
   Copy-Item -LiteralPath $src -Destination (Join-Path $folder $name)
  }
  if(!$c.source_order -or @($c.source_order).Count -ne @($c.input_sha256.PSObject.Properties).Count){throw 'Incomplete source order'}
  $params='';if($c.parameters){foreach($p in $c.parameters.PSObject.Properties){$params+="chparam -set $($p.Name) $($p.Value) $($c.top)`n"}}
  $script="read_verilog -icells $($c.source_order -join ' ')`n"+$params+"synth_intel -family cycloneiv -top $($c.top)`n"
  [System.IO.File]::WriteAllText((Join-Path $folder 'measure.ys'),$script,$utf8)
  $case=[ordered]@{id=$id;source_order=@($c.source_order);input_sha256=$c.input_sha256;script_sha256=(Get-FileHash (Join-Path $folder 'measure.ys') -Algorithm SHA256).Hash.ToLower();warmup=@();primary=[ordered]@{};review=@();decision=$null}
  $report.results+=,$case;Save-Report
  for($i=0;$i -lt 10;$i++){$case.warmup+=,(Measure-One 'baseline' $folder "warmup-$i-baseline");Save-Report}
  foreach($v in @('baseline','candidate')){$case.primary[$v]=Measure-One $v $folder "primary-$v";Save-Report}
  $b=$case.primary.baseline;$g=$case.primary.candidate
  $overTime=100*($g.elapsed_seconds/$b.elapsed_seconds-1)
  $overMem=100*($g.main_peak_working_set_bytes/$b.main_peak_working_set_bytes-1)
  $small=$b.elapsed_seconds -lt 1 -and $g.elapsed_seconds -lt 1
  if($small -and ($overTime -gt 5 -or $overMem -gt 5)) {
   for($i=0;$i -lt 5;$i++) {
    $pair=[ordered]@{index=$i;baseline=(Measure-One 'baseline' $folder "review-$i-baseline");candidate=(Measure-One 'candidate' $folder "review-$i-candidate")}
    $case.review+=,$pair;Save-Report
   }
  }
  # One pure policy implementation evaluates both new runs and archived runs.
  Save-Report
 }
 $report.status='COMPLETE'
} catch {$report.status='ERROR';$report.error=$_.Exception.Message;throw}
finally {
 $env:TEMP=$oldTemp;$env:TMP=$oldTmp;$env:FPGA_DIAG_FILE=$oldDiag
 foreach($name in $oldToolEnv.Keys){Set-Item -LiteralPath "Env:$name" -Value $oldToolEnv[$name]}
 Save-Report;Write-Output "RESULTS $run"
}
& python "$PSScriptRoot/evaluate_rules132.py" (Join-Path $run 'summary.json') --out (Join-Path $run 'acceptance.json')
if($LASTEXITCODE -ne 0){throw 'Efficiency gate did not pass; see acceptance.json'}
