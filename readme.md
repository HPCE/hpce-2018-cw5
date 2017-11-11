HPCE 2017 CW5
=============

- Issued: Fri 10th Nov
- Due: Fri 24th Nov, 22:00

Erratta
-------

- 2017/11/11 : Update to Gaussian Blur [due to typo](https://github.com/HPCE/hpce-2017-cw5/issues/2). This
  is a breaking change. Thanks to @pufik1337 for noticing it.

Specification
-------------

You have been given the included code with the
goal of making things faster. For our purposes,
faster means the wall-clock execution time of
`puzzler::Puzzle::Execute`, across a broad spectrum
of scale factors. There is also an emphasis on
good scaling - how large a scale parameter can
be executed?

The target platform is an AWS GPU (g2.2xlarge) instance. _I would
prefer to use a bigger instance, but this is the most economical,
and I don't want people running out of money here. I will probably
do some short runs on the submitted versions with a bigger instance
(e.g. a `p3.16xlarge`, at $27 an hour...), just out of interest,
but they will not be assessed_.

The target AMI will be the public `HPCE-2017-GPU-Image` AMI.
The AMI has OpenCL GPU and software providers installed, alongside
TBB. You can determine the location of headers and libraries by
starting up the AMI.

People working in triples must provide a solution to all six puzzles.

People working in pairs must choose four. The chosen puzzles will be
indicated in the `documentation.md` file, and the choice must be made
by the pair. If a pair has not made a clear choice of four for the
final submision, then four will be picked at random.

Triples have the advantage of knowing they need to 
do all puzzles, so there is no paralysis of choice, but they
have to do everything. Pairs have the opportunity to attack
more than four and try to pick the best performing or "easiest",
but may then not be able to probe as deeply. To the best
of my estimation there is no inherent advantage one
way or the other - the only thing that matters is the
ability of the people in the team, and not the choice of 2 or 3.

Meta-specification
------------------

You've now got some experience in different methods
for acceleration, and a decent working knowledge
about how to transform code in reasonable reliable
ways. This coursework represents a fairly common
situation - you haven't got much time, either to analyse
the problem or to do very low-level optimisation, and the problem
is actually a large number of sub-problems. So the goal
here is to identify and capture as much of the low-hanging
performance fruit as possible while not breaking anything.
If time allows you can dig deeper, but you first want to be
focussing on easy initial wins, and looking for any
clear asumptotic improvements (if they exist).

The code-base I've given you is somewhat baroque,
and despite having some rather iffy OOP practises,
actually has things quite reasonably
isolated. You will probably encounter the problem
that sometimes the reference solution starts to take
a very long time at large scales, but the persistence
framework gives you a way of dealing with that.

Beyond that, there isn't a lot more guidance, either
in terms of what you should focus on, or how
_exactly_ it will be measured. Part of the assesment
is in seeing whether you can work out what can be
accelerated (using parallelisation, restructuring, and
optimisation), and also seeing if you can focus your
efforts on the write parts.

The allocation of marks I'm using is:

- Compilation/Execution: 10%

  - How much work do I have to do to get it to compile and run.
    Everyone should get all marks here.

- Performance: 70%

  - You are competing with each other here, so there is an element of
    judgement in terms of how much you think others are doing or are
    capable of.
  
  - Marks for any puzzle which gives incorrect outputs will be
    reduced in proportion to the number of correct outputs. If
    a puzzle gives correct output only for a proportion _p_ of
    the inputs tested, the performance mark will be scaled by _p^2_. 
    You *really* want to be functionally correct.

  - If a puzzle runs too long and is terminated, it does not count
    as being incorrect.

  - People working in threes must implement all six puzzles.

  - People working in twos should _choose_ four puzzles, via `documentation.md`.

  - The performance mark is split equally between the puzzles
    you chose. So each puzzle is worth 1/6 of the performance marks in a
    triple, or 1/4 for a pair.
   
- Documentation: 20%

  - This should be given by filling in `documentation.md`, using
    the format given there. Each section will be evaluated in terms
    of whether it is coherent, whether it can be understood, and
    if there is any deeper insight being shown.

  - This section is to encourage concise communication and a bit of
    reflection, and to reward those who do it well. Some people get
    fast results through luck, and can't explain why - others get slower
    results, even though their reasoning and insight was much better.


Deliverable format
------------------

- Anything in the `include` directory is not owned by you, and subject to change

  - Any changes will happen in an additive way (none are expected for this CW)

  - Bug-fixes to `include` stuff are still welcome.

- You own the files in the `provider` directory

  - You'll be replacing the implementation of `XXXXProvider::Execute` in `provider/xxxx.hpp`
    with something (hopefully) faster.

  - A good starting point is to replace the implementation of `XXXXProvider::Execute` with a copy
    of the body of `XXXXPuzzle::ReferenceExecute`, and check that it still does the same thing.

  - The reason for the indirection is to force people to have an unmodified reference version
    available at all times, as it tends to encourage testing.

- The public entry point to your code is via `puzzler::PuzzleRegistrar::UserRegisterPuzzles`,
    which must be compiled into the static library `lib/libpuzzler.a`.

    - Clients will not directly include your code, they will only `#include "puzzler/puzzles.h`,
      then access puzzles via the registrar. They will get access to the registrar implementation
      by linking against `lib/libpuzzler.a`.

    - **Note**: If you do something complicated in your building of libpuzzler, it should still be
      possible to build it by going into `lib` and calling `make all`.

    - The current working directory during execution will be the root of the repository. So
      it will be executed as if typing `bin/execute_puzzle`, and an opencl kernel could be
      loaded using the relative path `provider/something.kernel`.

- The programs in `src` have no special meaning or status, they are just example programs

The reason for all this strange indirection is that I want to give
maximum freedom for you to do strange things within your implementation
(example definitions of "strange" include CMake) while still having a clean
abstraction layer between your code and the client code.

Intermediate Testing
--------------------

In the second week I'll be occasionally pulling and running tests on all the repositories,
and pushing the results back. These tests do _not_ check for correctness, they only check
that the implementations build and run correctly (and are also for my own interest
in seeing how performance evolves over time) I will push the results into
the `dt10_runs` directory.

If you are interested in seeing comparitive performance results, you can opt in
by changing the line `count-us-in` from `no` to `yes` in `documentation.md`.
This will result in graphs with lines for others who also opted in. There is no
specific relationship between the median and any assesment metric. The scripts will
attempt to run all puzzles, even for those in pairs. 

I will pull from the "master" branch, as this reflects good working practise - if
there is a testing branch, then that is where the unstable code should
be. The master branch should ideally always be compilable and correct, and
branches only merged into master once they are stable.

Finally, to re-iterate: the auto tests I am doing do _no_ testing at all for correctness;
they don't even look at the output of the tests.

Submission
----------

The code in github forms your submission, though you must submit your
final hash via blackboard for time-keeping and non-repudiation purposes.
Pushes to github after the deadline will not be treated as submissions,
_unless_ the new hash is also submitted after the deadline.

