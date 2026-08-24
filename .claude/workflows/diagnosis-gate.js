export const meta = {
  name: 'diagnosis-gate',
  description: 'Falsify a root-cause hypothesis before writing any fix: blind independent hypotheses, a dedicated refuter, then the single cheapest decisive measurement',
  whenToUse: 'Before implementing ANY fix for an observed defect. Pass args: {symptom, my_hypothesis, proposed_fix, context, repo}',
  phases: [
    { title: 'Blind', detail: 'independent root causes from the symptom alone — not told the standing hypothesis' },
    { title: 'Refute', detail: 'one agent paid to destroy the standing hypothesis' },
    { title: 'Decide', detail: 'rank by likelihood x cheapness, name the ONE measurement to make next' },
  ],
}

// ---------------------------------------------------------------------------
// Why this exists.
//
// Six wrong root causes in two days on this codebase, all with the same shape:
// a mechanism was found that plausibly explained the symptom, and plausibility
// was treated as proof. Every time a tool was finally consulted -- trace log,
// debugger, disassembler, a grep -- it overturned the story immediately. The
// decisive measurement was always cheap and always available.
//
// So this gate does not ask "is the hypothesis good?" It asks "what observation
// would prove it WRONG, and has that observation been made?" Fixes do not get
// written until that question has an answer.
//
// Two specific traps it is built to catch, both of which happened:
//   - adjacency read as causality (the last log line before a crash is not the
//     fault site)
//   - a named risk shipped anyway (a reviewer predicted a global-latch failure
//     mode; it was implemented as a global and failed exactly as predicted)
// ---------------------------------------------------------------------------

const A = args || {}
const SYMPTOM   = A.symptom       || '(no symptom given)'
const HYPO      = A.my_hypothesis || '(none stated)'
const FIX       = A.proposed_fix  || '(none stated)'
const CONTEXT   = A.context       || ''
const REPO      = A.repo          || 'c:\\TheLab\\Development\\Generals-Modern'

const BASE = `
## REPO
${REPO}

## THE OBSERVED SYMPTOM (this is ground truth — everything else is theory)
${SYMPTOM}

## CONTEXT
${CONTEXT}

## HARD RULES
- **READ-ONLY.** Create, modify or delete nothing. No git state changes. Do not build or run.
- Cite file:line or a quoted log line for every claim. Verify line references before relying on
  them; this codebase's docs run ~20 lines out of date.
- Grade confidence honestly: "proven" means you can point at the thing that makes it certain.
  An honest "plausible" is worth more than a confident wrong answer, which has already cost
  several build cycles here.
- A mechanism that *could* produce the symptom is NOT a diagnosis. Several different mechanisms
  usually could. Your value is in naming what DISTINGUISHES them.
`

const HYPO_SCHEMA = {
  type: 'object',
  additionalProperties: false,
  required: ['candidates'],
  properties: {
    candidates: {
      type: 'array', maxItems: 6,
      items: {
        type: 'object', additionalProperties: false,
        required: ['mechanism', 'confidence', 'evidence', 'falsifier', 'falsifier_cost'],
        properties: {
          mechanism: { type: 'string', description: 'how this would produce the exact symptom' },
          confidence: { type: 'string', enum: ['proven', 'strong', 'plausible', 'speculative'] },
          evidence: { type: 'array', items: { type: 'string' } },
          falsifier: { type: 'string', description: 'the single observation that would prove this WRONG' },
          falsifier_cost: { type: 'string', enum: ['grep', 'read-log', 'debugger', 'instrument-and-run', 'build-and-eyeball'] },
          would_also_explain: { type: 'array', items: { type: 'string' }, description: 'other known symptoms this accounts for' },
        },
      },
    },
    framing_challenge: { type: 'string', description: 'is the symptom description itself smuggling in an assumption? e.g. "X does not render" presupposes a render bug rather than X not existing' },
  },
}

const REFUTE_SCHEMA = {
  type: 'object',
  additionalProperties: false,
  required: ['verdict', 'attacks'],
  properties: {
    verdict: { type: 'string', enum: ['refuted', 'unsupported', 'survives-weakened', 'survives'] },
    attacks: {
      type: 'array',
      items: {
        type: 'object', additionalProperties: false,
        required: ['claim', 'problem'],
        properties: {
          claim: { type: 'string' },
          problem: { type: 'string' },
          counter_evidence: { type: 'string' },
        },
      },
    },
    fix_side_effects: { type: 'array', items: { type: 'string' }, description: 'what the proposed fix would break or mask, especially globals/latches that are not per-draw' },
    predicted_failure_mode: { type: 'string', description: 'if implemented and wrong, exactly how will it fail? this is the thing to test BEFORE shipping' },
  },
}

// ---------------------------------------------------------------------------
// Phase 1 — blind. Deliberately NOT told the standing hypothesis, so they
// cannot anchor on it. Three different investigative lenses.
// ---------------------------------------------------------------------------
phase('Blind')

const LENSES = [
  { key: 'data', lens: `**Lens: is the data there at all?** Before assuming a processing bug, check whether the thing being processed exists, was loaded, was parsed, and was registered. Look for swallowed failures — catch(...) handlers, early returns, "skipping" log lines, half-populated tables, load-ORDER problems where a lookup runs before its source is registered. A symptom phrased as "X is wrong" very often turns out to be "X was never created".` },
  { key: 'runtime', lens: `**Lens: what does the running program actually report?** Ignore what the code looks like it should do. Read the live logs and artifacts in the repo and say what is PROVEN to have happened versus assumed. Distinguish rigorously between correlation and causation — in particular, the last log line before a failure is NOT the failure site, and a counter that stopped may have stopped for an unrelated reason. Name what the logs do NOT cover.` },
  { key: 'mechanism', lens: `**Lens: trace the exact mechanism in code.** Follow the real control and data flow from cause to the specific observed symptom, and be precise about state lifetime: is the value per-draw, per-frame, or a global latch that some unrelated caller set earlier? Wrong-lifetime state is a recurring bug class in this codebase. Check whether an inspected value is even read by anything.` },
]

const blind = await parallel(LENSES.map(function (l) {
  return function () {
    return agent(
      `You are a **diagnostic specialist**. You are given ONLY a symptom. Produce the candidate root
causes, each with the cheapest observation that would prove it WRONG.

You are deliberately NOT told what anyone currently believes the cause is, so that you cannot
anchor on it. Reason from the symptom and the code.

${BASE}

## YOUR LENS
${l.lens}

Rank candidates by likelihood. For each, the \`falsifier\` field matters more than the mechanism:
name the concrete observation that would kill it, and how expensive that observation is. Cheap
decisive tests are the whole point.

Fill \`framing_challenge\` if the symptom as stated presupposes its own answer.
Return the structured object only.`,
      { label: `blind:${l.key}`, phase: 'Blind', schema: HYPO_SCHEMA, effort: 'high' }
    )
  }
}))

const cands = blind.filter(Boolean)
log(`Blind hypotheses from ${cands.length}/${LENSES.length} lenses`)

// ---------------------------------------------------------------------------
// Phase 2 — refute the standing hypothesis specifically.
// ---------------------------------------------------------------------------
phase('Refute')

const refutation = await agent(
  `You are a **skeptic** whose only job is to DESTROY the hypothesis below. You are not evaluating
it fairly; you are trying to break it. If it survives a genuine attempt to refute it, that is
worth something. If it does not, you have saved a build cycle.

${BASE}

## THE STANDING HYPOTHESIS (attack this)
${HYPO}

## THE PROPOSED FIX (attack this too)
${FIX}

## HOW TO ATTACK
1. Check every factual claim it rests on against the actual code and logs. Misread line numbers
   and stale references are common here.
2. Ask whether the hypothesis explains the symptom SPECIFICALLY, or merely permits it. If the
   symptom has features the hypothesis does not account for, say which.
3. Ask what ELSE would produce this exact symptom, and whether anything rules those out.
4. Attack the fix separately from the diagnosis. A correct diagnosis can still have a broken fix.
   Look hard for state-lifetime errors: does the fix put per-draw information in a global, or a
   frame value in a per-draw slot? Would it be a last-writer-wins latch?
5. \`predicted_failure_mode\`: if this fix ships and is wrong, describe exactly how it will fail.
   This is the single most valuable field — it is the test to run BEFORE shipping.

Return the structured object only.`,
  { label: 'refute', phase: 'Refute', schema: REFUTE_SCHEMA, effort: 'max' }
)

// ---------------------------------------------------------------------------
// Phase 3 — decide. Needs everything at once, so a barrier is correct here.
// ---------------------------------------------------------------------------
phase('Decide')

const DECIDE_SCHEMA = {
  type: 'object',
  additionalProperties: false,
  required: ['verdict', 'next_measurement', 'reasoning'],
  properties: {
    verdict: { type: 'string', enum: ['implement-now', 'measure-first', 'hypothesis-refuted', 'wrong-framing'] },
    next_measurement: { type: 'string', description: 'the ONE cheapest decisive observation to make next, concretely: the exact command, grep, log line or debugger step' },
    measurement_cost: { type: 'string', enum: ['grep', 'read-log', 'debugger', 'instrument-and-run', 'build-and-eyeball'] },
    discriminates: { type: 'string', description: 'which candidates this measurement separates, and what each outcome would mean' },
    ranked_candidates: {
      type: 'array', maxItems: 6,
      items: {
        type: 'object', additionalProperties: false,
        required: ['mechanism', 'standing', 'why'],
        properties: {
          mechanism: { type: 'string' },
          standing: { type: 'string', enum: ['leading', 'live', 'unlikely', 'ruled-out'] },
          why: { type: 'string' },
        },
      },
    },
    if_implementing_anyway: { type: 'string', description: 'the specific pre-ship test for the predicted failure mode' },
    reasoning: { type: 'string' },
  },
}

const decision = await agent(
  `You are the **diagnostician of record**. Three specialists produced candidate root causes from
the symptom alone, and a skeptic attacked the standing hypothesis. Decide what happens next.

${BASE}

## STANDING HYPOTHESIS
${HYPO}

## PROPOSED FIX
${FIX}

## BLIND CANDIDATES (authors were not told the standing hypothesis)
${JSON.stringify(cands, null, 1)}

## REFUTATION
${JSON.stringify(refutation, null, 1)}

## YOUR JOB
Default to \`measure-first\`. Only return \`implement-now\` if a candidate is genuinely PROVEN — a
cited code path or log line makes it certain — and the refuter failed to land a hit. Plausibility
is not sufficient; that specific error has been made repeatedly on this codebase.

\`next_measurement\` is the most important output: the ONE cheapest observation that best
discriminates between the live candidates. Prefer grep and log-reading over building. Say exactly
what to run and what each possible outcome would mean.

If any specialist raised a \`framing_challenge\` that holds up, return \`wrong-framing\` and say what
the question should have been instead.

If the verdict is to implement, \`if_implementing_anyway\` must name the concrete pre-ship test for
the refuter's predicted failure mode.

Return the structured object only.`,
  { label: 'decide', phase: 'Decide', schema: DECIDE_SCHEMA, effort: 'max' }
)

return {
  verdict: decision && decision.verdict,
  next_measurement: decision && decision.next_measurement,
  decision: decision,
  refutation: refutation,
  blind_candidate_count: cands.reduce(function (n, c) { return n + ((c.candidates || []).length) }, 0),
}
