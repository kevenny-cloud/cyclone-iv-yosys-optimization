"""Freeze a locally accepted development package; leave formal submission intact."""
from pathlib import Path
import hashlib,json,shutil,zipfile
R=Path(__file__).resolve().parent;ROOT=R.parents[2];V=R/'continuation-20261030/r48-one-snapshot-clean';O=R/'continuation-20261030/resume-20261030'
read=lambda p:json.loads(p.read_text(encoding='utf-8-sig'))
sha=lambda p:hashlib.sha256(p.read_bytes()).hexdigest()
run=sorted((R/'runs').glob('ten-r48-clean-first-*'));assert len(run)==1;run=run[0]
timing=read(run/'acceptance.json');raw=read(run/'summary.json');result=read(V/'result.json');build=read(V/'build/report.json')
assert timing['status']=='PASS_EFFICIENCY_ONLY'and len(timing['cases'])==10 and all(c['pass']for c in timing['cases'])
candidate=sha(V/'program/yosys.exe');assert candidate==raw['config']['candidate']['yosys_sha256']==build['candidate_sha256']==result['candidate_sha256']
assert read(V/'audit.json')['status']=='PASS'and build['fresh_objects']==160
assert read(V/'startup48/summary.json')['status']=='PASS'and len(read(V/'startup48/summary.json')['cases'])==48
formal=R.parents[1]/'提交材料/yosys.exe';assert sha(formal)==read(V/'plan.json')['formal_submission_sha256']
stress_proofs={}
for reference in ['current','historical']:
 proof=ROOT/('p48relation-'+reference);audit=ROOT/('p48relation-'+reference+'-checks')
 assert read(proof/'summary.json')['status']==read(audit/'summary.json')['status']=='PASS'
 assert len(read(audit/'summary.json')['mutations'])==6
 M=V/'stress'/('historical'if reference=='historical'else'')/'baseline_logic_unit_999'
 for cp,row in read(proof/'summary.json')['checkpoints'].items():
  assert row['all_registers_covered']and [sha(M/v/(cp+'.json'))for v in ['baseline','candidate']]==row['input_hashes']
  assert all(sha(proof/cp/n)==h for n,h in row['proof_hashes'].items())
 stress_proofs[reference]={'proof_directory':str(proof),'audit_directory':str(audit),'proof_summary_sha256':sha(proof/'summary.json'),'audit_summary_sha256':sha(audit/'summary.json'),'registers':[803,800],'all_outputs':96,'checkpoints':2}
resource=read(V/'resources.json');assert not resource['increases']
assert not any(c['candidate']['dff']>c['baseline']['dff']for c in resource['cases'].values())
stress_area=read(V/'stress/resources.json');assert all(x['strict20_pass']for x in stress_area.values())
result.update(status='LOCAL_PUBLIC9_AND_USER_STRESS_PASS_PORTABILITY_PENDING',efficiency={'report':str(run/'acceptance.json'),'status':timing['status'],'cases':10,'passed_cases':[c['id']for c in timing['cases']]},stress={'status':'PASS_FULL_STATE_EVENT_INIT_AND_RESOURCE_AND_CURRENT_REFERENCE_EFFICIENCY','resources':stress_area,'proofs':stress_proofs,'startup_reset_constraint':False},startup48='PASS',formal_submission_unchanged=True)
result['open_findings']['efficiency']='Ten current-reference cases pass in one declared batch; balanced memory+4.9625percent near threshold, no claim of cross-environment margin.'
result['open_findings']['not_a_full_competition_acceptance']=True
(V/'result.json').write_text(json.dumps(result,indent=2),encoding='utf-8')
shutil.copytree(run,V/'efficiency')
D=V/'dependencies';D.mkdir()
scripts=['rebuild_r48_clean.py','export_continuation_candidate.py','complete_continuation_proofs.py',
 'audit_r48_validation.py','validate_r48_boundaries.py','validate_r48_strong_miters.py','validate_r48_state_relations.py',
 'test_continuation_local_r48.py','test_native_guarded_rules.py','check_r48_startup_policy.py',
 'prove.py','prove_state_relation.py','prove_startup_relation.py','export_resumed_stress_candidate.py',
 'prove_r48_current_relation.py','check_r48_current_relation.py','prove_r48_historical_relation.py','check_r48_historical_relation.py',
 'MeasureRules132.ps1','NativePeak.ps1','evaluate_rules132.py','verify_portable_r48.py','finalize_resumed_r48.py']
for name in scripts:shutil.copy2(R/name,D/name)
for name in ['check_plan_v7_muxload.py','check_plan_v8_gate_load.py','test_windows_r1_original.py','test_windows_r1_multiple.py','test_windows_r3.py','test_windows_r10_clean_boxes.py','test_transport_negative.py']:
 shutil.copy2(R/name,D/name)
shutil.copy2(R/'models/enable2-clean1/primitive_decode_sync.v',D/'primitive_decode_sync.v')
shutil.copy2(R/'acceptance-rules-20261029/contract.json',D/'contract.json')
dep={'status':'PASS','candidate_sha256':candidate,'files':{p.name:sha(p)for p in D.iterdir()if p.is_file()},'independent_yosys':{'path':str(ROOT/'oss-cad-suite/bin/yosys.exe'),'sha256':sha(ROOT/'oss-cad-suite/bin/yosys.exe')},'formal_submission_unchanged':True}
(V/'dependency-audit.json').write_text(json.dumps(dep,indent=2),encoding='utf-8')
rows=[]
for area,metric in zip(result['nine_area'],timing['cases'][:9]):
 assert area['case']==metric['id'];d=metric['decision_overhead_percent']
 rows.append(f"| {area['case']} | {area['baseline']} | {area['candidate']} | {area['reduction_percent']:.2f}% | {d['elapsed_seconds']:+.2f}% | {d['main_peak_working_set_bytes']:+.2f}% |")
text=f'''# R48 本地验收报告（2026-10-30）

九例与用户压力测试的本地资源、功能及效率门槛均通过。程序SHA256：`{candidate}`。正式提交程序未替换。未知赛方压力测试和官方环境尚不能据此承诺满分。

| 用例 | 当前原版逻辑 | 候选逻辑 | 降幅 | 完整运行时间开销 | 同次主进程峰值内存开销 |
|---|---:|---:|---:|---:|---:|
'''+ '\n'.join(rows)+'''

用户压力测试：当前同工具链参考1016→754，减少25.7874%；历史参考重新导出999→754，减少24.5245%。两套参考均803个DFF，候选800个。源码、参数和读取顺序未修改。当前参考下压力时间+2.2062%、内存−3.7041%，通过5%门槛；历史参考仅另做资源与完整功能交叉验证，没有单独测历史参考效率。1016与999源自不同参考程序，未混用分母，计数差异原因未进一步定位。

相对R47仅在统一ABC9脚本的&dch中增加-s，减少重复结构合成。R47压力功能已证明，但时间+10.90175%失败，其原始批次保留。另一个有界映射原型R49未推广、未测速。没有重复测量同一版本选有利样本，没有修改无关pass或ABC程序来抵消耗时。

160个对象均从冻结源码、生成物和原编译命令新建；不复用对象。未独立重新配置CMake、未重新运行全部生成器。78个设计逻辑与DFF均无增加；156检查点全部覆盖：70个完整状态关系、56个独立EQY分区归纳、30个解码结构相同。原始只选择43设计的EQY汇总保留INCOMPLETE，最终覆盖以result.json为准。47项边界、23项重叠加强证明、40项局部真值、36项新增默认分支/共享区域/七输入规则检查通过；48种启动配置的完整状态关系通过。主审计2474项通过。

压力相对当前、历史两套参考，各自两个检查点完成完整多对一状态关系证明，覆盖原版全部803和候选全部800个寄存器、全部96个公开输出、实际初值、时钟、异步清零、下一状态。没有添加先复位或活性假设。每套审计重新生成方程并核对覆盖/初值/网表/日志哈希，六类故意错误均被拒绝。名称仅提出对应关系，最终关系由独立组合SAT证明；3个减少的寄存器是对应关系的一部分，没有擅自当常量丢弃。此前整图归纳超时和长路径失败记录仍保留在R47压力目录。

功能结论限于记录的离散二值原语模型，不另行宣称全部RTL到器件端到端等价、任意X/Z或模拟/物理时序。fresh_08原版默认BRAM因defparam INIT错误没有通过；双方统一-nobram作为补充配置，候选默认另与该参考比较通过。全图最大LUT级数检查不等同于物理时序保证。顺序/时序盒与其它未覆盖输入不得概括为通用全保证。

效率以冻结第132项执行：每例原版预热10次，原版再候选；符合短用例条件时恰好5组新配对均值，保留首次样本。仅test1触发复测。时间为完整Measure-Command，内存为同次主进程峰值工作集。测量期间本任务编译/证明均结束；后台和杀毒状态未独立认证，未修改系统设置。balanced内存+4.9625%接近5%门槛，此批通过不代表跨环境稳定余量。全部样本见efficiency/。

本地通过后准备独立便携候选包，正式目录保持原样。文件清单、输入、源码、构建与证明原始证据保留；搬迁验证结果将写入portable-check.json。实际评分和赛方未提供的压力输入不属于已验证结果。
'''
(V/'验证与验收报告.md').write_text(text,encoding='utf-8')
# Stage a new local candidate rather than modify the formal submission directory.
P=R/'候选交付/R48-20261030';assert not P.exists();P.parent.mkdir(exist_ok=True);shutil.copytree(V/'program',P)
shutil.copytree(V/'source',P/'source');shutil.copytree(V/'generated',P/'build-record/generated')
for name in ['commands.json','report.json','link.rsp']:shutil.copy2(V/'build'/name,P/'build-record'/name)
shutil.copy2(V/'plan.json',P/'build-record/frozen-plan.json')
shutil.copytree(R.parents[1]/'提交材料/licenses',P/'licenses')
shutil.copy2(R/'verify_portable_r48.py',P/'verify_portable.py')
F=P/'verification';F.mkdir();fixtures=read(V/'ten-case-fixtures/summary.json');portable={'cases':{}}
for name,c in fixtures['cases'].items():
 S=V/'stress'/name/'candidate'if name=='baseline_logic_unit_999'else V/'matrix'/name/'candidate'
 W=F/'fixtures'/name;W.mkdir(parents=True)
 for n in c['input_sha256']:shutil.copy2(V/'ten-case-fixtures'/name/n,W/n)
 Z=F/'expected'/name;Z.mkdir(parents=True)
 for cp in ['checkpoint','final']:shutil.copy2(S/(cp+'.json'),Z/(cp+'.json'))
 h=collections_counter=None
 import collections
 counts=collections.Counter(x['type']for x in read(S/'final.json')['modules'][c['top']]['cells'].values())
 portable['cases'][name]={'top':c['top'],'input_sha256':c['input_sha256'],'source_order':c['source_order'],
  'script':(S/'run.ys').read_text(encoding='utf-8'),'logic':counts['cycloneiv_lcell_comb']+counts['$not'],'dff':counts['dffeas']}
(F/'fixtures.json').write_text(json.dumps(portable,indent=2),encoding='utf-8')
E=P/'evidence';E.mkdir()
for directory in ['matrix','state-relations','boundaries','strong-rule-miters','local-sat','native-rules','startup48','default-rom-functional','efficiency','stress','dependencies']:
 shutil.copytree(V/directory,E/directory)
for reference in ['current','historical']:
 shutil.copytree(ROOT/('p48relation-'+reference),E/('stress-relation-'+reference));shutil.copytree(ROOT/('p48relation-'+reference+'-checks'),E/('stress-relation-'+reference+'-checks'))
for name in ['result.json','audit.json','resources.json','flow-addendum.json','dependency-audit.json','验证与验收报告.md']:shutil.copy2(V/name,E/name)
(P/'README.md').write_text('''# R48 Windows 本地验收候选包

本包是已完成所列本地验收的独立候选，未替换项目正式提交目录。运行入口为根目录yosys.exe，配套yosys-abc.exe和share/。运行不依赖WSL或原项目绝对路径。

在本目录执行 `python verify_portable.py --out ../R48验证结果`，输出目录必须不存在且位于包外。该命令校验文件后重跑九例及用户压力综合，对照冻结两检查点网表、资源；**不是重新执行独立形式证明或效率评分**。只核查文件用 `python verify_portable.py --check-only`。Python要求3.9+。

完整本地结论和边界见evidence/验证与验收报告.md。独立证明与原始效率样本均已归档；历史证明脚本中的绝对路径是溯源记录，不能未经路径适配直接重跑。不得使用含导出操作的复现入口代替效率测量。

source/是实际编译冻结的完整源码；build-record/含原命令、生成输入、对象指纹及计划，命令记录含原工作路径，需要适配源码/生成物/对象路径和LLVM-MinGW编译器才能在其它位置重建。没有把对象哈希清单冒充包内已包含全部对象或独立重配置构建。

候选在所列78设计、九公开例和一个用户压力测试中通过；不保证未知赛方压力全部满分，不覆盖模拟/物理时序。balanced内存距5%门槛很近，正式环境须另验。licenses/保留依赖许可，实际Windows构建编译器见build-record/report.json。正式提交前按赛方要求确认材料。
''',encoding='utf-8')
(P/'manifest.json').write_text(json.dumps({'status':'LOCAL_ACCEPTANCE_PASS_PORTABILITY_PENDING','candidate_sha256':candidate,'abc_sha256':sha(P/'yosys-abc.exe'),'local_cases':10,'scope_report':'evidence/验证与验收报告.md','source':'source','formal_submission_unchanged':True},indent=2),encoding='utf-8')
package_files={p.relative_to(P).as_posix():sha(p)for p in sorted(P.rglob('*'))if p.is_file()and p.name!='SHA256.json'}
(P/'SHA256.json').write_text(json.dumps(package_files,indent=2),encoding='utf-8')
(O/'portable-package-pointer.json').write_text(json.dumps({'package':str(P),'candidate_sha256':candidate,'formal_unchanged':True},indent=2),encoding='utf-8')
print('LOCAL_GATES_PASS_PACKAGE_STAGED',P,len(package_files),flush=True)
