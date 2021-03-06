/* -*- C++ -*-
 *
 * This file is a part of LEMON, a generic C++ optimization library
 *
 * Copyright (C) 2003-2008
 * Egervary Jeno Kombinatorikus Optimalizalasi Kutatocsoport
 * (Egervary Research Group on Combinatorial Optimization, EGRES).
 *
 * Permission to use, modify and distribute this software is granted
 * provided that this copyright notice appears in all copies. For
 * precise terms see the accompanying LICENSE file.
 *
 * This software is provided "AS IS" with no warranty of any kind,
 * express or implied, and with no claim as to its suitability for any
 * purpose.
 *
 */

#ifndef LEMON_NAUTY_READER_H
#define LEMON_NAUTY_READER_H

#include <vector>
#include <iostream>
#include <string>


/// \ingroup io_group
///
/// @defgroup nauty_group NAUTY format
///
/// \brief Read \e Nauty format
///
/// Tool to read graphs from \e Nauty format data

/// \ingroup nauty_group
/// \file
/// \brief Nauty file reader.
namespace lemon {

  /// \ingroup nauty_group
  ///
  /// \brief Nauty file reader
  ///
  /// The \e geng program is in the \e gtools suite of the nauty
  /// package. This tool can generate all non-isomorphic undirected
  /// graphs with given node number from several classes (for example,
  /// general, connected, biconnected, triangle-free, 4-cycle-free,
  /// bipartite and graphs with given edge number and degree
  /// constraints). This function reads a \e nauty \e graph6 \e format
  /// line from the given stream and builds it in the given graph.
  ///
  /// The site of nauty package: http://cs.anu.edu.au/~bdm/nauty/
  ///
  /// For example, the number of all non-isomorphic connected graphs
  /// can be computed with next code:
  ///\code
  /// int num = 0;
  /// SmartUGraph ugraph;
  /// while(readNauty(std::cin, ugraph)) {
  ///   PlanarityChecking<SmartUGraph> pc(ugraph);
  ///   if (pc.run()) ++num;
  /// }
  /// std::cout << "Number of planar graphs: " << num << std::endl;
  ///\endcode
  ///
  /// The nauty files are quite huge, therefore instead of the direct
  /// file generation the pipelining is recommended. The execution of
  /// this program:
  ///\code 
  /// ./geng -c 10 | ./num_of_pg 
  ///\endcode
  template <typename UGraph>
  std::istream& readNauty(std::istream& is, UGraph& ugraph) {
    ugraph.clear();

    std::string line;
    if (getline(is, line)) {
      int index = 0;
    
      int n;

      if (line[index] == '>') {
	index += 10; 
      }

      char c = line[index++]; c -= 63;
      if (c != 63) {
	n = int(c);
      } else {
	c = line[index++]; c -= 63;
	n = (int(c) << 12);
	c = line[index++]; c -= 63;
	n |= (int(c) << 6);
	c = line[index++]; c -= 63;
	n |= int(c);      
      }

      std::vector<typename UGraph::Node> nodes;
      for (int i = 0; i < n; ++i) {
	nodes.push_back(ugraph.addNode());
      }

      int bit = -1;
      for (int j = 0; j < n; ++j) {
	for (int i = 0; i < j; ++i) {
	  if (bit == -1) {
	    c = line[index++]; c -= 63;
	    bit = 5;
	  }
	  bool b = (c & (1 << (bit--))) != 0;

	  ugraph.addEdge(nodes[i], nodes[j]);
	}
      }
    }
    return is;
  }
}

#endif
