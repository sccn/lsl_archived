function varargout = hlp_microcache(dom, f, varargin)
% Cache results of functions for repeated calls with the same arguments.
% Results... = hlp_microcache(Domain, Function, Arguments...)
%
% This is a lightweight mechanism to memoize results of functions that are often repeatedly called
% with the same arguments. It is indended for small-scale situations -- i.e., the function is called
% only with a small variety of arguments, for example less than 100, and the arguments (as well as
% results) are not too big (e.g., no large matrices or data sets). If too many different calls are
% to be cached, hlp_microcache "forgets" the oldest ones. Its memory is also lost after MATLAB is
% closed or after typing "clear all". Different places of a program can independently memoize
% results of their functions, by calling hlp_microcache with their own unique 'domain' identifier.
% The overhead is comparable to that of MATLAB's num2str(), i.e., very low.
%
% In:
%   Domain    : arbitrary (MATLAB-conformant) string identifier of the cache 'domain'
%               may be used to keep separate matters separate from each other
%
%   Function  : function handle to compute a result from some arguments
%
%   Arguments... : arguments to pass to the function
%
% Out:
%   Results... : return values of the function for the given arguments
%
%
% Notes:
%   Only *referentially transparent* functions are allowed; if a function can give different outputs
%   for the same arguments, this can lead to subtle bugs; examples include functions that refer to
%   global state. This can be fixed by turning all dependencies of the function into arguments.
%
%   A special note applies to lambda functions in the function's arguments (even deeply nested).
%   Every instance of such a lambda function is usually considered unique by MATLAB because lambdas
%   implicitly reference all variables in the scope where they were created, so hlp_microcache will
%   re-invoke the function eagerly. To ameliorate this, top-level lambda terms in the arguments can
%   be treated differently if the flag 'lambda_equality' is set to 'proper' or 'string' for the domain
%   (see Advanced section).
%
%   Special care has to be taken if a function behaves differently depending on the types of
%   arguments, especially uint and int types (which behave distinctly differently than doubles in
%   calculations): If the input values are the same numbers as for a previously cached result,
%   hlp_microcache will return that result regardless of the data type! You can work aroun this
%   where necessary by also passing the type of the affected arguments as to your function (this
%   way, matching will be type-sensitive)
%
%   This function is safe for concurrent use via timers.
%
% Examples:
%   % if this line is executed for the first time, it is as slow as magic(2000)
%   m = hlp_microcache('test',@magic,2000);
%
%   % if it is executed a second time, it is three orders of magnitude faster than m=magic(2000)
%   m = hlp_microcache('test',@magic,2000);
%
%
% Advanced:
%  When called as hlp_microcache(Domain, 'option1', value1, 'option2', value2, ...), the options
%  override aspects of the caching policy for the respective domain. Possible options are:
%   'resort_freq' : re-sort cached argument sets by their usage frequency every resort_freq lookups (default: 10)
%                   note: the number is not per domain but per group of equally-sized argument sets.
%   'group_size'  : retain only the group_size most-recently used argument sets (default: 100)
%                   note: the number is not per domain but per group of equally-sized argument sets.
%   'max_key_size': maximum size of keys that are cached, in bytes (larger ones are not cached) (default: 100000000)
%   'max_result_size': maximum size of results that are cached, in bytes (larger ones are not cached) (default: 100000000)
%   'lambda_equality': how the equality of lambda functions (that are not nested into data structures) is determined
%                      - false: the default MATLAB behavior will be used (fastest); this will consider
%                               lambda's to be non-equal in far more cases than strictly necessary,
%                               because they automatically bind all variables in the same scope as
%                               where they were created (default)
%                      - 'proper': the 'proper' way to handle top-level lambda functions with bound
%                                  arguments: any non-referenced variables from the same scope of
%                                  the lambda function will be excluded from comparison (which works
%                                  except for corner cases where the lambda term refers to them
%                                  using expressions like eval())
%                      - 'string': in this case, only the string representation will be compared --
%                                  ignoring any directly or indirectly bound variables (which is
%                                  fast); this works for simple expressions, but *will* give
%                                  incorrect results in more complicated cases, including
%                                  expressions such as @self.method (which MATLAB implicitly turns
%                                  into a lambda term!).
%
% When called as hlp_microcache(Domain, 'clear'), the cached contents of the domain will be cleared,
% and when called as hlp_microcache('clear'), the contents of *all* domains are cleared. Instead of
% 'clear', 'reset' can be used, which in addition resets the configurations for the affected domains.
%
% Advanced Examples:
%   % for the caching domain 'test', set a different resorting frequency and a different group size
%   hlp_microcache('test', 'resort_freq',30, 'group_size',100);
%
%   % clear the caching domain 'test' (configurations remain)
%   hlp_microcache('test', 'clear');
%
%   % reset the caching domain 'test' (including configurations)
%   hlp_microcache('test', 'reset');
%
%   % clear the contents of all caching domains (configurations remain)
%   hlp_microcache('clear');
%
%   % reset the micro-cache (including configuration)
%   hlp_microcache('reset');
%
%                                   Christian Kothe, Swartz Center for Computational Neuroscience, UCSD
%                                   2010-06-18

% mc ("microcache"): struct
%   domain-id --> domainpool
% domainpool: struct
%   config --> configuration
%   size-id --> sizepool
% sizepool: struct
%   outs: cell array of {f's outputs}
%   inps: cell array of [{char(f)},varargin,{nargout}]
%   frqs: double array of usage frequencies
%   luse: double array of last use cputime
%   lcnt: lookup count for this pool; used to schedule resorting (by usage frequency)
% configuration: struct
%   resort_freq: re-sort a sizepool every this many lookups (into that pool)
%   group_size: maximum number of entries per size-pool
%   max_result_size: maximum size of results that are cached
%   max_key_size: maximum size of keys that are cached
%   lambda_equality: desired equality relation for lambda expressions
persistent mc;


% [varargout{1:nargout}] = f(varargin{:}); return; % UNCOMMENT TO BYPASS

try
    % is this a regular call?
    if isa(f,'function_handle')
        
        % get the current lookup time
        now = cputime;
        
        % compute the key structure
        cf = char(f);
        if cf(1) == '@'
            % ignore variables only accessible via eval() / evalin() in lambda terms
            tmp = functions(f);
            key_f = {cf,tmp.workspace{1}};
        else
            key_f = f;
        end
        
        try
            % optionally do some special processing of lambda terms in the function arguments
            args = varargin;
            leq = mc.(dom).config.lambda_equality;
            if leq
                for k = find(cellfun('isclass',argin,'function_handle'))
                    ck = char(args{k});
                    if ck(1) == '@'
                        if strcmp(leq,'proper')
                            % the 'proper' way for most purposes is to consider both the expression and
                            % the directly bound variables but ignore variables in the same scope
                            tmp = functions(args{k});
                            args{k} = {ck,tmp.workspace{1}};
                        else
                            % the fastest way to deal with them is to scrap any possible bound variables
                            % and just consider the raw expression
                            args{k} = ck;
                        end
                    end
                end
            end
        catch
            % the lambda lookup failed...
            mc.(dom).config.lambda_equality = false;
        end
        key = [args,{key_f,nargout}];
        
        % get the size id (sid) of the key (MATLAB keeps track of that for every object)
        keyinfo = whos('key');
        keysid = sprintf('s%.0f',keyinfo.bytes);
        
        try
            % retrieve the pool of size-equivalent objects
            sizepool = mc.(dom).(keysid);
            % search for the key in the pool (checking the most-frequently used keys first)
            for k=1:length(sizepool.inps)
                if isequalwithequalnans(key,sizepool.inps{k}) % (isequalwithequalnans() is a fast builtin)
                    % found the key, deliver outputs
                    varargout = sizepool.outs{k};
                    % update the db record...
                    sizepool.frqs(k) = sizepool.frqs(k)+1;
                    sizepool.luse(k) = now;
                    sizepool.lcnt = sizepool.lcnt+1;
                    % resort by lookup frequency every resort_freq lookups
                    if sizepool.lcnt > mc.(dom).config.resort_freq
                        [sizepool.frqs,inds] = sort(sizepool.frqs,'descend');
                        sizepool.inps = sizepool.inps(inds);
                        sizepool.outs = sizepool.outs(inds);
                        sizepool.luse = sizepool.luse(inds);
                        sizepool.lcnt = 0;
                    end
                    % write back
                    mc.(dom).(keysid) = sizepool;
                    return;
                end
            end
        catch
            % domain+keysid not yet in the cache: create appropriate structures (this is rare)
            sizepool = struct('inps',{{}}, 'outs',{{}}, 'frqs',{[]}, 'luse',{[]}, 'lcnt',{0});
        end
        
        if ~exist('varargout','var')
            % set up the default configuration for a domain, if it's not yet present
            if ~isfield(mc,dom) || ~isfield(mc.(dom),'config')
                mc.(dom).config = struct(); end
            if ~isfield(mc.(dom).config,'resort_freq')
                mc.(dom).config.resort_freq = 10; end
            if ~isfield(mc.(dom).config,'group_size')
                mc.(dom).config.group_size = 100; end
            if ~isfield(mc.(dom).config,'max_result_size')
                mc.(dom).config.max_result_size = 100000000; end
            if ~isfield(mc.(dom).config,'max_key_size')
                mc.(dom).config.max_key_size = 100000000; end
            % did not find the entry in the size pool: compute it
            [varargout{1:nargout}] = f(varargin{:});
            iteminfo = whos('varargout');
            if iteminfo.bytes <= mc.(dom).config.max_result_size && keyinfo.bytes <= mc.(dom).config.max_key_size
                % add to pool
                sizepool.luse(end+1) = now;
                sizepool.frqs(end+1) = 1;
                sizepool.inps{end+1} = key;
                sizepool.outs{end+1} = varargout;
                sizepool.lcnt = 0;
                % remove least-recently used entries if necessary
                while length(sizepool.inps) > mc.(dom).config.group_size
                    [x,idx] = min(sizepool.luse); %#ok<ASGLU>
                    sizepool.luse(idx) = [];
                    sizepool.frqs(idx) = [];
                    sizepool.inps(idx) = [];
                    sizepool.outs(idx) = [];
                end
                % write back
                mc.(dom).(keysid) = sizepool;
            end
        end
        
    else
        % a control invocation
        varargin = [{f} varargin];
        if length(varargin) > 1
            % f and what follows are name-value pairs that define the domain configuration
            for k=1:2:length(varargin)
                mc.(dom).config.(varargin{k}) = varargin{k+1}; end
        else
            % single argument: this is a command to be applied to the domain
            switch varargin{1}
                case 'clear'
                    % clear the entire domain
                    if isfield(mc,dom)
                        if isfield(mc.(dom),'config')
                            % retain the config if applicable
                            mc.(dom) = struct('config',{mc.(dom).config});
                        else
                            mc = rmfield(mc,dom);
                        end
                    end
                case 'reset'
                    % reset the domain, including config
                    mc = rmfield(mc,dom);
                otherwise
                    error('Unrecognized command.');
            end
        end
    end
catch e
    if ~exist('f','var')
        % global command (no f specified)
        if strcmp(dom,'clear') && isstruct(mc)
            % clear all domains...
            for d = fieldnames(mc)'
                hlp_microcache(d{1},'clear'); end
        elseif strcmp(dom,'reset')
            % reset all domains
            mc = [];
        end
    else
        % genuine error
        rethrow(e);
    end
end
