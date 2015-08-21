/*
 * SSSPGTest.cpp
 *
 *  Created on: 21.07.2014
 *      Author: ebergamini
 */

#include "SSSPGTest.h"
#include "../DynBFS.h"
#include "../BFS.h"
#include "../DirOptBFS.h"
#include "../DynDijkstra.h"
#include "../Dijkstra.h"
#include "../../io/METISGraphReader.h"
#include "../../auxiliary/Log.h"
#include "../../auxiliary/Timer.h"

#include <stack>


namespace NetworKit {

TEST_F(SSSPGTest, testDijkstra) {
/* Graph:
         ______
		/      \
	   0    3   6
		\  / \ /
		 2    5
		/  \ / \
	   1    4   7
*/
	int n = 8;
	Graph G(n, true);

	G.addEdge(0, 2);
	G.addEdge(1, 2);
	G.addEdge(2, 3);
	G.addEdge(2, 4);
	G.addEdge(3, 5);
	G.addEdge(4, 5);
	G.addEdge(5, 6);
	G.addEdge(5, 7);
	G.addEdge(0, 6);


	Dijkstra sssp(G, 5, true, true);
	sssp.run();
	std::stack<node> stack = sssp.getStack();
	while (!stack.empty()) {
		node t = stack.top();
		stack.pop();
		DEBUG(t);
	}
}

TEST_F(SSSPGTest, testShortestPaths) {
	METISGraphReader reader;
	Graph G = reader.read("input/PGPgiantcompo.graph");
	INFO("The graph has been read.");
	int source = 2;
	BFS bfs(G, source);
	bfs.run();
	bigfloat max = 0;
	node x;
	G.forNodes([&](node n){
		if(bfs.numberOfPaths(n) > max) {
			max = bfs.numberOfPaths(n);
			x = n;
		}
	});
	count dist = bfs.distance(x);
	std::set<std::vector<node>> paths = bfs.getPaths(x, true);
	count i = 0;
	for (auto path : paths) {
		INFO("Path number ", i);
		i ++;
		INFO(path);
		EXPECT_EQ(path[0], source);
		EXPECT_EQ(path[dist], x);
	}
	INFO("Maximum number of shortest paths: ", bfs.numberOfPaths(x));
	INFO("Distance: ", dist);
}

TEST_F(SSSPGTest, testGetAllShortestPaths) {
/* Graph:

	   0    3   6   9
		\  / \ / \ /
         2    5   8
		/  \ / \ / \
	   1    4   7   10
*/
	int n = 11;
	Graph G(n, false);
	G.addEdge(0, 2);
	G.addEdge(1, 2);
	G.addEdge(2, 3);
	G.addEdge(2, 4);
	G.addEdge(3, 5);
	G.addEdge(4, 5);
	G.addEdge(5, 6);
	G.addEdge(5, 7);
	G.addEdge(6, 8);
	G.addEdge(7, 8);
	G.addEdge(8, 9);
	G.addEdge(8, 10);
	Dijkstra sssp(G, 0, true, false);
	sssp.run();
	std::set<std::vector<node>> paths = sssp.getPaths(9, true);
	count i = 0;
	for (auto path : paths) {
		INFO("Path number ", i);
		i ++;
		for (node n : path) {
			INFO(n);
		}
	}
}

TEST_F(SSSPGTest, testDirectedBFS) {
/* Graph:
         ________
		/        \.
	   0     3.    6
		\. ./  \ ./
		  2     .5
		./  \. / \.
	   1     4    7
*/
	int n = 8;
	// G directed unweighted
	Graph G(n, false, true);

	G.addEdge(0, 6);
	G.addEdge(0, 2);
	G.addEdge(3, 2);
	G.addEdge(5, 3);
	G.addEdge(6, 5);
	G.addEdge(5, 7);
	G.addEdge(4, 5);
	G.addEdge(2, 4);
	G.addEdge(2, 1);


	BFS sssp(G, 0);
	sssp.run();
	EXPECT_EQ(sssp.distance(0), 0);
	EXPECT_EQ(sssp.distance(1), 2);
	EXPECT_EQ(sssp.distance(2), 1);
	EXPECT_EQ(sssp.distance(3), 3);
	EXPECT_EQ(sssp.distance(4), 2);
	EXPECT_EQ(sssp.distance(5), 2);
	EXPECT_EQ(sssp.distance(6), 1);
	EXPECT_EQ(sssp.distance(7), 3);
}

TEST_F(SSSPGTest, testDirectedDijkstra) {
/* Graph:
         ________
		/        \.
	   0     3.    6
		\. ./  \ ./
		  2     .5
		./  \. / \.
	   1     4    7
*/
	int n = 8;
	// G directed unweighted
	Graph G(n, false, true);

	G.addEdge(0, 6, 1);
	G.addEdge(0, 2, 1);
	G.addEdge(3, 2, 1);
	G.addEdge(5, 3, 1);
	G.addEdge(6, 5, 1);
	G.addEdge(5, 7, 1);
	G.addEdge(4, 5, 1);
	G.addEdge(2, 4, 1);
	G.addEdge(2, 1, 1);


	Dijkstra sssp(G, 0);
	sssp.run();
	EXPECT_EQ(sssp.distance(0), 0);
	EXPECT_EQ(sssp.distance(1), 2);
	EXPECT_EQ(sssp.distance(2), 1);
	EXPECT_EQ(sssp.distance(3), 3);
	EXPECT_EQ(sssp.distance(4), 2);
	EXPECT_EQ(sssp.distance(5), 2);
	EXPECT_EQ(sssp.distance(6), 1);
	EXPECT_EQ(sssp.distance(7), 3);
}

TEST_F(SSSPGTest, testDirOptBFS) {
/* Graph:
         ______
		/      \
	   0    3   6
		\  / \ /
		 2    5
		/  \ / \
	   1    4   7
*/
	int n = 8;
	Graph G(n);

	G.addEdge(0, 2);
	G.addEdge(1, 2);
	G.addEdge(2, 3);
	G.addEdge(2, 4);
	G.addEdge(3, 5);
	G.addEdge(4, 5);
	G.addEdge(5, 6);
	G.addEdge(5, 7);
	G.addEdge(0, 6);


	BFS bfs_ref(G, 5);
	bfs_ref.run();

	DirOptBFS bfs_diropt(G, 5);
	bfs_diropt.run();

	EXPECT_EQ(bfs_ref.getDistances(),bfs_diropt.getDistances());
}

TEST_F(SSSPGTest, benchDirOptBFS) {
	METISGraphReader reader;
	Graph G = reader.read("input/as-Skitter.metis.graph");
	//Graph G = reader.read("input/caidaRouterLevel.graph");
	Aux::Timer t;
	count startNode = 100000;

	auto minimal_bfs = [](const Graph& G, count source) {
		count z = G.upperNodeIdBound();
		std::vector<double> distances(z,std::numeric_limits<edgeweight>::max());
		std::vector<bool> visited(z,false);
		distances[source] = 0;
		visited[source] = true;
		std::queue<node> q;
		q.push(source);
		while(!q.empty()) {
			auto current = q.front();
			q.pop();
			G.forNeighborsOf(current,[&](node v) {
				if (!visited[v]) {
					visited[v] = true;
					q.push(v);
					distances[v] = distances[current] + 1;
				}
			});
		}
		return std::move(distances);
	};

	t.start();
	auto min_dist = minimal_bfs(G,startNode);
	t.stop();
	INFO("bfs_min took:\t",t.elapsedTag());

	BFS bfs_ref(G, startNode);
	t.start();
	bfs_ref.run();
	t.stop();
	INFO("bfs_ref took:\t",t.elapsedTag());

	DirOptBFS bfs_diropt(G, startNode);
	t.start();
	bfs_diropt.run();
	t.stop();
	INFO("bfs_diropt took:\t",t.elapsedTag());

	auto ref_dist = bfs_ref.getDistances();
	auto do_dist = bfs_diropt.getDistances();
	G.forNodes([&](node v){
		EXPECT_EQ(ref_dist[v],min_dist[v]) << "min_dist differ at node " << v;
		EXPECT_EQ(ref_dist[v],do_dist[v]) << "diropt_dist differ at node " << v;
	});
}

} /* namespace NetworKit */