function result = countConnectedSubgraphsBySize(varargin)
	adjMat = getGraph(varargin{:});
	sizes = size(adjMat)
	if sizes(2) == 0
		resultStrings = countConnectedSubgraphsBySizeImpl(adjMat);
	elseif sizes(2) == 1
		resultStrings = countConnectedSubgraphsBySizeImpl(adjMat);
	elseif sizes(2) > 32
		resultStrings = countConnectedSubgraphsBySizeImpl(adjMat);
		error('Can only perform exhaustive enumeration of all subgraphs if the number of vertices is 32 or less');
	else
		resultStrings = exhaustiveSearchImpl(adjMat);
	end
	result = resultStrings;
end
