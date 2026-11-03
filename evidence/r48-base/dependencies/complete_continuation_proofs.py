"""Run retained boundary and independent proof procedures for a clean candidate."""
from pathlib import Path
import hashlib, importlib.util, json, os, shutil, subprocess, sys

R = Path(__file__).resolve().parent
label = sys.argv[1]
revision = label.split('-')[0]
V = R / 'continuation-20261030' / label
env = dict(os.environ, PYTHONUTF8='1')
sha = lambda p: hashlib.sha256(p.read_bytes()).hexdigest()
read = lambda p: json.loads(p.read_text(encoding='utf-8-sig'))

for script in [f'validate_{revision}_boundaries.py',
               f'test_continuation_local_{revision}.py',
               f'validate_{revision}_strong_miters.py',
               f'validate_{revision}_state_relations.py']:
    with (V / (Path(script).stem + '.log')).open('wb') as out:
        subprocess.run([sys.executable, str(R/script)], cwd=R, env=env,
                       stdout=out, stderr=subprocess.STDOUT, check=True)
    print('PASS', script, flush=True)

matrix = read(V/'matrix/summary.json')
state = read(V/'state-relations/summary.json')
names = [n for n in matrix['cases'] if n not in state['cases']]
assert len(names) == 43 and len(state['cases']) == 35
args = [sys.executable, str(R/'prove.py'), '--matrix', str(V/'matrix'),
        '--model', str(R/'models/enable2-clean1/primitive_decode_sync.v'),
        '--sync-controls', '--sync-load-data', '--short-dirs', '--depth', '30']
for name in names:
    args += ['--case', name]
with (V/'eqy-run.log').open('wb') as out:
    subprocess.run(args, cwd=R, env=env, stdout=out,
                   stderr=subprocess.STDOUT, check=True)
print('PASS independent partitions for43 selected designs', flush=True)

O = V/'default-rom-functional'
W = O/'candidate'
W.mkdir(parents=True, exist_ok=True)
S = V/'matrix/fresh_08/candidate'
shutil.copy2(S/'input.v', W/'input.v')
(W/'run.ys').write_text((S/'run.ys').read_text().replace(' -nobram', ''), encoding='utf-8')
with (W/'run.log').open('wb') as out:
    p = subprocess.run([str(V/'program/yosys.exe'), '-Q', '-s', 'run.ys'],
                       cwd=W, stdout=out, stderr=subprocess.STDOUT, check=True)
spec = importlib.util.spec_from_file_location('stateproof', R/'prove_state_relation.py')
sr = importlib.util.module_from_spec(spec)
spec.loader.exec_module(sr)
for cp in ['checkpoint', 'final']:
    gs = [read(q)['modules']['fresh_08'] for q in
          [V/'matrix/fresh_08/baseline'/(cp+'.json'), W/(cp+'.json')]]
    d = sr.prove(*gs, O/cp)
    assert d['status'] == 'STATE_TRANSITION_SAT_PASS'
    sr.audit(*gs, O/cp)
f = R/'validation-r10-20261030/matrix/fresh_08/default-flow-baseline-failed.log'
(V/'flow-addendum.json').write_text(json.dumps({'fresh_08': {
    'synth_options': ['-nobram'],
    'reason': 'Original same-toolchain baseline default BRAM map cannot resolve defparam INIT. Preserve failure; same supplementary -nobram in both arms. Not a default-BRAM PASS.',
    'default_baseline_failure_sha256': sha(f),
    'default_candidate_returncode': p.returncode,
    'current_default_candidate_log_sha256': sha(W/'run.log'),
    'original_failure_record': str(f)}}, indent=2), encoding='utf-8')
subprocess.run([sys.executable, str(R/f'audit_{revision}_validation.py')],
               cwd=R, env=env, check=True)
cfg = {}
for variant, directory in [('baseline', R/'native/windows-reference'), ('candidate', V/'program')]:
    cfg[variant] = {'yosys': str(directory/'yosys.exe'), 'yosys_sha256': sha(directory/'yosys.exe'),
                    'abc': str(directory/'yosys-abc.exe'), 'abc_sha256': sha(directory/'yosys-abc.exe'),
                    'data': str(directory/'share'), 'runtime_sha256': {
                        str(q.relative_to(directory/'share')): sha(q)
                        for q in sorted((directory/'share').rglob('*.v'))}}
(V/'timing-config.json').write_text(json.dumps(cfg, indent=2), encoding='utf-8')
(V/'timing-pointer.json').write_text(json.dumps({'results': str(V/'matrix')}, indent=2), encoding='utf-8')
print('ALL_REQUIRED_PROOFS_AUDITED_READY_FOR_ONE_TIMING_BATCH', flush=True)
