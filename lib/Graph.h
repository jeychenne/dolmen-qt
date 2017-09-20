/*
 * Graph.h
 *
 * Copyright (C) 2010-2017 Julien Eychenne 
 *
 * This file is part of Dolmen.
 *
 * Dolmen is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License (GPL) as
 * published by the Free Software Foundation, either version 2 of the
 * License, or (at your option) any later version.
 *
 * Dolmen is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Dolmen. If not, see <http://www.gnu.org/licenses/>.
 *
 * Author:  Julien Eychenne
 * Purpose:
 * Created: 15/02/11
 */

#ifndef GRAPH_H
#define GRAPH_H
// We use lists rather than sets throughout because the order often matters
// and we don't want to lose it.

#include <QList>
#include <QPair>

class Vertex;
// for now
typedef QPair<Vertex*, Vertex*> Edge;


class Vertex
{
public:
	Vertex();

	QList<Edge> inEdges() const;
	QList<Edge> outEdges() const;

	QList<Vertex*> inNeighbours() const;
	QList<Vertex*> outNeighbours() const;

	void addInEdge(Edge e);
	void addOutEdge(Edge e);

private:
	QList<Edge> m_inEdges;
	QList<Edge> m_outEdges;
};



class Graph
{
public:
	Graph(bool directed);

	bool isDirected();

protected:
	bool m_directed;
	QList<Edge> m_edges;
};

// Tree structure
class GraphNode : public Graph
{
public:
	GraphNode(Vertex *root);
	Vertex * root() const;
	void addDaughter(Vertex *d);
	QList<Vertex*> daughters() const;

private:
	Vertex *m_root;
};


#endif // GRAPH_H
