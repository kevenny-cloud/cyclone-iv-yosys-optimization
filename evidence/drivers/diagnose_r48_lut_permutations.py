"""Exact permutation experiment on frozen checkpoints; not a promoted candidate."""
from pathlib import Path
import json,itertools,collections,subprocess,os,hashlib
R=Path(__file__).resolve().parent
D=R/'continuation-20261030/next-plan-execution'
I=D/'memory-diagnostic-v1';O=D/'permutation-experiment-v1'
assert not O.exists();O.mkdir()
cache={}
def canonical(n,t):
 key=(n,t)
 if key in cache:return cache[key]
 best=(t,tuple(range(n)))
 for p in itertools.permutations(range(n)):
  q=0
  for a in range(1<<n):
   old=sum(((a>>j)&1)<<p[j]for j in range(n))
   q|=((t>>old)&1)<<a
  if q<best[0]:best=(q,p)
 cache[key]=best;return best
report={'status':'DIAGNOSTIC_ONLY','cases':{}}
for w in sorted(I.iterdir()):
 if not w.is_dir():continue
 for v in ['baseline','candidate']:
  src=w/v/'checkpoint.json';g=json.loads(src.read_text());before=set();after=set();changed=0
  for m in g['modules'].values():
   if m.get('attributes',{}).get('blackbox')=='00000000000000000000000000000001':continue
   for c in m.get('cells',{}).values():
    if c['type']!='$lut' or c.get('attributes',{}).get('keep'):continue
    pins=c['connections']['A'];n=len(pins);s=c['parameters']['LUT']
    if n>4 or any(x not in '01'for x in s):continue
    t=int(s,2);q,p=canonical(n,t);before.add((n,t));after.add((n,q))
    # Check every old input assignment against the emitted pin ordering.
    for a in range(1<<n):
     new=sum(((a>>p[j])&1)<<j for j in range(n))
     assert ((t>>a)&1)==((q>>new)&1)
    if q!=t:
     c['connections']['A']=[pins[j]for j in p];c['parameters']['LUT']=format(q,'0%db'%len(s));changed+=1
  out=O/w.name/v;out.mkdir(parents=True);(out/'canonical.json').write_text(json.dumps(g),encoding='utf-8')
  report['cases'].setdefault(w.name,{})[v]={'distinct_before':len(before),'distinct_after':len(after),'changed':changed,'all_truth_assignments_checked':True}
  print(w.name,v,len(before),'->',len(after),flush=True)
(O/'summary.json').write_text(json.dumps(report,indent=2),encoding='utf-8')
