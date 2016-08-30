function arg_init
% Initialize the argument system.
%
%                                Christian Kothe, Swartz Center for Computational Neuroscience, UCSD
%                                2012-08-07

% set up some microcache properties
hlp_microcache('arg','lambda_equality','proper');
hlp_microcache('spec','group_size',5);
hlp_microcache('findfunction','lambda_equality','fast','group_size',5);

global tracking;
tracking.arg_sys = struct();