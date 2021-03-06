function result = conditionalMC(probability, n, seed, varargin)
	adjMat = getGraph(varargin{:});
	persistent p
	if isempty(p)
		p = inputParser;
		p.KeepUnmatched = true;
		addRequired(p, 'probability', @(x) isreal(x) && x >= 0 && x <= 1);
		addRequired(p, 'n', @(x) validateattributes(x, {'numeric'}, {'scalar', 'integer', 'nonnan', 'nonnegative', 'nonzero'}));
		addRequired(p, 'seed', @(x) validateattributes(x, {'numeric'}, {'scalar', 'integer', 'nonnan', 'nonnegative'}));
	end
	parse(p, probability, n, seed, varargin{:})
	result = conditionalMCImpl(probability, n, seed, adjMat);
end
