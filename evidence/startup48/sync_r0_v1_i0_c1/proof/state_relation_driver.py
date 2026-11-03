"""Prove a complete bijective register relation with independent combinational SAT.

Every register is paired, initial values agree under the proposed polarity, and
all clock, asynchronous-clear and next-state equations and public outputs are
compared for arbitrary inputs/current state. No reachable-state assumptions,
input constraints, omitted registers or omitted outputs are permitted.
This is restricted to the same binary primitive semantics as prove.py.
"""
import sys
sys.dont_write_bytecode = True
from pathlib import Path
import argparse, hashlib, itertools, json, os, shutil, subprocess
R = Path(__file__).resolve().parent
YOSYS = R.parents[2]/'oss-cad-suite/bin/yosys.exe'
read = lambda p: json.loads(p.read_text(encoding='utf-8-sig'))
sha = lambda p: hashlib.sha256(p.read_bytes()).hexdigest()


def registers(g):
    result = {}
    for name, c in g['cells'].items():
        if c['type'] != 'dffeas':
            continue
        p = c['connections']
        assert set(p) == {'d','q','clk','clrn','prn','ena','asdata','aload','sclr','sload'}
        assert all(len(b) == 1 for b in p.values())
        assert p['prn'] == ['1'] and p['aload'] == ['0']
        assert c['parameters']['power_up'] in ('low','high')
        q = p['q'][0]
        assert isinstance(q,int) and q not in result
        result[q] = (name,c)
    return result


def lut(c):
    p = c['connections']
    if c['type'] == '$not':
        assert len(p['A']) == len(p['Y']) == 1
        return p['A'], p['Y'][0], 1
    if c['type'] == '$lut':
        n = int(c['parameters']['WIDTH'],2)
        assert 1 <= n <= 4 and len(p['A']) == n and len(p['Y']) == 1
        mask = c['parameters']['LUT']
        assert len(mask) == 1 << n and set(mask) <= {'0','1'}
        return p['A'], p['Y'][0], int(mask,2)
    assert c['type'] == 'cycloneiv_lcell_comb', c['type']
    assert c['parameters'].get('sum_lutc_input','datac') == 'datac'
    mask = c['parameters']['lut_mask']
    assert len(mask) == 16 and set(mask) <= {'0','1'}
    return [p[n][0] for n in ('dataa','datab','datac','datad')], p['combout'][0], int(mask,2)


def anchors(g):
    # Names only propose a relation. SAT must prove the complete relation later.
    ff = registers(g)
    aliases = {}
    for name, net in g['netnames'].items():
        if net.get('hide_name',0):
            continue
        for i,b in enumerate(net['bits']):
            aliases.setdefault(b,set()).add((name,i+net.get('offset',0)))
    unary = {}
    for c in g['cells'].values():
        if c['type'] == 'dffeas':
            continue
        pins,y,mask = lut(c)
        bits = sorted({b for b in pins if isinstance(b,int)})
        if len(bits) != 1:
            continue
        def value(x):
            a = sum((x if isinstance(b,int) else int(b)) << i for i,b in enumerate(pins))
            return (mask >> a) & 1
        lo,hi = value(0),value(1)
        if lo != hi:
            unary.setdefault(bits[0],[]).append((y,lo))
    result = {}
    for q in ff:
        stack = [(q,0)]; seen = set()
        while stack:
            bit,phase = stack.pop()
            if bit in seen:
                continue
            seen.add(bit)
            for name in aliases.get(bit,[]):
                result.setdefault(name,set()).add((q,phase))
            stack.extend((b,phase^p) for b,p in unary.get(bit,[]))
    return result


def propose(a,b):
    left,right = anchors(a),anchors(b)
    pairs = {}; used = set()
    for name in sorted(set(left)&set(right)):
        if len(left[name]) != 1 or len(right[name]) != 1:
            continue
        (q,p),(r,s) = next(iter(left[name])),next(iter(right[name]))
        item = (r,p^s)
        if q in pairs:
            assert pairs[q] == item, ('Conflicting state aliases',name)
        elif r not in used:
            pairs[q] = item; used.add(r)
    return pairs


def emit(g, name, state):
    # state: graph Q bit -> (common-state index, polarity)
    ff = registers(g)
    assert set(state) == set(ff)
    assert len({i for i,p in state.values()}) == len(state)
    inputs = [(n,i,b) for n,p in sorted(g['ports'].items()) if p['direction']=='input'
              for i,b in enumerate(p['bits'])]
    assert len({b for n,i,b in inputs}) == len(inputs), 'Aliased input ports unsupported'
    outputs = [b for n,p in sorted(g['ports'].items()) if p['direction']=='output' for b in p['bits']]
    assert all(p['direction'] in ('input','output') for p in g['ports'].values())
    drivers = set(); used = set()
    def bit(b):
        assert isinstance(b,int) or b in ('0','1'), 'Undefined bit'
        if isinstance(b,int):
            used.add(b)
            return 'b'+str(b)
        return "1'b"+b
    def drive(b):
        assert isinstance(b,int) and b not in drivers, ('Invalid/multiple driver',b)
        drivers.add(b)
        return bit(b)
    code = []
    for i,(_,_,b) in enumerate(inputs):
        code.append(f'assign {drive(b)} = pi[{i}];')
    for q,(i,p) in state.items():
        code.append(f"assign {drive(q)} = st[{i}] ^ 1'b{p};")
    combinational={}
    for c in g['cells'].values():
        if c['type']=='dffeas':
            continue
        pins,y,mask = lut(c)
        combinational[y]={b for b in pins if isinstance(b,int)}
        # Dedicated COUT must never be observed in this restricted model.
        if c['type']=='cycloneiv_lcell_comb':
            cout = set(c['connections'].get('cout',[]))
            consumers = set(outputs)
            for other in g['cells'].values():
                for port,direction in other['port_directions'].items():
                    if direction=='input': consumers.update(other['connections'][port])
            assert not (cout & consumers)
        index = '{'+','.join(bit(b) for b in reversed(pins))+'}'
        code.append(f"assign {drive(y)} = {1<<len(pins)}'h{mask:x} >> {index};")
    compared = [bit(b) for b in outputs]
    # Equal physical clocks, reset behavior between edges, and clock-edge
    # transitions establish the relation for every discrete event ordering.
    for q,(i,phase) in sorted(state.items(),key=lambda x:x[1][0]):
        p = ff[q][1]['connections']; v = lambda n: bit(p[n][0])
        reset = f"({v('clrn')} ? {v('q')} : 1'b0)"
        nextq = f"({v('clrn')} ? ({v('ena')} ? ({v('sclr')} ? 1'b0 : ({v('sload')} ? {v('asdata')} : {v('d')})) : {v('q')}) : 1'b0)"
        compared += [v('clk'), f"({reset} ^ 1'b{phase})", f"({nextq} ^ 1'b{phase})"]
    code.append('assign po = {'+','.join(reversed(compared))+'};')
    assert used <= drivers, ('Undriven logic',used-drivers)
    resolved={b for _,_,b in inputs}|set(ff)
    pending=dict(combinational)
    while pending:
        ready={b for b,pins in pending.items() if pins<=resolved}
        assert ready, 'Combinational cycle or undriven dependency'
        resolved.update(ready)
        pending={b:pins for b,pins in pending.items() if b not in ready}
    header = f'module {name}(input [{len(inputs)-1}:0] pi,input [{len(ff)-1}:0] st,output [{len(compared)-1}:0] po);'
    return header+'\n'+'\n'.join('wire b'+str(b)+';' for b in sorted(used))+'\n'+'\n'.join(code)+'\nendmodule\n', len(compared)


def validate_pair(a,b,pairs):
    fa,fb = registers(a),registers(b)
    assert set(pairs)==set(fa) and {r for r,p in pairs.values()}==set(fb)
    for q,(r,phase) in pairs.items():
        assert (fa[q][1]['parameters']['power_up']=='high') == ((fb[r][1]['parameters']['power_up']=='high') ^ bool(phase))
    assert {n:(p['direction'],len(p['bits'])) for n,p in a['ports'].items()} == {n:(p['direction'],len(p['bits'])) for n,p in b['ports'].items()}
    sa = {q:(i,0) for i,q in enumerate(sorted(fa))}
    sb = {pairs[q][0]:(i,pairs[q][1]) for i,q in enumerate(sorted(fa))}
    gold,n = emit(a,'gold',sa); gate,m = emit(b,'gate',sb)
    assert n==m
    return gold+gate,n


def prove(a,b,out,max_unmatched=4):
    out.mkdir(exist_ok=False)
    shutil.copy2(Path(__file__),out/'state_relation_driver.py')
    fa,fb = registers(a),registers(b)
    assert len(fa)==len(fb)>0
    fixed = propose(a,b)
    left = sorted(set(fa)-set(fixed));right = sorted(set(fb)-{r for r,p in fixed.values()})
    assert len(left)==len(right)<=max_unmatched, ('State matching unresolved',len(left))
    report = {'status':'UNPROVEN','fixed_state_pairs':len(fixed),'unmatched_states':len(left),
              'registers':len(fa),'proof_tool':str(YOSYS),'proof_tool_sha256':sha(YOSYS),
              'driver_sha256':sha(out/'state_relation_driver.py'),'attempts':[],
              'scope':__doc__}
    env = dict(os.environ);suite = YOSYS.parent.parent
    env['PATH']=str(suite/'bin')+os.pathsep+str(suite/'lib')+os.pathsep+env['PATH']
    try:
        for perm in itertools.permutations(right):
            for phases in itertools.product((0,1),repeat=len(left)):
                pairs = dict(fixed);pairs.update({q:(r,p) for q,r,p in zip(left,perm,phases)})
                try: verilog,width = validate_pair(a,b,pairs)
                except AssertionError: continue
                f = out/('attempt'+str(len(report['attempts'])));f.mkdir()
                (f/'relation.v').write_text(verilog,encoding='utf-8',newline='\n')
                script = 'read_verilog relation.v\nmiter -equiv -flatten gold gate miter\nprep -top miter\nsat -verify -prove trigger 0 -set-def-inputs -show-inputs -show-outputs miter\n'
                (f/'run.ys').write_text(script,encoding='utf-8',newline='\n')
                with (f/'run.log').open('wb') as log:
                    run = subprocess.run([str(YOSYS),'-Q','-T','-s','run.ys'],cwd=f,env=env,
                                         stdout=log,stderr=subprocess.STDOUT,timeout=120,creationflags=subprocess.CREATE_NO_WINDOW)
                logtext=(f/'run.log').read_text(encoding='utf-8')
                passed = run.returncode==0 and 'SAT proof finished - no model found: SUCCESS!' in logtext
                if not passed:
                    assert 'proof did fail' in logtext or 'SAT proof finished - model found: FAIL!' in logtext, 'SAT invocation failed without a counterexample: '+str(f)
                attempt = {'status':'PASS' if passed else 'UNPROVEN','directory':f.name,
                           'pairs':[[q,r,p] for q,(r,p) in sorted(pairs.items())],
                           'compared_bits':width,'returncode':run.returncode,
                           'hashes':{n:sha(f/n) for n in ('relation.v','run.ys','run.log')}}
                report['attempts'].append(attempt)
                if passed:
                    report.update(status='STATE_TRANSITION_SAT_PASS',successful_attempt=len(report['attempts'])-1)
                    return report
        return report
    finally:
        (out/'summary.json').write_text(json.dumps(report,ensure_ascii=False,indent=2)+'\n',encoding='utf-8')


def audit(a,b,folder):
    report = read(folder/'summary.json')
    assert report['status']=='STATE_TRANSITION_SAT_PASS'
    assert sha(YOSYS)==report['proof_tool_sha256'] and sha(folder/'state_relation_driver.py')==report['driver_sha256']
    attempt = report['attempts'][report['successful_attempt']]
    f = folder/attempt['directory']
    assert all(sha(f/n)==h for n,h in attempt['hashes'].items())
    verilog,width = validate_pair(a,b,{q:(r,p) for q,r,p in attempt['pairs']})
    assert (f/'relation.v').read_text(encoding='utf-8')==verilog and width==attempt['compared_bits']
    assert 'SAT proof finished - no model found: SUCCESS!' in (f/'run.log').read_text(encoding='utf-8')
    assert (f/'run.ys').read_text(encoding='utf-8')=='read_verilog relation.v\nmiter -equiv -flatten gold gate miter\nprep -top miter\nsat -verify -prove trigger 0 -set-def-inputs -show-inputs -show-outputs miter\n'
    return report


if __name__=='__main__':
    ap=argparse.ArgumentParser();ap.add_argument('--matrix',required=True);ap.add_argument('--case',required=True);ap.add_argument('--checkpoint',choices=('checkpoint','final'),default='final');ap.add_argument('--out',required=True);a=ap.parse_args()
    f=R/a.matrix/a.case;top=read(R/a.matrix/'summary.json')['cases'][a.case]['top']
    graphs=[read(f/v/(a.checkpoint+'.json'))['modules'][top] for v in ('baseline','candidate')]
    report=prove(*graphs,R/a.out)
    print(report['status'],report['registers'],'registers',len(report['attempts']),'attempts')
    assert report['status']=='STATE_TRANSITION_SAT_PASS'
