$ErrorActionPreference='Stop'
$root=$PSScriptRoot
$dir=Join-Path $root 'continuation-20261030/next-plan-execution'
$config=Join-Path $dir 'r50-regenerated-final/timing-config.json'
$pointer=Join-Path $dir 'r50-permutation-prototype/fixture-pointer.json'
$cases=@('test1','test2','test3','test4','pmux_case01_balanced','pmux_case02_pipeline','pmux_case03_crossbar','pmux_case04_feedback','pmux_case05_nested','baseline_logic_unit_999')
$labels=@('r50-regenerated-primary','r50-regenerated-stability1','r50-regenerated-stability2')
$records=@()
foreach($label in $labels){
 $log=Join-Path $dir ($label+'.log')
 try{
  & (Join-Path $root 'MeasureRules132.ps1') -Config $config -MatrixPointer $pointer -Label $label -Cases $cases *> $log
  $state='PASS'
 }catch{
  $_ | Out-String | Add-Content -LiteralPath $log
  $state='FAIL_RETAINED'
 }
 $batch=Get-ChildItem -LiteralPath (Join-Path $root 'runs') -Directory -Filter ($label+'-*') | Sort-Object Name | Select-Object -Last 1
 $records+=@{label=$label;status=$state;batch=$batch.FullName}
 $records | ConvertTo-Json -Depth 8 | Set-Content -LiteralPath (Join-Path $dir 'final-batch-results.json') -Encoding utf8
 Write-Output "$label $state $($batch.FullName)"
}
