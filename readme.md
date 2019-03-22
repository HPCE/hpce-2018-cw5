The deadline for this assignment is

	22:00 Mon 4 Mar 2019

Code infrastructure
===================

This repository contains a simple framework for describing and executing
small computational puzzles. Each puzzle is derived from `puzzler::Puzzle`,
which provides a number of methods for creating inputs, executing puzzles,
and checking outputs. Of particular interest to you is the `puzzler::Puzzle::Execute`
method, which is the method that you need to accelerate, and is the function
that is timed for performance purposes. Each input problem has an associated
"scale", which is a measure of computational complexity - in general, as the
scale number goes up, the problem will take longer.

There are four puzzles that you need to work on as a pair:

- `ising`
- `rank`
- `decompose`
- `integral`

Each puzzle represents the computational core of some common calulation,
though all have been simplified to just a few lines of code. It's not
too difficult to work out what the underlying application is, and you
may find it interesting to look at existing solutions in the literature.
However, no specialised knowledge of each puzzles application domain
is needed, and the expectation is that only the standard techniques
from the lectures and earlier courseworks are needed.

The repository has a number of different directories:

- The directories under `include` should not be modified.
  - They are owned by the framework, and will be over-written in assessment environments to ensure they have not been modified.
  - If there are any changes to these files in the spec then they will be done in an additive way wherever possible (no changes are expected).

- You own the files in the `provider` directory
  - You'll be replacing the implementation of `XXXXProvider::Execute` in `provider/xxxx.hpp` with something (hopefully) faster.
  - A good starting point is to replace the implementation of `XXXXProvider::Execute` with a copy of the body of `XXXXPuzzle::ReferenceExecute`, and check that it still does the same thing.
  - The reason for the indirection is to force people to have an unmodified reference version available at all times, as it tends to encourage testing.
  - You can have multiple live alternative engines for each puzzle compiled into the executable, allowing you to progressively refine
    solutions while keeping previous known-good versions around. Each version should be registered under a different engine name.

- The public entry point to your code is via `puzzler::PuzzleRegistrar::UserRegisterPuzzles`, which must be compiled into the static library `lib/libpuzzler.a`.
  - Clients will not directly include your code, they will only `#include "puzzler/puzzles.h`, then access puzzles via the registrar. They will get access to the registrar implementation by linking against `lib/libpuzzler.a`.
  - Note: If you do something complicated in your building of libpuzzler, it should still be possible to build it by going into `provider` and calling `make all`.
  - The current working directory during execution will be the root of the repository. So it will be executed as if by typing `bin/execute_puzzle`, and an opencl kernel should be loaded using the relative path `provider/my_kernel.cl`.
  - Programs which link against and use `lib/libpuzzler.a` will also link in TBB and OpenCL.
  - You can register a given implementation with the registrar under a chosen engine name. This allows you to have multiple versions
    of a puzzle solution available, and allows you to expose specific solutions for different deliverable components. 

- The programs in `src` have no special meaning or status, they are just example programs

The reason for all this strange indirection is that I want to give maximum freedom for you to do strange things within your implementation (example definitions of "strange" include CMake) while still having a clean abstraction layer between your code and the client code.

Deliverables
============

The deliverable sections are:

- TBB Competence (20%)

  - For _all_ puzzles, you must deliver a version which correctly uses TBB
    to solve the problem. The majority of the compute must be performed
    under TBB, and it should use multiple cores. However, performance is
    not considered for this part, only correctness.
 
  - The engines demonstrating competence should be exposed via the name `${puzzle}.tbb`,
    where `<puzzle>` is the puzzle name. For example `ising.tbb` or `rank.tbb`.

- OpenCL Competence (20%)

  - For _two_ puzzles you must deliver a version which correctly uses
    OpenCL to solve the problem. The majority of the compute must be
    performed in kernels, and it should use multiple work-items in
    parallel. Again, performance is not considered here, only correctness.

  - The puzzles demonstrating OpenCL competence should be exposed via the name
    `${puzzle}.opencl`.

  - If there are more than two engines listed of the form `${puzzle}.opencl` then
    two will be selected in a deterministic way.

- Performance optimisation (40%)

  - Each puzzle has two target performance metrics (see below), defined for a
    competent and expert person, which were based on a person at the given level
    spending three hours per puzzle. For each puzzle you should provide an
    optimised version, and the improvement relative to the reference will be mapped
    to a mark:

    - [reference,competent) : 0..40%
    - [competent,expert*0.9)   : 40..100%

    Matching (or exceeding) expert*0.9 is equivalent to getting full marks, though you are
    not time constrained.

  - The puzzles to be assessed for performance should have a name of the form `${puzzle}.opt`.

  - The target environment is a `g3.4xlarge` instance, running the `HPCE-2018` AMI.

  - There is no requirement on how the optimised engines are implemented. You could use
    OpenCL, TBB, a combination of OpenCL and TBB, or even straight sequential code.

  - The engines selected here might be aliases for engines from the competence sections, or
    they could be completely different.

- Results presentation (20%)

  - Produce an A4 pdf called `question/results.pdf` which uses empirical data to explore and answer
    a question that arose while working with the puzzles. The pdf should contain
    no more than 300 words of text, and should use empirical data such
    as graphs and/or tables to support the conclusion.

    - **Quality of question**
      - Is it clear what question is being asked?
      - Is the question non-trivial?
      - Is the question specific to a puzzle (or puzzles)?
      - Would a reader find the answer interesting?

    - **Appropriateness of method**
      - Is the method aligned with the question: can it give an answer?
      - Are metrics and independent/dependent variables clear?
      - Is the tool or measurement method appropriate for the accuracy/precision needed?

    - **Quality of data**
      - Is the data clearly presented?
      - Are any important properties of the experimental environment described?
      - Are there sufficient data points to allow conclusions to be drawn?

    - **Analysis and insight**
      - Is the answer or result of the experiment clearly stated (or equally is the _lack_ of a clear answer identified)?
      - Does the conclusion draw upon the data presented?
      - Does the conclusion show insight beyond simply reporting results?

  - The word limit is to avoid people spending too much time on writing long reports,
    and also to enforce concision and clarity of communication.
  
  - This section is deliberately open-ended, and is intended to assess your ability to
    formulate your own questions and design experiments, rather than just following
    instructions (this is a masters level course, after all).

  - It is worth thinking of this part as preparation for final year project evaluation
    sections, as your project evaluation should be addressing all these components too.


Performance metric
==================

Performance is defined in terms of the number of puzzles of increasing scale
executed within a fixed time budget. The time budget is fixed at $t_B=60$ seconds
for each puzzle, and the budget includes the entirety of `Puzzle::Execute`.

The sequence of scales is determined according to a schedule for each puzzle,
which maps an index $i>=1$ to a sequence of scales $s_1,s_2,s_3,...$. The
scale schedule is intended to match the inherent scaling properties of each puzzle,
and balances some exploration of smaller scales, while also
allowing faster implementations to push up to larger scales. The sequence
as given is defined ising a random term $U$ to make it non-deterministic;
so it is possible to work out what the sequence looks like, but not the precise numbers
(to avoid over-fitting). $U$ is a uniform random number in the range $[0,1)$.
For assessment purposes, the sequence of input scales
and actual inputs for a given puzzle will be identical across all submissions.

To create a continuous metric, the puzzle which is executing when the timer expires
will be added pro-rata based on "how far" it had got through. If we have a sequence
of scales $s_1, s_2, s_3, ...$ and an implementation which takes $f(s)$ seconds at
scale $s$, then the puzzles in the sequence will complete at $t_j = \sum_{i=1}^{j} f(i)$.
The sequence of complete times $t_1,t_2,t_3, ... t_{n},t_{n+1}$, will eventually
exceed the time budget $t_B$ at some point $n$, which occurs when $t_{n} <= t_B < t_{n+1}$.
We consider $n$ puzzles to be completed, and $(t_B-t_{n})/(t_{n+1}-t_{n})$ of the
final puzzle to have been completed, for a final metric of:
```
n+(t_B-t_{n})/(t_{n+1}-t_{n})
```
In order to manage execution time in assessment (e.g. in the case of an infinite loop),
if $1.5*t_B < t_{n+1}$ then we assume that $t_{n+1} = \infty$. Practically speaking
this means that if $t_B=60$ then the timeout for the final puzzle is 90 seconds.

| Puzzle      | Sequence             | Reference   | Competent  | Expert   |
|-------------|----------------------|-------------|------------|----------|
| rank        | `50+(i*30+U*20)^2`   | 43.3910     | 44.7968    | 60.0788  |
| decompose   | `20+(i*30+U*10)^1.2` | 15.7040     | 27.2232    | 38.1773  |
| ising       | `20+(i*20+U*5)`      | 18.5639     | 18.7988    | 35.9099  |
| integral    | `20+(i*20+U*5)`      | 23.2298     | 37.3449    | 170.683  |

The expert observed that they weren't at the limits of performance on
any puzzle, only of implementation time, so if you end up with higher
performance don't assume you've done something wrong.

Getting Started
===============

Drop into the root of the repo and do `make all`. This should produce a number of executables:

- `bin/run_puzzle engine_name scale [logLevel]` : General purpose driver, which does the following steps:

   1. Creates an instance of `engine_name`.
   2. Creates an input for the chosen puzzle type using the given `scale`.
   3. Executes the engine on the puzzle input and captures the output.
   4. Executes the reference implementation on the same puzzle input and captures the output.
   5. Verifies that the two solutions are the same.
  
- `bin/create_input puzzle_name scale [logLevel]` : Creates an input for the given puzzle and scale, and then
  writes it (as binary!) to stdout.

- `bin/execute_puzzle engine|"ref" [logLevel]` : Reads a problem from stdin, uses the chosen engine to
  solve it, then writes the solution to stdout. Selecting "ref" as the engine will use the reference
  solver.

- `bin/compare_puzzle_output inputFile refFile gotFile [logLevel]` : Takes in an input problem, a reference
  solution, and an alternative solution, and checks that the two solutions are the same. The exit code
  of the program is 0 iff the two solutions match. Not that for some puzzles the answers do not
  have to be bit-exact equal, so two different outputs may both be correct.

The different programs are intended to make it easy to write infrastructure for
testing performance and correctness. For example, if one wished to test an engine
called "ising.tbb" at a number of different scales, one could script something
along these lines:
```
#!/bin/bash
ENGINE="ising.tbb"
PUZZLE=${ENGINE%%.*}
SCALES="100 150 200"
WORKING=.tmp
mkdir ${WORKING}
for SCALE in $SCALES ; do
  bin/create_puzzle_input ${PUZZLE} ${SCALE} > ${WORKING}/${PUZZLE}.${SCALE}.input
  cat ${WORKING}/${PUZZLE}.${SCALE}.input | bin/execute_puzzle "ref" > ${WORKING}/${PUZZLE}.${SCALE}.ref
  cat ${WORKING}/${PUZZLE}.${SCALE}.input | bin/execute_puzzle ${ENGINE} > ${WORKING}/${PUZZLE}.${SCALE}.${ENGINE}.got
  bin/compare_puzzle_output ${WORKING}/${PUZZLE}.${SCALE}.input ${WORKING}/${PUZZLE}.${SCALE}.ref ${WORKING}/${PUZZLE}.${SCALE}.${ENGINE}.got
  if [[ $? -ne 0 ]] ; then
    >&2 echo "FAIL"
    exit 1
  fi
done
```

Submission
==========

The code in github forms your submission, though you must submit your final hash via blackboard for time-keeping and non-repudiation purposes. Pushes to github after the deadline will not be treated as submissions, unless the new hash is also submitted after the deadline. All members of each group must submit the same hash to show agreement.
