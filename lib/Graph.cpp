#include "Graph.h"

Vertex::Vertex()
{
	m_inEdges  = QList<Edge>();
	m_outEdges = QList<Edge>();
}

void Vertex::addInEdge(Edge e)
{
	if (! m_inEdges.contains(e))
		m_inEdges << e;
}

void Vertex::addOutEdge(Edge e)
{
	if (! m_outEdges.contains(e))
		m_outEdges << e;
}

QList<Edge> Vertex::inEdges() const
{
	return m_inEdges;
}

QList<Edge> Vertex::outEdges() const
{
	return m_outEdges;
}

QList<Vertex*> Vertex::inNeighbours() const
{
	QList<Vertex*> neighbours = QList<Vertex*>();

	foreach(Edge e, m_inEdges)
		neighbours << e.first;

	return neighbours;
}

QList<Vertex*> Vertex::outNeighbours() const
{
	QList<Vertex*> neighbours = QList<Vertex*>();

	foreach(Edge e, m_outEdges)
		neighbours << e.second;

	return neighbours;
}


//////////////////////////////////////////////////////
//////////////////////////////////////////////////////


Graph::Graph(bool directed)
{
	m_directed = directed;
	m_edges = QList<Edge>();
}

bool Graph::isDirected()
{
	return m_directed;
}


//////////////////////////////////////////////////////
//////////////////////////////////////////////////////

// graphnode is used for cross-tier search. edges shouldn't be hard-coded into annotation vertices
// otherwise they'll be remembered across searches...
GraphNode::GraphNode(Vertex *root) : Graph(true)
{
	m_root = root;
}

Vertex * GraphNode::root() const
{
	return m_root;
}

void GraphNode::addDaughter(Vertex *d)
{
	m_edges << Edge(m_root, d);
}

QList<Vertex*> GraphNode::daughters() const
{
	QList<Vertex*> neighbours = QList<Vertex*>();

	foreach (Edge e, m_edges)
		neighbours << e.second;

	return neighbours;
}




