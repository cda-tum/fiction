# Contract audits (SpecAudits)

A SpecAudit investigates a concrete concern about tests and the code they constrain.
Seek useful changes, not a finding count. Follow [AGENTS.md](../AGENTS.md) and the
instructions for the source and test subtrees in scope. Report results directly;
save a record in `.ai/audits/<scope-slug>.md` only when useful or requested. Audit
records and experimental artifacts stay local and untracked.

## Scope and authorization

Choose a bounded question, source paths, and tests that can answer it. For example:
does an exact output check prevent a simpler layout writer while preserving the
format another tool consumes? Start with the suspected cost and relevant behavior.
An exhaustive assertion census is optional. Split a broad audit into useful scopes.

Record the baseline commit, in-scope uncommitted changes, and dependency versions
that affect the result. Check related issues and pull requests when overlap matters.
Refresh affected evidence when the scope or baseline changes. Missing access is a
limitation, not evidence that no related work exists.

An audit request authorizes investigation and a report. If fixes are also authorized,
implement and validate them without another approval checkpoint. Otherwise, stop at
the report. Publish only when authorized. Group related changes for review and follow
the repository's commit, disclosure, and PR rules.

## Establish the contract

Read the request, relevant specifications, documentation, declarations,
implementation, callers, and tests. Distinguish external requirements, maintainer
decisions, compatibility obligations, internal invariants, and incidental choices.
Cite the source of each consequential claim and resolve conflicting evidence.

Missing documentation does not mean missing intent. Tests, type constraints,
interfaces, and downstream uses can expose requirements omitted from prose. A plan
or implementation alone does not make every detail permanent. In _fiction_, inspect
layout and clocking invariants, gate-library assumptions, numerical tolerances,
binding defaults and exception translations, and formats consumed by other tools.
Include a publication when the implementation promises the method it describes;
a citation alone does not promise every detail of that method.

Treat these as questions, never automatic findings:

- Exact text, iteration order, or counts may protect a file format, deterministic
  output, a normal form, or a performance bound.
- Exact floating-point checks may protect a lossless binding round trip. Use a
  justified tolerance for approximate computations; do not weaken exact contracts.
- Internal tests may isolate a safety invariant or an expensive regression.
- Mock call counts may enforce resource ownership, batching, or an external cost.
- Parameter matrices may cover distinct geometries, numerical cases, or backends.
- A defensive branch may protect external input even when in-tree callers cannot
  construct it. Search public and downstream boundaries before calling it dead.

AI assistance, co-introduction of code and tests, and a test edit accompanying an
implementation change do not establish a defect. History helps recover intent and
regressions; `git log -S` cannot tell whether a test has ever failed.

## Investigate candidates

For each candidate, state the proposed change, the behavior that must survive, and
the concrete benefit. Search callers and consumers before claiming that production
code exists only for a test. An exported API can have users outside this repository.

Use the smallest check that settles the question. Source reasoning can establish a
duplicate check or ownership; behavioral and performance claims need a reproducer
or measurement. Preserve regression coverage, resource safety, input validation,
numerical accuracy, and compatibility. Narrow an assertion only when its replacement
still detects the intended defect. For deletion, identify the surviving oracle and
justify the relevant input classes, paths, and failure modes.

Run experiments in your own worktree under `.ai/worktrees/`. Inspect its state and
preserve pre-existing changes. Record the baseline result, exact experimental edit
or self-contained reproducer, command, exit status, and relevant outcome. Serialize
experiments that share a checkout or build directory. Restore and recheck the
pre-experiment state after each probe, including after a failed build or test run.

Use the existing test entry points in `AGENTS.md`. Configure one suitable preset
from `CMakePresets.json` per worktree. Rebuild affected targets before testing a
native mutation, including a mutation exercised through Python bindings. Confirm
that Python imports the rebuilt extension and that the selected tests actually ran.
Check that the binaries reflect the current source; preserved timestamps can leave
stale binaries after restoration.
Use scoped CTest filters or pytest paths; an empty selection proves nothing.
Coverage instrumentation is optional; select the `coverage` preset at setup only
when the question needs it. Remove disposable worktrees and their builds when done.

For performance claims, prefer existing tools. Keep ad hoc harnesses, measurements,
and plots in `.ai/`. Record the workload, baseline, environment, measurements, and
limits, including neutral results and regressions. Untested optimizations remain
candidates. Preserve published experiment results under `experiments/`.

### What an experiment proves

- A behavior-preserving variation that breaks an assertion suggests a representation
  constraint. First establish that the variation preserves all relevant contracts,
  including downstream requirements.
- Another test catching one injected defect proves overlap for that defect, not
  redundancy across all inputs and failure modes.
- A surviving mutation may be equivalent, unreachable, poorly exercised, or evidence
  of a coverage gap. It never proves that behavior is unwanted or code removable.
  A missed observable defect calls for stronger coverage.
- Equal line coverage does not establish equal assertion strength. A finite mutation
  set supports conclusions only about that set.
- Build failures, collection errors, crashes before the target assertion, empty test
  selections, and broken result parsers are inconclusive probes.

Use direct test exit status and diagnostics. Do not infer verdicts from failure
counts or introduce an automatic probe wrapper. Coverage and mutation tools are
optional aids, not mandatory stages or verdict generators.

## Report findings people can act on

Put the result first. Rank confirmed findings by impact. Each finding needs:

1. **Change and benefit:** what to change, where, and why it matters.
2. **Contract and evidence:** the retained requirement, its source, and what the
   investigation established.
3. **Risk and limits:** downstream impact, untested cases, and any maintainer decision
   needed before implementation.

Keep confidence separate from impact. List unresolved candidates separately with
the missing evidence and next useful check. Reject candidates contradicted by
evidence and exclude them from the actionable count. No actionable findings is a
valid result. Avoid catalogs of passing assertions and hypothetical improvements.

For a test change, name any production simplification it enables and the consumer
checks needed. Test clarity alone can be a useful result; do not force a production
deletion. Report other correctness, simplification, and performance findings
separately. Do not credit them to an assertion that never blocked the change.

Challenge each finding with its strongest counterexample before handoff. Independent
review can help with disputed or high-risk findings; no fixed agent roster, model,
or multi-agent workflow is required. Apply only authorized fixes and follow
`AGENTS.md` for separate cleanup commits and maintainer approval of redesigns.

## Maintain a local record

Keep the baseline, durable reasoning, minimal reproducible evidence, and latest
disposition: proposed, accepted, applied, deferred, rejected, or superseded.
Acceptance is not implementation. Link the implementing change when known. Label
historical results and unverified current status. Recheck affected findings after
code or contracts change instead of rerunning an entire audit by default.

Remove transcripts, repeated status snapshots, and superseded recommendations.
Preserve a rejected finding's reason when it prevents a repeated mistake. Use
repository-relative paths and stable symbols; line numbers need a baseline.

```markdown
# Contract audit: <scope>

Status: <disposition>. Baseline: <commit and in-scope edits>. Date: <date>.
Source and tests: <repository-relative paths>.

## Result

<Ranked confirmed findings, or no actionable findings.>

## Findings

### <Concrete change>

<Contract, evidence, command and result, benefit, risk, confidence, disposition.>

## Unresolved questions

<Material gaps and the next check or decision needed.>

## Validation

<Reproducers, results, restoration checks, and limitations.>
```

## Example request

> Audit `test/python/sidb/io/test_print_sidb_layout.py` against the SiDB text
> printer's documented contract, C++ tests, bindings, and callers. Follow
> `.agent/AUDITS.md`. Investigate whether output assertions constrain incidental
> formatting or miss promised behavior. Report findings without applying fixes;
> save reproducible evidence in `.ai/audits/sidb-text-printer.md`.

## Source

Adapted from [MQT Core's contract audit guidance at revision `575acd1`](https://github.com/munich-quantum-toolkit/core/blob/575acd1b9ca52e4608ccf4833e7d60640913cef4/.agent/AUDITS.md).
The framework was introduced in [#2124](https://github.com/munich-quantum-toolkit/core/pull/2124).
The upstream method uses bounded evidence and direct test diagnostics; _fiction_
keeps audit records and experimental artifacts under ignored `.ai/`.

<!--
Upstream license notice for the adapted material:

MIT License

Copyright (c) 2023 - 2026 Chair for Design Automation, TUM
Copyright (c) 2025 - 2026 Munich Quantum Software Company GmbH

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
-->
