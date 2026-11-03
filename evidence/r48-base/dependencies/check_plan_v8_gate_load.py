"""Reuse independent transition miter infrastructure with explicit gate formulas."""
from pathlib import Path
R=Path(__file__).resolve().parent
t=(R/'check_plan_v7_muxload.py').read_text()
t=t.replace('plan-v7-muxload/local-checks-v3','plan-v8-gate-load/local-checks').replace('plan-v7-muxload-probe','plan-v8-gate-load-probe')
t=t.replace("('alias_wire','','','s','ena',True)]", "('alias_wire','','','s','ena',True),('and_gate','','','s','ena',True),('or_gate','','','s','ena',True),('not_gate','','','s','ena',True)]")
t=t.replace("if c['type']=='$_MUX_' for b", "if c['type'] in ('$_MUX_','$_AND_','$_OR_','$_NOT_') for b")
t=t.replace("            return '('+sig(c['S'][0])+'?'+sig(c['B'][0])+':'+sig(c['A'][0])+')'", """            kind=drivers[b]['type']
            if kind=='$_NOT_':return '(~'+sig(c['A'][0])+')'
            if kind=='$_AND_':return '('+sig(c['A'][0])+'&'+sig(c['B'][0])+')'
            if kind=='$_OR_':return '('+sig(c['A'][0])+'|'+sig(c['B'][0])+')'
            return '('+sig(c['S'][0])+'?'+sig(c['B'][0])+':'+sig(c['A'][0])+')'""")
t=t.replace("    (w/'fixture.v').write_text(text)", """    if name in ('and_gate','or_gate','not_gate'):
        kind={'and_gate':'$_AND_','or_gate':'$_OR_','not_gate':'$_NOT_'}[name]
        text=text.replace('$_MUX_',kind).replace('.S(s),','')
        if kind=='$_NOT_':text=text.replace('.B(b),','')
    (w/'fixture.v').write_text(text)""")
t=t.replace("changed=not any(c['type']=='$_MUX_' for c in after['cells'].values())", "changed='mux' not in after['cells']")
exec(compile(t,str(R/'check_plan_v8_gate_load.py'),'exec'))
