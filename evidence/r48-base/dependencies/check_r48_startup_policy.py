"""Reset/clock polarity and explicit-init regressions for the startup policy."""
import importlib.util,itertools,json,sys
from pathlib import Path
sys.dont_write_bytecode=True
import prove_state_relation as relation
R=Path(__file__).resolve().parent
OUT=R/'continuation-20261030/r48-one-snapshot-clean/startup48'
assert not OUT.exists()
OUT.mkdir(exist_ok=True)
cfg=json.loads((R/'continuation-20261030/r48-one-snapshot-clean/startup-config.json').read_text(encoding='utf-8-sig'))
spec=importlib.util.spec_from_file_location('measure',R/'user-stress999-enable2-clean1/measure_baseline.py');measure=importlib.util.module_from_spec(spec);spec.loader.exec_module(measure)
report={'status':'RUNNING','cases':{}}
try:
 for async_reset,reset_positive,reset_value,initial,clock_positive in itertools.product((False,True),(False,True),(0,1),(None,0,1),(False,True)):
    name=f'{"async" if async_reset else "sync"}_r{int(reset_positive)}_v{reset_value}_i{initial}_c{int(clock_positive)}'
    folder=OUT/name;folder.mkdir(exist_ok=True)
    clock='posedge' if clock_positive else 'negedge'
    reset_edge='posedge' if reset_positive else 'negedge'
    reset='rst' if reset_positive else '!rst'
    event=f'{clock} clk'+(f' or {reset_edge} rst' if async_reset else '')
    init='' if initial is None else f"=1'b{initial}"
    rtl=f'''module startup_policy(input clk,rst,en,input [1:0] sel,input a,b,c,d,output reg [1:0] y,output reg q{init},output reg sampled);
always @* case(sel) 2'd0:y={{a,sel[0]}}; 2'd1:y={{b,sel[1]}}; default:y={{c,d}}; endcase
always @({event}) if({reset}) q<=1'b{reset_value}; else if(en) q<=a^b;
always @(posedge clk) if(en) sampled<=q;
endmodule
'''
    (folder/'input.v').write_text(rtl)
    for variant in ('baseline','candidate'):
        measure.measure([folder/'input.v'],'startup_policy',Path(cfg[variant]['yosys']),folder/variant,True)
    a,b=[json.loads((folder/v/'final.json').read_text())['modules']['startup_policy'] for v in ('baseline','candidate')]
    result=relation.prove(a,b,folder/'proof')
    assert result['status']=='STATE_TRANSITION_SAT_PASS',name
    report['cases'][name]={'status':'PASS','registers':result['registers'],'initial':initial,'asynchronous_reset':async_reset}
    (OUT/'summary.json').write_text(json.dumps(report,indent=2)+'\n')
    print(name,'PASS',flush=True)
 report['status']='PASS'
finally:
 (OUT/'summary.json').write_text(json.dumps(report,indent=2)+'\n')
