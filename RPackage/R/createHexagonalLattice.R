createHexagonalLattice <- function(x, y)
{
	if(missing(x) || missing(y)) 			stop("Inputs x and y are required")
	if(length(x) != 1 || length(y) != 1) 	stop("Inputs x and y must be positive integers")
	if(!is.numeric(x) || ! is.numeric(y))	stop("Inputs x and y must be positive integers")
	if(x < 1 || y < 1) 						stop("Inputs x and y must be positive integers")
	if(abs(x - as.integer(x)) > 1e-3 || abs(y - as.integer(y)) > 1e-3) 	stop("Inputs x and y must be positive integers")
	x <- as.integer(x)
	y <- as.integer(y)
	edgesAndPositions <- .Call("createHexagonalLattice", x, y, PACKAGE="residualConnectivity")
	return(list(graph = igraph::graph(edgesAndPositions$edges, directed=FALSE), positions = edgesAndPositions$positions))
}