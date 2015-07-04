function result = crudeMC(probability, n, varargin)
	adjMat = getGraph(varargin{:});
	persistent p
	if isempty(p)
		p = inputParser;
		p.KeepUnmatched = true;
		addRequired(p, 'probability', @(x) isreal(x) && x >= 0 && x <= 1);
		addRequired(p, 'n', @(x) validateattributes(x, {'numeric'}, {'scalar', 'integer', 'nonnan', 'nonnegative', 'nonzero'}));
	end
	parse(p, probability, n, varargin{:})
	result = crudeMCImpl(probability, n, adjMat);
end
