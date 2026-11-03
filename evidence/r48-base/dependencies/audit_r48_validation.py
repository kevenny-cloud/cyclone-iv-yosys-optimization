from pathlib import Path
import json,hashlib,importlib.util,shutil,collections
R=Path(__file__).resolve().parent;V=R/'continuation-20261030/r48-one-snapshot-clean';M=V/'matrix';sha=lambda p:hashlib.sha256(p.read_bytes()).hexdigest();read=lambda p:json.loads(p.read_text(encoding='utf-8-sig'));plan=read(V/'plan.json');build=read(V/'build/report.json');matrix=read(M/'summary.json');eq=read(M/'proof_summary.json');state=read(V/'state-relations/summary.json');bound=read(V/'boundaries/summary.json');strong=read(V/'strong-rule-miters/summary.json');checks=[]
def check(name,cond):
 checks.append({'check':name,'pass':bool(cond)});assert cond,name
spec=importlib.util.spec_from_file_location('sr_audit',R/'prove_state_relation.py');sr=importlib.util.module_from_spec(spec);spec.loader.exec_module(sr)
spec=importlib.util.spec_from_file_location('graph_guard',R/'prove.py');pg=importlib.util.module_from_spec(spec);spec.loader.exec_module(pg)
check('fresh160',build['fresh_objects']==build['completed']==build['expected_objects']==160 and build['clean_rebuild']and build['no_prior_objects_or_build_tree']);check('source_frozen',all(sha(V/'source'/p)==h for p,h in plan['source_sha256'].items()));check('generated_frozen',all(sha(V/'generated'/p)==h for p,h in plan['generated_sha256'].items()));check('all_objects_bound',len(build['object_sha256'])==160 and all(sha(Path(p))==h for p,h in build['object_sha256'].items()));candidate=sha(V/'program/yosys.exe');check('candidate_bound',candidate==build['candidate_sha256']==matrix['config']['candidate']['yosys_sha256']==bound['candidate_sha256']==strong['candidate_sha256']==state['candidate_sha256']);check('abc_unchanged',sha(V/'program/yosys-abc.exe')==matrix['config']['baseline']['abc_sha256']==matrix['config']['candidate']['abc_sha256']);check('formal_submission_unchanged',sha(R.parents[1]/'提交材料/yosys.exe')==plan['formal_submission_sha256']);check('78_exports',matrix['status']=='EXPORTED'and len(matrix['cases'])==78);check('boundary_groups_complete',bound['status']=='PASS'and len(bound['groups'])==6);check('strong_miters_complete',strong['status']=='PASS'and len(strong['tests'])==23);check('state35_complete',state['status']=='PASS'and len(state['cases'])==35)
for name,c in matrix['cases'].items():
 for rel,h in c['input_sha256'].items():check('input:'+name+'/'+rel,sha(M/name/rel)==h)
 for v in ['baseline','candidate']:
  for f,h in c['variants'][v]['sha256'].items():check('export:'+name+'/'+v+'/'+f,sha(M/name/v/f)==h)
  for cp in ['checkpoint','final']:pg.guard(M/name/v/(cp+'.json'),c['top'],True,True)
for name,d in state['cases'].items():
 for cp in ['checkpoint','final']:
  a,b=[read(M/name/v/(cp+'.json'))['modules'][matrix['cases'][name]['top']]for v in ['baseline','candidate']];sr.audit(a,b,V/'state-relations'/name/cp)
# Audit every reported successful EQY/identity result without rewriting failed attempts.
for name,c in eq['cases'].items():
 for label,record in c['checkpoints'].items():
  if record.get('status')not in ['EQY_PARTITION_SAT_PASS','STRUCTURAL_IDENTITY']:continue
  cp='checkpoint'if label=='abc'else'final';folder=Path(record['directory'])
  for v in ['baseline','candidate']:
   check('eq-raw:'+name+'/'+label+'/'+v,sha(M/name/v/(cp+'.il'))==record['transport'][v]['raw_sha256']);check('eq-json:'+name+'/'+label+'/'+v,sha(M/name/v/(cp+'.json'))==record['guards'][v]['sha256']);check('eq-generic:'+name+'/'+label+'/'+v,sha(folder/(v+'-generic.il'))==record['decoded'][v]['generic_sha256'])
  if record['status']=='STRUCTURAL_IDENTITY':
   norm=lambda p:'\n'.join(x for x in p.read_text(encoding='utf-8').splitlines()if not x.lstrip().startswith(('#','autoidx ','attribute \\src ','attribute \\generator ')))
   check('structural:'+name+'/'+label,norm(folder/'baseline-generic.il')==norm(folder/'candidate-generic.il'))
  else:
   check('eq-partitions-nonempty:'+name+'/'+label,bool(record['partitions']))
   for part,p in record['partitions'].items():
    f=folder/'eqy-partitions/strategies'/part/'induction';check('eq-script:'+name+'/'+label+'/'+part,sha(f/'run.ys')==p['script_sha256']);check('eq-log:'+name+'/'+label+'/'+part,sha(f/'independent-run.log')==p['log_sha256']);check('eq-sat-pass:'+name+'/'+label+'/'+part,any(marker in (f/'independent-run.log').read_text(encoding='utf-8',errors='replace') for marker in ['SAT temporal induction proof finished - no model found: SUCCESS!','Induction step proven: SUCCESS!']));check('eq-model:'+name+'/'+label+'/'+part,sha(folder/'eqy-partitions/partitions'/(part+'.il'))==p['model_sha256'])
coverage={};methods=collections.Counter()
for name in matrix['cases']:
 selected={}
 for cp,label in [('checkpoint','abc'),('final','final')]:
  if name in state['cases']:
   record=state['cases'][name][cp];check('selected-state:'+name+'/'+cp,record['status']=='STATE_TRANSITION_SAT_PASS');method='COMPLETE_STATE_EVENT_INIT_SAT'
  else:
   record=eq['cases'][name]['checkpoints'][label];check('selected-eq:'+name+'/'+cp,record['status']in ['EQY_PARTITION_SAT_PASS','STRUCTURAL_IDENTITY']);method=record['status']
  selected[cp]=method;methods[method]+=1
 coverage[name]={'checkpoints':selected,'synth_options':matrix['cases'][name].get('synth_options',[]),'flow_note':matrix['cases'][name].get('flow_note','Default flow')}
for g,x in bound['groups'].items():check('boundary-result:'+g,sha(V/'boundaries'/g/'summary.json')==x['summary_sha256'])
check('counterexample-check',strong['tests']['xor-decode_default']['deliberate_fault_rejected'])
local=read(V/'local-sat/summary.json');check('local40',local['status']=='PASS'and len(local['cases'])==40 and local['candidate_sha256']==candidate);check('local-fault-rejected',local['cases']['case00']['deliberate_fault_rejected'])
check('proof-tool-current',sha(Path(eq['proof_tool']))==eq['proof_tool_sha256']==strong['proof_tool_sha256']==local['proof_tool_sha256']);check('proof-model-current',sha(Path(eq['model']))==eq['model_sha256']);check('proof-driver-current',sha(R/'prove.py')==eq['proof_driver_sha256'])
check('all78-no-increase',not read(V/'resources.json')['increases'])
for name,d in state['cases'].items():
 for cp,rec in d.items():check('state-summary:'+name+'/'+cp,sha(V/'state-relations'/name/cp/'summary.json')==rec['summary_sha256'])
for cp in ['checkpoint','final']:
 a=read(M/'fresh_08/baseline'/(cp+'.json'))['modules']['fresh_08'];b=read(V/'default-rom-functional/candidate'/(cp+'.json'))['modules']['fresh_08'];sr.audit(a,b,V/'default-rom-functional'/cp)
new=read(V/'native-rules/summary.json');check('native-rules36',new['status']=='PASS'and len(new['cases'])==36 and new['candidate_sha256']==candidate);check('native-rules-independent-tool',new['proof_tool_sha256']==eq['proof_tool_sha256'])
for name,record in new['cases'].items():
 w=V/'native-rules'/name
 for file,h in record['sha256'].items():check('native-rule-file:'+name+'/'+file,sha(w/file)==h)
 if record['method']=='STRUCTURAL_IDENTITY':check('native-rule-identity:'+name,read(w/'gold.json')['modules']['t']==read(w/'gate.json')['modules']['t'])
 else:check('native-rule-sat:'+name,'SAT proof finished - no model found: SUCCESS!'in(w/'proof.log').read_text(encoding='utf-8'))
resource=read(V/'resources.json');nine=['test1','test2','test3','test4','pmux_case01_balanced','pmux_case02_pipeline','pmux_case03_crossbar','pmux_case04_feedback','pmux_case05_nested'];area=[]
for n in nine:
 d=resource['cases'][n];b=d['baseline']['logic'];g=d['candidate']['logic'];check('nine_area:'+n,g<=b*4//5);area.append({'case':n,'baseline':b,'candidate':g,'reduction_percent':100*(1-g/b)})
result={'status':'VALIDATION_PASS_EFFICIENCY_PENDING','requested_checks':{'clean_rebuild':'PASS_FROZEN_SOURCE_AND_GENERATED_INPUTS_160_NEW_OBJECTS','functional_boundary_tests':'PASS_47_LISTED_CHECKS_PLUS_23_STRONG_RECHECKS','independent_equivalence':'PASS_SCOPED_78_DESIGNS_156_CHECKPOINTS'},'candidate_sha256':candidate,'cases':coverage,'checkpoint_methods':dict(methods),'boundary_unique_tests':sum(x['tests']for x in bound['groups'].values()),'stateful_complete_relation_designs':35,'audited_assertions':len(checks),'nine_area':area,'open_findings':{'resource_increases':resource['increases'],'baseline_bram':read(V/'flow-addendum.json'),'eqy_depth_limit_case':'35 designs covered by complete state relations;43 separately selected designs by EQY partitions/structural identity. Raw selected-only EQY status remains INCOMPLETE.','efficiency':'Pending one final rules132 batch for this newly built candidate','not_a_full_competition_acceptance':True},'new_rule_tests':{'status':'PASS','count':36,'summary':str(V/'native-rules/summary.json')},'formal_submission_unchanged':True,'scope':'Defined binary primitive semantics; actual power_up, clocks, async clear, next state and public outputs; no analog timing/arbitrary X/Z/device implementation claim'}
(V/'audit.json').write_text(json.dumps({'status':'PASS','checks':checks},indent=2),encoding='utf-8');(V/'result.json').write_text(json.dumps(result,indent=2),encoding='utf-8');print('AUDIT_PASS',len(checks),'CHECKPOINTS',sum(methods.values()),dict(methods))
