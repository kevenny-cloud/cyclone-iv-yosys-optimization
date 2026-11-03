"""Complete many-to-one state relation, including all initial and event equations.

Names and truth tables propose matches only. Independent combinational SAT proves
every register equation and output for arbitrary binary inputs and related state.
No reset requirement, ignored register, or assumed internal cutpoint is allowed.
"""
import hashlib,json,os,shutil,subprocess,sys
from pathlib import Path
import prove_state_relation as base
import prove as guards
R=Path(__file__).resolve().parent
M=R/'matrix-startup1-stress/baseline_logic_unit_999'
TOP='baseline_logic_unit_999'
OUT=R.parents[2]/'pmux_work/startup1-relation'
OUT.mkdir(exist_ok=True)
sha=lambda p:hashlib.sha256(p.read_bytes()).hexdigest()

class Unmatched(Exception):pass

def initial_pairs(a,b):
    left,right=base.anchors(a),base.anchors(b)
    proposed={}
    for key in sorted(set(left)&set(right)):
        if '$' in key[0] or len(left[key])!=1 or len(right[key])!=1:continue
        (q,p),(r,s)=next(iter(left[key])),next(iter(right[key]))
        proposed.setdefault(q,set()).add((r,p^s))
    return {q:next(iter(x)) for q,x in proposed.items() if len(x)==1}

def signature(g,target,mapping,phase):
    ff=base.registers(g)
    drivers={}
    for c in g['cells'].values():
        if c['type']!='dffeas':
            pins,y,mask=base.lut(c);drivers[y]=(pins,mask)
    inputs={bit:('i',n,i) for n,p in g['ports'].items() if p['direction']=='input' for i,bit in enumerate(p['bits'])}
    supports={}
    def support(b):
        if isinstance(b,str):return set()
        if b in supports:return supports[b]
        if b in inputs:v={inputs[b]}
        elif b in ff:
            if b not in mapping:raise Unmatched(b)
            v={('q',mapping[b][0])}
        else:v=set().union(*(support(x) for x in drivers[b][0]))
        supports[b]=v;return v
    p=ff[target][1]['connections']
    fields=('clk','clrn','ena','sclr','sload','asdata','d','q')
    variables=sorted(set().union(*(support(p[n][0]) for n in fields)))
    if len(variables)>20:raise Unmatched('support too large')
    size=1<<len(variables);ones=(1<<size)-1
    tables={}
    for i,v in enumerate(variables):
        tables[v]=(ones//((1<<(1<<i))+1))<<(1<<i)
    values={}
    def evaluate(b):
        if isinstance(b,str):
            assert b in ('0','1');return ones if b=='1' else 0
        if b in values:return values[b]
        if b in inputs:v=tables[inputs[b]]
        elif b in ff:
            q,flip=mapping[b];v=tables[('q',q)]^(ones if flip else 0)
        else:
            pins,mask=drivers[b];pin_values=[evaluate(x) for x in pins];v=0
            for pattern in range(1<<len(pins)):
                if not (mask>>pattern)&1:continue
                term=ones
                for i,pv in enumerate(pin_values):term&=pv if (pattern>>i)&1 else pv^ones
                v|=term
        values[b]=v;return v
    t={n:evaluate(p[n][0]) for n in fields}
    data=((t['sload']&t['asdata'])|((t['sload']^ones)&t['d']))&(t['sclr']^ones)
    nxt=t['clrn']&((t['ena']&data)|((t['ena']^ones)&t['q']))
    reset=t['clrn']&t['q'];flip=ones if phase else 0
    cache={}
    def canonical(table,n):
        if table==0:return 0
        if table==(1<<(1<<n))-1:return 1
        key=(table,n)
        if key in cache:return cache[key]
        half=1<<(n-1);low=canonical(table&((1<<half)-1),n-1);high=canonical(table>>half,n-1)
        result=low if low==high else (variables[n-1],low,high)
        cache[key]=result;return result
    return tuple(canonical(x,len(variables)) for x in (t['clk'],reset^flip,nxt^flip))

def propose(a,b):
    fa,fb=base.registers(a),base.registers(b)
    pairs=initial_pairs(a,b)
    for q,(r,p) in pairs.items():
        assert (fa[q][1]['parameters']['power_up']=='high')==((fb[r][1]['parameters']['power_up']=='high')^bool(p)),('Initial mismatch',q,r)
    right={r:(r,0) for r in fb}
    cached={}
    def origin(cell):return cell['attributes'].get('src','').split('|')[0]
    while len(pairs)<len(fa):
        count=len(pairs)
        for q in sorted(set(fa)-set(pairs)):
            for r in sorted(fb):
                if origin(fa[q][1])!=origin(fb[r][1]):continue
                phase=int((fa[q][1]['parameters']['power_up']=='high')!=(fb[r][1]['parameters']['power_up']=='high'))
                test=dict(pairs);test[q]=(r,phase)
                try:
                    lhs=signature(a,q,test,phase)
                    if r not in cached:cached[r]=signature(b,r,right,0)
                except Unmatched:continue
                if lhs==cached[r]:pairs[q]=(r,phase);break
        print('matched',len(pairs),'/',len(fa),'covered',len({r for r,p in pairs.values()}),'/',len(fb),flush=True)
        if len(pairs)==count:
            left=sorted(set(fa)-set(pairs));right=sorted(set(fb)-{r for r,p in pairs.values()})
            if len(left)==len(right)==1:
                q,r=left[0],right[0]
                phase=int((fa[q][1]['parameters']['power_up']=='high')!=(fb[r][1]['parameters']['power_up']=='high'))
                pairs[q]=(r,phase)
                print('Proposed last remaining pair; independent full SAT must verify it.',flush=True)
                continue
            for q in sorted(set(fa)-set(pairs)):print('LEFT',q,fa[q],flush=True)
            for q in sorted(set(fb)-{r for r,p in pairs.values()}):print('RIGHT',q,fb[q],flush=True)
            raise RuntimeError(('Unresolved state mapping',sorted(set(fa)-set(pairs))))
    assert {r for r,p in pairs.values()}==set(fb)
    return pairs

def emit(g,name,state,order,width):
    ff=base.registers(g);assert set(state)==set(ff)
    inputs=[(n,i,b) for n,p in sorted(g['ports'].items()) if p['direction']=='input' for i,b in enumerate(p['bits'])]
    assert len({b for n,i,b in inputs})==len(inputs)
    outputs=[b for n,p in sorted(g['ports'].items()) if p['direction']=='output' for b in p['bits']]
    used=set();drivers=set();code=[]
    def bit(b):
        assert isinstance(b,int) or b in ('0','1')
        if isinstance(b,int):used.add(b);return 'b'+str(b)
        return "1'b"+b
    def drive(b):
        assert isinstance(b,int) and b not in drivers;drivers.add(b);return bit(b)
    for i,(n,j,b) in enumerate(inputs):code.append(f'assign {drive(b)}=pi[{i}];')
    for q,(i,p) in sorted(state.items()):code.append(f"assign {drive(q)}=st[{i}] ^ 1'b{p};")
    for c in g['cells'].values():
        if c['type']=='dffeas':continue
        pins,y,mask=base.lut(c);index='{'+','.join(bit(b) for b in reversed(pins))+'}'
        code.append(f"assign {drive(y)}={1<<len(pins)}'h{mask:x} >> {index};")
    compared=[bit(b) for b in outputs]
    for q in order:
        p=ff[q][1]['connections'];v=lambda n:bit(p[n][0]);phase=state[q][1]
        reset=f"({v('clrn')} ? {v('q')} : 1'b0)"
        nxt=f"({v('clrn')} ? ({v('ena')} ? ({v('sclr')} ? 1'b0 : ({v('sload')} ? {v('asdata')} : {v('d')})) : {v('q')}) : 1'b0)"
        compared.extend([v('clk'),f"({reset} ^ 1'b{phase})",f"({nxt} ^ 1'b{phase})"])
    assert used<=drivers,('Undriven bits',used-drivers)
    code.append('assign po={'+','.join(reversed(compared))+'};')
    return f'module {name}(input [{len(inputs)-1}:0] pi,input [{width-1}:0] st,output [{len(compared)-1}:0] po);\n'+'\n'.join('wire b'+str(b)+';' for b in sorted(used))+'\n'+'\n'.join(code)+'\nendmodule\n'

def main():
    report={'status':'RUNNING','scope':__doc__,'checkpoints':{}}
    env=dict(os.environ);env['PATH']=str(guards.SUITE/'bin')+os.pathsep+str(guards.SUITE/'lib')+os.pathsep+env['PATH']
    try:
      for checkpoint in ('checkpoint','final'):
        out=OUT/checkpoint;out.mkdir(exist_ok=True)
        paths=[M/v/(checkpoint+'.json') for v in ('baseline','candidate')]
        a,b=[json.loads(p.read_text())['modules'][TOP] for p in paths]
        for p in paths:guards.guard(p,TOP,True,True)
        pairs=propose(a,b);fa,fb=base.registers(a),base.registers(b)
        order=sorted(fa);index={r:i for i,r in enumerate(sorted(fb))}
        sa={q:(index[r],phase) for q,(r,phase) in pairs.items()};sb={r:(index[r],0) for r in fb}
        text=emit(a,'gold',sa,order,len(fb))+emit(b,'gate',sb,[pairs[q][0] for q in order],len(fb))
        (out/'relation.v').write_text(text,encoding='utf-8')
        (out/'run.ys').write_text('read_verilog relation.v\nmiter -equiv -flatten gold gate miter\nprep -top miter\nsat -verify -timeout 60 -prove trigger 0 -set-def-inputs -show-inputs -show-outputs miter\n')
        with (out/'run.log').open('wb') as log:p=subprocess.run([str(guards.YOSYS),'-Q','-T','-s','run.ys'],cwd=out,env=env,stdout=log,stderr=subprocess.STDOUT,timeout=90)
        success=p.returncode==0 and 'SAT proof finished - no model found: SUCCESS!' in (out/'run.log').read_text(errors='replace')
        report['checkpoints'][checkpoint]={'status':'PASS' if success else 'UNPROVEN','gold_registers':len(fa),'gate_registers':len(fb),'all_registers_covered':True,'pairs':[[q,r,phase] for q,(r,phase) in sorted(pairs.items())],'input_hashes':[sha(p) for p in paths],'proof_hashes':{n:sha(out/n) for n in ('relation.v','run.ys','run.log')}}
        assert success,out/'run.log'
        print(checkpoint,'COMPLETE_STATE_RELATION_PASS',flush=True)
      report['status']='PASS'
    finally:
      (OUT/'summary.json').write_text(json.dumps(report,ensure_ascii=False,indent=2)+'\n',encoding='utf-8')
      shutil.copy2(__file__,OUT/'proof_driver.py')
if __name__=='__main__':main()
