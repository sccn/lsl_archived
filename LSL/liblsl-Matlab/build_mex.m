% Build mex bindings
% For Octave on Linux, you need the package liboctave-dev installed
% You also need the liblsl64 binary in the bin folder and a configured
% C compiler (mex -setup)

libs = '-llsl64';
if ispc
	dllext = 'dll';
elseif isunix
	dllext = 'so';
	libs = ['-llsl64 -ldl'];
elseif ismac
	dllext = 'dylib';
end


if isempty(dir(['bin/liblsl64.', dllext]))
	error('liblsl64.so not found in bin/');
end

ext = ['.' mexext];

files = dir('mex/*.c');
cd('bin');
for i = 1:length(files)
    f = files(i);
	[~, base, ~] = fileparts(f.name);
	targetstats = dir([base, ext]);
	if isempty(targetstats) || f.datenum > targetstats.datenum
		mex('-I../../liblsl/include','-L.', libs, ['../mex/', f.name]);
	else
		disp([base, ext, ' up to date']);
	end
end
cd('..');
