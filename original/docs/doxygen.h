/** \file 

\brief Used to hold doxygen documentation.  Ignore this file.

*/

/** \mainpage VFML

Welcome to the VFML (Very Fast Machine Learning toolkit) for mining
high-speed data streams and very large data sets.  VFML is made up of
three main components.  The first is a collection of tools and APIs
that help a user develop new learning algorithms.  The second
component is a collection of implementations of important learning
algorithms.  The third component is a collection of scalable learning
algorithms that were developed by Pedro Domingos and Geoff Hulten
(with the help of several other people see \ref thanks).  VFML is
written in standard C (and a bit of Python), and provides a series of
tutorials and examples as well as extensive in-source documentation in
JavaDoc format.  VFML is being distributed under a modified <a
href="http://www.opensource.org/licenses/bsd-license.php">BSD
license</a>.

VFML provides code to help read and process training data, to gather
sufficient statistics from it, ADTs for several important machine
learning structures, and various helper code.  You can get an overview
of what is provided by visiting the \ref CoreAPI and \ref UtilAPI
sections of the documentation.

VFML contains a series of tools for working with data sets: cleaning
them, sampling them, splitting them into train/test sets.  It also has
tools to help you experiment with learning algorithms.  See the \ref
Tools documentation heading for more information.

VFML contains tools for learning decision trees, for learning the
structure belief nets (aka Bayesian networks), and for clustering.
Much of this code is easy to modify or extend (several other
researchers have benefited from the \ref bnlearn program, for
example), and much of it can scale to learning from very large data
sets or from data streams.  You can get an overview of all the
learners by checking out the \ref Learners section.

\section downloads Downloads

- See the <a href="topics/gettingstarted.htm">Getting Started</a>
page for links to the software.
- <a href="appendixes/ucidatasets.htm">UCI Datasets</a>
- <a href="appendixes/beliefnets.htm">Benchmark Bayesian Networks</a>

\section topics User Documentation

<p>These links take you to some tutorials and example code on parts of
the VFML system.

<ul>
  <li><a href="topics/gettingstarted.htm">Getting Started with VFML</a> including details about how to get the code, build, and install the tools.</li>
  <li>A description of how to <a href="topics/runningtools.html">run
the tools and learners</a> that come with VFML.
  <li>A tutorial on <a href="topics/comparing-learners.htm">comparing learning algorithms</a> using the tools in VFML.</li>
  <li>A brief description of how to use the learners in VFML to <a href="topics/miningdatastreams.htm">mine data streams</a>.</li>
</ul>

The following sections contain more detailed documentation about
VFML's tool and learning programs.

- The \ref Tools "Tools Section" describes all the data manipulation and generation tools contained in VFML.
- The \ref Learners "Learners Section" describes all of the learners that are included in VFML.



\section details Developer and Reference Documentation

<ul>
  <li>An overview of what you will need to do to <a href="topics/creating-learners.htm">create your own learner</a> as well as a description of an example learning program that can be a starting point for your own projects.</li> 
  <li>A tutorial and example program on using VFML's APIs to <a
href="topics/loadingdata.htm">load a data set</a> into a program.</li>
  <li>An example on how to <a href="examples/interface_with_c45.htm"> interface with C4.5</a> in your programs</li>
</ul>

The following sections contain links to the documentation for all of
the APIs that you might find useful.  You might
like to download the reference manual (which contains all this
information) in <a href="downloads/vfml-reference.pdf">pdf format</a>.

- The \ref CoreAPI "Core APIs" describes learning related APIs and ADTs.
- The \ref UtilAPI "Utility APIs" describes generic APIs that may help you.
- The \ref DecisionTree "Decision Tree Section" contains documentation for the parts of VFML that are relevant to working with decision trees.
- The \ref BeliefNet "Belief Net Section" contains documentation for the parts of VFML that are relevant to working with belief nets.
- The \ref Clustering "Clustering Setction" contains documentation for the parts of VFML that are relevant to clustering.


\section appendixes Appendixes

- <a href="appendixes/c45.htm">C4.5 Format</a>
- <a href="appendixes/bif.htm">Bayesian Interchange Format</a>

\section contact Contact Us

<p>If you have any comments, suggestions, or bug reports, please feel
free to send us email: <a
href="mailto:ghulten@cs.washington.edu">ghulten@cs.washington.edu</a>
and <a
href="mailto:pedrod@cs.washington.edu">pedrod@cs.washington.edu</a></p></p>.  You can also post messages to our <a href="https://sourceforge.net/projects/vfml/">sourceforge forums</a>.

\section license Terms Of Use

You are welcome to use the code under the terms of the modified BSD
license for research or commercial purposes, however please
acknowledge its use with a citation:

   Hulten, G. and Domingos, P.  "VFML -- A toolkit for mining high-speed
time-changing data streams" http://www.cs.washington.edu/dm/vfml/.  2003.

Here is a BiBTeX entry:

\verbatim
   @unpublished{VFML,
      author = "Geoff Hulten and Pedro Domingos",
      title = "{V}{F}{M}{L} -- A toolkit for mining high-speed time-changing data streams",
      url = "http://www.cs.washington.edu/dm/vfml/",
      year = 2003}
\endverbatim

If you like, please also drop us a line about what you do with VFML
and what results you obtain. We'd love to know, and it will help us in
directing the future developments of VFML.

<hr>

The official license information follows:

VFML - Very Fast Machine Learning toolkit
Copyright (C) 2003, Geoff Hulten and Pedro Domingos
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.

Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in the
documentation and/or other materials provided with the distribution.

Neither the name of the University of Washington nor the names of its
contributors may be used to endorse or promote products derived from
this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

<hr>

\thanks VFML was made possible by a gift from the Ford Motor Company.

See \ref thanks for a list of additional people that have contributed to VFML.

\wish The windows distribution needs to be brought up to date.

*/



/** \defgroup CoreAPI Core APIs

These sections should be used as reference for details on the
interfaces you can use in your programs. They contain detailed
information on all of the APIs that VFML offers.
*/

/** \defgroup UtilAPI Utility APIs

Some utility code that is used by VFML. You may find it useful or
not. If you plan on modifying the internals of VFML you will certainly
care about these. (There are a few places where some of these are exposed
in an API too.)
*/

/** \defgroup DecisionTree Decision Tree Section

All of these are related to the use and construction of decision trees.
*/

/** \defgroup BeliefNet Belief Net Section

All of these are related to the use and construction of belief nets.
*/

/** \defgroup Clustering Clustering Section

All of these are related to clustering.
*/

/** \defgroup Learners Learning Programs

Learning programs that are included with VFML.
*/

/** \defgroup Tools Other Tools

Some tools to help you work with data sets and learners.
*/


