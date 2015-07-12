function result = exhaustiveSearch(varargin)
	adjMat = getGraph(varargin{:});
	sizes = size(adjMat)
	if sizes(2) == 0
		resultStrings = exhaustiveSearchImpl(adjMat);
	elseif sizes(2) == 1
		resultStrings = exhaustiveSearchImpl(adjMat);
	elseif sizes(2) > 32
		resultStrings = exhaustiveSearchImpl(adjMat);
		error('Can only perform exhaustive enumeration of all subgraphs if the number of vertices is 32 or less');
	else
		resultStrings = exhaustiveSearchImpl(adjMat);
	end
	result = resultStrings;
end
