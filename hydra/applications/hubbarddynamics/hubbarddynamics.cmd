Opt std::string outfile "" o outfile outfile "name of outfile"
Opt std::string latticefile "" l latticefile latticefile "name of latticefile"
Opt std::string couplingfile "" t couplingfile couplingfile "name of couplingfile (default: latticefile)"
Opt std::string corrfile "" c corrfile corrfile "name of file containing sites of correlator (optional: default empty, only 0-0 correlator is computed)"
Opt int nup -1 u nup nup "number of up electrons (optional, default half filling)"
Opt int ndown -1 d ndown ndown "number of down electrons (optional, default half filling)"
Opt std::string fermiontype "" f fermiontype fermiontype "type of fermion (one of cdagdn, cdn) (optinal: default all are computed)"
Opt std::string algorithm "" a algorithm algorithm "algorithm used (one of lanczos, bandlanczos) (default: lanczos)"
Opt double precision 1e-12 p precision precision "precision of ground state Lanczos procedure (optional, default 1e-12)"
Opt int iters 1000 i iters iters "maximum number of ground state Lanczos iterations performed (optional, default 1000)"
Opt double dynprecision 1e-12 q dynprecision dynprecision "precision of dynamical Lanczos procedure (optional, default 1e-12)"
Opt int dyniters 1000 j dyniters dyniters "maximum number of dynamical Lanczos iterations performed (optional, default 1000)"
Opt int verbosity 1 v verbosity verbosity "verbosity level, one of 0, 1 ,2 (optional, default 1)"
Opt double deflationtol 1e-8 x deflationtol deflationtol "tolerance for detecting deflation in Band Lanczos algortihm (optional, default 1e-8)"