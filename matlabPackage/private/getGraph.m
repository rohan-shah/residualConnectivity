function output = getGraph(varargin)
	persistent p
	if isempty(p)
		p = inputParser;
		p.KeepUnmatched = true;
		isSquare = @(x) all(size(x) == size(x'));
		addParameter(p, 'adjMat', NaN, @(x) isreal(x) && ismatrix(x) && isSquare(x) && isBinary(x));
		addParameter(p, 'edges', NaN, @(x) isreal(x) && ismatrix(x) && twoColumns(x));
	end
	parse(p, varargin{:});
	missingAdjMat = all(size(p.Results.adjMat) == 1) && isnan(p.Results.adjMat);
	missingEdges = all(size(p.Results.edges) == 1) && isnan(p.Results.edges);
	if ~(xor(missingEdges, missingAdjMat))
		error('Exactly one of adjMat and edges must be input');
	end
	if missingEdges 
		adjMat = uint8(p.Results.adjMat);
		if (adjMat ~= adjMat')
			error('Input adjacency matrix must be symmetric (and therefore represent an undirected graph)')
		end
	else
		adjMat = uint8(zeros(max(p.Results.edges(:))));
		sizes = size(p.Results.edges);
		%Edges
		indices = sub2ind(size(adjMat), p.Results.edges(:,1), p.Results.edges(:, 2));
		adjMat(indices) = 1;
		%And reverse edges so it's symmetric
		indices = sub2ind(size(adjMat), p.Results.edges(:,2), p.Results.edges(:, 1));
		adjMat(indices) = 1;
	end
	output = adjMat;
end
function output = isBinary(x)
	toBinary = (uint8(x) == 0) | (uint8(x) == 1);
	output = all(toBinary(:));
end
function output = twoColumns(x)
	sizes = size(x);
	output = sizes(2) == 2;
end
