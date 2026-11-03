"""Verify actual pass-emitted transition wiring with independent SAT miters."""
from pathlib import Path
import hashlib,itertools,json,subprocess,os
R=Path(__file__).resolve().parent;O=R/'plan-v7-muxload/local-checks-v3';O.mkdir()
Y=R/'native/plan-v7-muxload-probe/yosys.exe'
P=R.parents[2]/'oss-cad-suite/bin/yosys.exe'
tests=[('plain','','','s','ena',True),('select_enable_alias','','','ena','ena',True),
 ('select_clear_alias','','','clrn','ena',True),('data_alias','','','a','ena',True),
 ('keep_mux','(* keep=1 *)','','s','ena',False),
 ('shared_output','','output wire tap,','s','ena',False),
 ('initial_q','','','s','ena',False),('alias_wire','','','s','ena',True)]
def run(w,script,log,proof=False):
    (w/'run.ys').write_text(script)
    with (w/log).open('wb') as f:
        env=dict(os.environ)
        env['PATH']=str(P.parent)+os.pathsep+str(P.parent.parent/'lib')+os.pathsep+env['PATH']
        return subprocess.run([str(P if proof else Y),'-Q','-s','run.ys'],cwd=w,env=env,stdout=f,stderr=subprocess.STDOUT,timeout=60).returncode

def transition(m):
    cells=m['cells'];ff=next(c for c in cells.values() if c['type']=='dffeas')
    drivers={b:c for c in cells.values() if c['type']=='$_MUX_' for b in c['connections']['Y']}
    def sig(b):
        if b in ('0','1'):return "1'b"+b
        if b in drivers:
            c=drivers[b]['connections']
            return '('+sig(c['S'][0])+'?'+sig(c['B'][0])+':'+sig(c['A'][0])+')'
        return 'v'+str(b)
    c=ff['connections'];p=lambda n:sig(c[n][0])
    return '('+p('clrn')+'?('+p('ena')+'?('+p('sclr')+"?1'b0:("+p('sload')+'?'+p('asdata')+':'+p('d')+')):'+p('q')+"):1'b0)"
report=[]
for name,attr,extra,sel,ena,expected in tests:
    w=O/name;w.mkdir()
    qattr='(* init=0 *) ' if name=='initial_q' else ''
    alias='wire alias_d; assign alias_d=d;' if name=='alias_wire' else ''
    data='alias_d' if name=='alias_wire' else 'd'
    tap='assign tap=d;' if name=='shared_output' else ''
    text=f'''module t(input wire clk,a,b,s,ena,clrn, {extra} {qattr}output wire q);
wire d; {alias}
{attr} \\$_MUX_ mux (.A(a),.B(b),.S({sel}),.Y(d));
dffeas ff (.clk(clk),.d({data}),.q(q),.clrn(clrn),.prn(1'b1),.ena({ena}),.asdata(1'b0),.aload(1'b0),.sclr(1'b0),.sload(1'b0));
{tap}
endmodule
'''
    (w/'fixture.v').write_text(text)
    script='read_verilog -sv -lib +/intel/cycloneiv/cells_sim.v\nread_verilog -icells fixture.v\nhierarchy -top t\nwrite_json before.json\nlut_ff_load\nwrite_json after.json\n'
    assert run(w,script,'emission.log')==0,name
    before=json.loads((w/'before.json').read_text())['modules']['t']
    after=json.loads((w/'after.json').read_text())['modules']['t']
    changed=not any(c['type']=='$_MUX_' for c in after['cells'].values())
    assert changed==expected,(name,changed)
    for n,d in before['netnames'].items():
        assert after['netnames'][n].get('attributes',{})==d.get('attributes',{})
    old=transition(before).replace('?', ' ? ').replace(':', ' : ')
    new=transition(after).replace('?', ' ? ').replace(':', ' : ')
    leaves=sorted({b for m in (before,after) for d in m['netnames'].values() for b in d['bits'] if isinstance(b,int)})
    inputs=','.join('input wire v'+str(b) for b in leaves)
    (w/'miter.v').write_text('module m('+inputs+',output wire bad); assign bad=('+old+')^('+new+'); endmodule\n')
    assert run(w,'read_verilog miter.v\nprep -top m\nsat -verify -prove bad 0 -set-def-inputs\n','sat.log',True)==0,name
    assert 'SAT proof finished - no model found: SUCCESS!' in (w/'sat.log').read_text()
    fault=False
    if changed:
        (w/'wrong.v').write_text('module m('+inputs+',output wire bad); assign bad=('+old+')^(~('+new+')); endmodule\n')
        fault=run(w,'read_verilog wrong.v\nprep -top m\nsat -verify -prove bad 0 -set-def-inputs\n','deliberate-fault.log',True)!=0
        assert 'proof did fail' in (w/'deliberate-fault.log').read_text()
        assert fault
    report.append({'fixture':name,'emitted_mux_absorption':changed,'transition_sat':'PASS','original_wire_attributes_preserved':True,'deliberate_fault_rejected':fault if changed else 'not applicable'})
out={'status':'PASS_LOCAL_TRANSITION_ONLY','candidate_sha256':hashlib.sha256(Y.read_bytes()).hexdigest(),'proof_tool_sha256':hashlib.sha256(P.read_bytes()).hexdigest(),'tests':report,'boundary':'Local data/enable/reset transition checks; not complete designs, all events/initial state proof, or efficiency acceptance.'}
(O/'summary.json').write_text(json.dumps(out,indent=2))
print(out['status'],len(report))
