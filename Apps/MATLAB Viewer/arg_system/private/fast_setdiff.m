function A = fast_setdiff(A,B)
% A fast version of setdiff for cell arrays of strings.

try
    if isempty(A)
        A = {}; 
    else
        A(CStrAinBP(A,B)) = [];
    end
catch
    disp_once('Using the slower MATLAB fallback for setdiff(). Consider setting up a compiler to get much improved BCILAB performance (in some areas).');
    A = setdiff(A,B);
end

