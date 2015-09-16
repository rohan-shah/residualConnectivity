# Problem description
The Residual Connectedness Reliability (RCR) problem is as follows. We are given a graph G = (V, E) known as the *base graph* which models some complex system. To every vertex v we assign a binary random variable known as the *state*. For notational simplicity we assume that these states are independent and identically distributed. If the random variable takes value 1 then the part of the system represented by v is functioning correctly. If it takes value 0 then that part of the system has failed. We refer to these states as being *up* and *down*, respectively. 

The system as a whole is considered to be functioning (or in the *up* state) if the set of *up* vertices induces a connected subgraph of G. The RCR problem is to compute the probability that the system as a whole is in the *up* state. This problem is #P complete, which means we need to turn to approximation methods such as Monte Carlo Methods.

This repository contains
* Code for Monte Carlo methods for the RCR problem
* Visualisation code to help apply the methods
* Code related to exact computation for small graphs, and 
* Utility code used to implement the previous three. 

# Contents

## Monte Carlo Methods
This repository contains implementations of the following methods under the monteCarloMethods/ folder:
* The crude Monte Carlo method (monteCarloMethods/crudeMC.cpp).
* Permutation Monte Carlo (monteCarloMethods/PMC.cpp). This computes RCR indirectly using the spectra. 
* Stochastic Enumeration (monteCarloMethods/stochasticEnumeration*.cpp). This computes RCR indirectly using the spectra. 
* The Recursive Variance Reduction technique (monteCarloMethods/recursiveVarianceReduction.cpp). For details see
```
Cancela H, Urquhart M (2002) Adapting RVR simulation techniques for residual connectedness
network reliability models. IEEE Transactions on Computers 51(4):439–443
```
* A conditional Monte Carlo method (monteCarloMethods/conditionalMC.cpp). For details see
```
Shah R, Hirsch C, Kroese DP, Schmidt V (2014) Rare event probability estimation for connectivity of large random graphs. In: Tolks A, Diallo SD, Ryzhov IO, Yilmaz L, Buckley S,
Miller JA (eds) Proceedings of the 2014 Winter Simulation Conference, Institute of Electrical and Electronics Engineers, Inc, Piscataway, New Jersey
```
* A basic splitting implementation (monteCarloMethods/splittingBasic.cpp).
* A splitting implementation that decomposes by biconnected components in the laste step (monteCarloMethods/usingBiconnectedSplitting.cpp).
* Sequential Monte Carlo methods using articulation vertices, (monteCarloMethods/articulationConditionangResampling.cpp and monteCarloMethods/articulationConditioningSplitting.cpp). 

## Exact computation
This repository contains code for:
* Determining the spectra exactly by brute-force, under folder exhaustiveSearch/
* Using the spectra to compute the RCR exactly, under folder exhaustiveProbability/. 
* Determining the expected number of vertices conditional on observing a connected graph, under folder expectedVertexCount/.
* Determining the minimum reliability using Mathematica and the exact spectra, under folder minimumReliability/. 
* Computing the spectra of regular grid graphs exactly using the transfer matrix method, under folder transferMatrix/. For further details see 
```
Klein DJ, Hite GE, Schmalz TG (1986) Transfer-matrix method for subgraph enumeration:
Applications to polypyrene fusenes. Journal of Computational Chemistry 7(4):443–456
```
* Folders gridCountSpecificSize and gridCountSpecificSize2 contain code that count the number of connected subgraphs of a square grid-graph, which have a spectific number of vertices. This code was used to check the transfer matrix method code. 
## Visualisation code

## Utility code
* Folder residualConnectivityCommon contains utility code used throughout.
* Folder transferMatrixMethodCommon contains utilitiy code used for the transfer matrix method computations.
* Folder RPackage contains an R package with bindings for *some* of the code listed, allowing it to be called from within R.
* Folder matlabPackage contains a matlab package. 

# Building the software

## General instructions

The software is mostly organised as a collection of static libraries and executables. Static libraries include the utility code (residualConnectivityCommon/, transferMatrixCommon/) and implementations of some of the methods (crudeMC/crudeMCLib/, stochasticEnumeration/stochasticEnumerationLib/, etc). These are then linked into command-line executables (E.g. crudeMC/crudeMC, and the R and matlab packages). Note that some of the Monte Carlo methods are only provided as command-line executables; they'll be converted across to library+executable when I get round to it.

You can build the software using CMake on Windows and Linux (Mac is untested, but should work). Required dependencies are:
* Cmake 3.*
* The Boost C++ libraries.
* The Eigen linear algebra libraries. 
* The GNU MPFR and MPIR libraries. 

Optional dependencies are:
* Mathematica for the code in getMinimumReliability/.
* Matlab for the code in matlabPackage.
* R for the code in RPackage/.
* Igraph and QT 5 for the visualisation code. 

Building the Matlab package is controlled by the variable BUILD_MATLAB_PACKAGE (default: ON), building the R package is controlled by the variable BUILD_R_PACKAGE (default: ON), and building the visualisation code is controlled by the variable BUILD_VISUALISATION (default: ON). The minimal cmake command I have been using is:

cmake .. -DCMAKE_BUILD_TYPE=Release -DBOOST_ROOT=/home/uqrshah/Software/boost_1_58_0 -DEIGEN_INCLUDE_DIR=/home/uqrshah/Software/Eigen3.2.5 -DBUILD_VISUALISATION=OFF -DRcpp_DIR=/home/uqrshah/Software/Rcpp/release -DBOOST_LIBRARYDIR=/home/uqrshah/Software/boost_1_58_0/stage/lib-gcc5.2/

## Building on Windows

The only compiler that's likely to work on Windows is Visual Studio. To compile the R Package you will need the custom version of Rcpp available at rohan-shah/Rcpp. 
