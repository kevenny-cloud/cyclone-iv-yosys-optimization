"""Evaluate row132 measurements without rerunning or discarding any sample."""
import argparse
import json
import math
from pathlib import Path


def valid(x):
    return (x['exit_code'] == 0 and all(isinstance(x[k], (int, float))
            and math.isfinite(x[k]) and x[k] > 0 for k in
            ('elapsed_seconds', 'main_peak_working_set_bytes')))


def ratio(pairs, field):
    return (sum(p['candidate'][field] for p in pairs) /
            sum(p['baseline'][field] for p in pairs) - 1) * 100


def evaluate(report):
    out = {'status': 'FAIL', 'schema': 'RULE132_V1', 'cases': [], 'errors': []}
    try:
        assert report['schema'] == 'RULE132_V1' and report['status'] == 'COMPLETE'
        assert report['warmup_baseline_per_case'] == 10 and report['review_count'] == 5
        assert report['primary_order'] == ['baseline', 'candidate']
        ids = [c['id'] for c in report['results']]
        assert ids == report['cases'] and len(ids) == len(set(ids))
        for c in report['results']:
            assert len(c['warmup']) == 10 and all(x['variant'] == 'baseline' and valid(x) for x in c['warmup'])
            primary = c['primary']
            assert list(primary) == ['baseline', 'candidate']
            assert all(primary[v]['variant'] == v and valid(primary[v]) for v in primary)
            small = all(primary[v]['elapsed_seconds'] < 1 for v in primary)
            initial = {f: ratio([primary], f) for f in ('elapsed_seconds', 'main_peak_working_set_bytes')}
            triggered = small and any(x > 5 for x in initial.values())
            reviews = c['review']
            assert len(reviews) == (5 if triggered else 0), 'Missing/excess review pairs'
            for i, p in enumerate(reviews):
                assert p['index'] == i and all(p[v]['variant'] == v and valid(p[v]) for v in primary)
            chosen = reviews if triggered else [primary]
            final = {f: ratio(chosen, f) for f in initial}
            all_measured = [*primary.values(), *(x[v] for x in reviews for v in primary)]
            absolute = all(x['elapsed_seconds'] <= 60 and x['main_peak_working_set_bytes'] <= 2*1024**3 for x in all_measured)
            out['cases'].append({'id': c['id'], 'initial_overhead_percent': initial,
                                 'review_triggered': triggered, 'decision_overhead_percent': final,
                                 'pass': absolute and all(x <= 5 for x in final.values())})
        out['status'] = 'PASS_EFFICIENCY_ONLY' if all(c['pass'] for c in out['cases']) else 'FAIL_EFFICIENCY'
    except (AssertionError, KeyError, TypeError, ZeroDivisionError) as e:
        out['errors'].append(str(e) or 'Malformed/incomplete measurement')
    return out


if __name__ == '__main__':
    ap = argparse.ArgumentParser();ap.add_argument('report', type=Path);ap.add_argument('--out', type=Path, required=True)
    a = ap.parse_args();out = evaluate(json.loads(a.report.read_text(encoding='utf-8-sig')))
    a.out.write_text(json.dumps(out, ensure_ascii=False, indent=2)+'\n', encoding='utf-8')
    print(out['status']);raise SystemExit(0 if out['status'] == 'PASS_EFFICIENCY_ONLY' else 1)
