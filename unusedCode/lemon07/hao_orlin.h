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

#ifndef LEMON_HAO_ORLIN_H
#define LEMON_HAO_ORLIN_H

#include <vector>
#include <list>
#include <ext/hash_set>
#include <limits>

#include <lemon/maps.h>
#include <lemon/graph_utils.h>
#include <lemon/graph_adaptor.h>
#include <lemon/iterable_maps.h>

/// \file
/// \ingroup min_cut
/// \brief Implementation of the Hao-Orlin algorithm.
///
/// Implementation of the Hao-Orlin algorithm class for testing network 
/// reliability.

namespace lemon {

  /// \ingroup min_cut
  ///
  /// \brief %Hao-Orlin algorithm to find a minimum cut in directed graphs.
  ///
  /// Hao-Orlin calculates a minimum cut in a directed graph
  /// \f$D=(V,A)\f$. It takes a fixed node \f$ source \in V \f$ and
  /// consists of two phases: in the first phase it determines a
  /// minimum cut with \f$ source \f$ on the source-side (i.e. a set
  /// \f$ X\subsetneq V \f$ with \f$ source \in X \f$ and minimal
  /// out-degree) and in the second phase it determines a minimum cut
  /// with \f$ source \f$ on the sink-side (i.e. a set 
  /// \f$ X\subsetneq V \f$ with \f$ source \notin X \f$ and minimal
  /// out-degree). Obviously, the smaller of these two cuts will be a
  /// minimum cut of \f$ D \f$. The algorithm is a modified
  /// push-relabel preflow algorithm and our implementation calculates
  /// the minimum cut in \f$ O(n^2\sqrt{m}) \f$ time (we use the
  /// highest-label rule), or in \f$O(nm)\f$ for unit capacities. The
  /// purpose of such algorithm is testing network reliability. For an
  /// undirected graph you can run just the first phase of the
  /// algorithm or you can use the algorithm of Nagamochi and Ibaraki
  /// which solves the undirected problem in 
  /// \f$ O(nm + n^2 \log(n)) \f$ time: it is implemented in the
  /// NagamochiIbaraki algorithm class.
  ///
  /// \param _Graph is the graph type of the algorithm.
  /// \param _CapacityMap is an edge map of capacities which should
  /// be any numreric type. The default type is _Graph::EdgeMap<int>.
  /// \param _Tolerance is the handler of the inexact computation. The
  /// default type for this is Tolerance<typename CapacityMap::Value>.
  ///
  /// \author Attila Bernath and Balazs Dezso
#ifdef DOXYGEN
  template <typename _Graph, typename _CapacityMap, typename _Tolerance>
#else
  template <typename _Graph,
	    typename _CapacityMap = typename _Graph::template EdgeMap<int>,
            typename _Tolerance = Tolerance<typename _CapacityMap::Value> >
#endif
  class HaoOrlin {
  private:

    typedef _Graph Graph;
    typedef _CapacityMap CapacityMap;
    typedef _Tolerance Tolerance;

    typedef typename CapacityMap::Value Value;

    GRAPH_TYPEDEFS(typename Graph);
    
    const Graph& _graph;
    const CapacityMap* _capacity;

    typedef typename Graph::template EdgeMap<Value> FlowMap;
    FlowMap* _flow;

    Node _source;

    int _node_num;

    // Bucketing structure
    std::vector<Node> _first, _last;
    typename Graph::template NodeMap<Node>* _next;
    typename Graph::template NodeMap<Node>* _prev;    
    typename Graph::template NodeMap<bool>* _active;
    typename Graph::template NodeMap<int>* _bucket;
    
    std::vector<bool> _dormant;

    std::list<std::list<int> > _sets;
    std::list<int>::iterator _highest;
    
    typedef typename Graph::template NodeMap<Value> ExcessMap;
    ExcessMap* _excess;

    typedef typename Graph::template NodeMap<bool> SourceSetMap;
    SourceSetMap* _source_set;

    Value _min_cut;

    typedef typename Graph::template NodeMap<bool> MinCutMap;
    MinCutMap* _min_cut_map;

    Tolerance _tolerance;

  public: 

    /// \brief Constructor
    ///
    /// Constructor of the algorithm class. 
    HaoOrlin(const Graph& graph, const CapacityMap& capacity, 
             const Tolerance& tolerance = Tolerance()) :
      _graph(graph), _capacity(&capacity), _flow(0), _source(),
      _node_num(), _first(), _last(), _next(0), _prev(0), 
      _active(0), _bucket(0), _dormant(), _sets(), _highest(),
      _excess(0), _source_set(0), _min_cut(), _min_cut_map(0), 
      _tolerance(tolerance) {}

    ~HaoOrlin() {
      if (_min_cut_map) {
        delete _min_cut_map;
      } 
      if (_source_set) {
        delete _source_set;
      }
      if (_excess) {
        delete _excess;
      }
      if (_next) {
	delete _next;
      }
      if (_prev) {
	delete _prev;
      }
      if (_active) {
	delete _active;
      }
      if (_bucket) {
	delete _bucket;
      }
      if (_flow) {
        delete _flow;
      }
    }
    
  private:

    void activate(const Node& i) {
      _active->set(i, true);

      int bucket = (*_bucket)[i];

      if ((*_prev)[i] == INVALID || (*_active)[(*_prev)[i]]) return;	    
      //unlace
      _next->set((*_prev)[i], (*_next)[i]);
      if ((*_next)[i] != INVALID) {
	_prev->set((*_next)[i], (*_prev)[i]);
      } else {
	_last[bucket] = (*_prev)[i];
      }
      //lace
      _next->set(i, _first[bucket]);
      _prev->set(_first[bucket], i);
      _prev->set(i, INVALID);
      _first[bucket] = i;
    }

    void deactivate(const Node& i) {
      _active->set(i, false);
      int bucket = (*_bucket)[i];

      if ((*_next)[i] == INVALID || !(*_active)[(*_next)[i]]) return;
      
      //unlace
      _prev->set((*_next)[i], (*_prev)[i]);
      if ((*_prev)[i] != INVALID) {
	_next->set((*_prev)[i], (*_next)[i]);
      } else {
	_first[bucket] = (*_next)[i];
      }
      //lace
      _prev->set(i, _last[bucket]);
      _next->set(_last[bucket], i);
      _next->set(i, INVALID);
      _last[bucket] = i;
    }

    void addItem(const Node& i, int bucket) {
      (*_bucket)[i] = bucket;
      if (_last[bucket] != INVALID) {
	_prev->set(i, _last[bucket]);
	_next->set(_last[bucket], i);
	_next->set(i, INVALID);
	_last[bucket] = i;
      } else {
	_prev->set(i, INVALID);
	_first[bucket] = i;
	_next->set(i, INVALID);
	_last[bucket] = i;
      }
    }
    
    void findMinCutOut() {

      for (NodeIt n(_graph); n != INVALID; ++n) {
	_excess->set(n, 0);
      }

      for (EdgeIt e(_graph); e != INVALID; ++e) {
	_flow->set(e, 0);
      }

      int bucket_num = 1;
      
      {
	typename Graph::template NodeMap<bool> reached(_graph, false);
	
	reached.set(_source, true);

	bool first_set = true;

	for (NodeIt t(_graph); t != INVALID; ++t) {
	  if (reached[t]) continue;
	  _sets.push_front(std::list<int>());
	  _sets.front().push_front(bucket_num);
	  _dormant[bucket_num] = !first_set;

	  _bucket->set(t, bucket_num);
	  _first[bucket_num] = _last[bucket_num] = t;
	  _next->set(t, INVALID);
	  _prev->set(t, INVALID);

	  ++bucket_num;
	  
	  std::vector<Node> queue;
	  queue.push_back(t);
	  reached.set(t, true);
	  
	  while (!queue.empty()) {
	    _sets.front().push_front(bucket_num);
	    _dormant[bucket_num] = !first_set;
	    _first[bucket_num] = _last[bucket_num] = INVALID;
	    
	    std::vector<Node> nqueue;
	    for (int i = 0; i < int(queue.size()); ++i) {
	      Node n = queue[i];
	      for (InEdgeIt e(_graph, n); e != INVALID; ++e) {
		Node u = _graph.source(e);
		if (!reached[u] && _tolerance.positive((*_capacity)[e])) {
		  reached.set(u, true);
		  addItem(u, bucket_num);
		  nqueue.push_back(u);
		}
	      }
	    }
	    queue.swap(nqueue);
	    ++bucket_num;
	  }
	  _sets.front().pop_front();
	  --bucket_num;
	  first_set = false;
	}

	_bucket->set(_source, 0);
	_dormant[0] = true;
      }
      _source_set->set(_source, true);	  
	  
      Node target = _last[_sets.back().back()];
      {
	for (OutEdgeIt e(_graph, _source); e != INVALID; ++e) {
	  if (_tolerance.positive((*_capacity)[e])) {
	    Node u = _graph.target(e);
	    _flow->set(e, (*_capacity)[e]);
	    _excess->set(u, (*_excess)[u] + (*_capacity)[e]);
	    if (!(*_active)[u] && u != _source) {
	      activate(u);
	    }
	  }
	}
	if ((*_active)[target]) {
	  deactivate(target);
	}
	
	_highest = _sets.back().begin();
	while (_highest != _sets.back().end() && 
	       !(*_active)[_first[*_highest]]) {
	  ++_highest;
	}
      }


      while (true) {
	while (_highest != _sets.back().end()) {
	  Node n = _first[*_highest];
	  Value excess = (*_excess)[n];
	  int next_bucket = _node_num;

	  int under_bucket;
	  if (++std::list<int>::iterator(_highest) == _sets.back().end()) {
	    under_bucket = -1;
	  } else {
	    under_bucket = *(++std::list<int>::iterator(_highest));
	  }

	  for (OutEdgeIt e(_graph, n); e != INVALID; ++e) {
	    Node v = _graph.target(e);
	    if (_dormant[(*_bucket)[v]]) continue;
	    Value rem = (*_capacity)[e] - (*_flow)[e];
	    if (!_tolerance.positive(rem)) continue;
	    if ((*_bucket)[v] == under_bucket) {
	      if (!(*_active)[v] && v != target) {
		activate(v);
	      }
	      if (!_tolerance.less(rem, excess)) {
		_flow->set(e, (*_flow)[e] + excess);
		_excess->set(v, (*_excess)[v] + excess);
		excess = 0;
		goto no_more_push;
	      } else {
		excess -= rem;
		_excess->set(v, (*_excess)[v] + rem);
		_flow->set(e, (*_capacity)[e]);
	      }
	    } else if (next_bucket > (*_bucket)[v]) {
	      next_bucket = (*_bucket)[v];
	    }
	  }

	  for (InEdgeIt e(_graph, n); e != INVALID; ++e) {
	    Node v = _graph.source(e);
	    if (_dormant[(*_bucket)[v]]) continue;
	    Value rem = (*_flow)[e];
	    if (!_tolerance.positive(rem)) continue;
	    if ((*_bucket)[v] == under_bucket) {
	      if (!(*_active)[v] && v != target) {
		activate(v);
	      }
	      if (!_tolerance.less(rem, excess)) {
		_flow->set(e, (*_flow)[e] - excess);
		_excess->set(v, (*_excess)[v] + excess);
		excess = 0;
		goto no_more_push;
	      } else {
		excess -= rem;
		_excess->set(v, (*_excess)[v] + rem);
		_flow->set(e, 0);
	      }
	    } else if (next_bucket > (*_bucket)[v]) {
	      next_bucket = (*_bucket)[v];
	    }
	  }
	  
	no_more_push:
	  
	  _excess->set(n, excess);
	  
	  if (excess != 0) {
	    if ((*_next)[n] == INVALID) {
	      typename std::list<std::list<int> >::iterator new_set = 
		_sets.insert(--_sets.end(), std::list<int>());
	      new_set->splice(new_set->end(), _sets.back(), 
			      _sets.back().begin(), ++_highest);
	      for (std::list<int>::iterator it = new_set->begin();
		   it != new_set->end(); ++it) {
		_dormant[*it] = true;
	      }
	      while (_highest != _sets.back().end() && 
		     !(*_active)[_first[*_highest]]) {
		++_highest;
	      }
	    } else if (next_bucket == _node_num) {
	      _first[(*_bucket)[n]] = (*_next)[n];
	      _prev->set((*_next)[n], INVALID);
	      
	      std::list<std::list<int> >::iterator new_set = 
		_sets.insert(--_sets.end(), std::list<int>());
	      
	      new_set->push_front(bucket_num);
	      _bucket->set(n, bucket_num);
	      _first[bucket_num] = _last[bucket_num] = n;
	      _next->set(n, INVALID);
	      _prev->set(n, INVALID);
	      _dormant[bucket_num] = true;
	      ++bucket_num;

	      while (_highest != _sets.back().end() && 
		     !(*_active)[_first[*_highest]]) {
		++_highest;
	      }
	    } else {
	      _first[*_highest] = (*_next)[n];
	      _prev->set((*_next)[n], INVALID);
	      
	      while (next_bucket != *_highest) {
		--_highest;
	      }

	      if (_highest == _sets.back().begin()) {
		_sets.back().push_front(bucket_num);
		_dormant[bucket_num] = false;
		_first[bucket_num] = _last[bucket_num] = INVALID;
		++bucket_num;
	      }
	      --_highest;

	      _bucket->set(n, *_highest);
	      _next->set(n, _first[*_highest]);
	      if (_first[*_highest] != INVALID) {
		_prev->set(_first[*_highest], n);
	      } else {
		_last[*_highest] = n;
	      }
	      _first[*_highest] = n;	      
	    }
	  } else {

	    deactivate(n);
	    if (!(*_active)[_first[*_highest]]) {
	      ++_highest;
	      if (_highest != _sets.back().end() && 
		  !(*_active)[_first[*_highest]]) {
		_highest = _sets.back().end();
	      }
	    }
	  }
	}

	if ((*_excess)[target] < _min_cut) {
	  _min_cut = (*_excess)[target];
	  for (NodeIt i(_graph); i != INVALID; ++i) {
	    _min_cut_map->set(i, true);
	  }
	  for (std::list<int>::iterator it = _sets.back().begin();
	       it != _sets.back().end(); ++it) {
	    Node n = _first[*it];
	    while (n != INVALID) {
	      _min_cut_map->set(n, false);
	      n = (*_next)[n];
	    }
	  }
	}

	{
	  Node new_target;
	  if ((*_prev)[target] != INVALID) {
	    _last[(*_bucket)[target]] = (*_prev)[target];
	    _next->set((*_prev)[target], INVALID);
	    new_target = (*_prev)[target];
	  } else {
	    _sets.back().pop_back();
	    if (_sets.back().empty()) {
	      _sets.pop_back();
	      if (_sets.empty())
		break;
	      for (std::list<int>::iterator it = _sets.back().begin();
		   it != _sets.back().end(); ++it) {
		_dormant[*it] = false;
	      }
	    }
	    new_target = _last[_sets.back().back()];
	  }

	  _bucket->set(target, 0);

	  _source_set->set(target, true);	  
	  for (OutEdgeIt e(_graph, target); e != INVALID; ++e) {
	    Value rem = (*_capacity)[e] - (*_flow)[e];
	    if (!_tolerance.positive(rem)) continue;
	    Node v = _graph.target(e);
	    if (!(*_active)[v] && !(*_source_set)[v]) {
	      activate(v);
	    }
	    _excess->set(v, (*_excess)[v] + rem);
	    _flow->set(e, (*_capacity)[e]);
	  }
	  
	  for (InEdgeIt e(_graph, target); e != INVALID; ++e) {
	    Value rem = (*_flow)[e];
	    if (!_tolerance.positive(rem)) continue;
	    Node v = _graph.source(e);
	    if (!(*_active)[v] && !(*_source_set)[v]) {
	      activate(v);
	    }
	    _excess->set(v, (*_excess)[v] + rem);
	    _flow->set(e, 0);
	  }
	  
	  target = new_target;
	  if ((*_active)[target]) {
	    deactivate(target);
	  }

	  _highest = _sets.back().begin();
	  while (_highest != _sets.back().end() && 
		 !(*_active)[_first[*_highest]]) {
	    ++_highest;
	  }
	}
      }
    }    

    void findMinCutIn() {

      for (NodeIt n(_graph); n != INVALID; ++n) {
	_excess->set(n, 0);
      }

      for (EdgeIt e(_graph); e != INVALID; ++e) {
	_flow->set(e, 0);
      }

      int bucket_num = 1;
      
      {
	typename Graph::template NodeMap<bool> reached(_graph, false);
	
	reached.set(_source, true);

	bool first_set = true;

	for (NodeIt t(_graph); t != INVALID; ++t) {
	  if (reached[t]) continue;
	  _sets.push_front(std::list<int>());
	  _sets.front().push_front(bucket_num);
	  _dormant[bucket_num] = !first_set;

	  _bucket->set(t, bucket_num);
	  _first[bucket_num] = _last[bucket_num] = t;
	  _next->set(t, INVALID);
	  _prev->set(t, INVALID);

	  ++bucket_num;
	  
	  std::vector<Node> queue;
	  queue.push_back(t);
	  reached.set(t, true);
	  
	  while (!queue.empty()) {
	    _sets.front().push_front(bucket_num);
	    _dormant[bucket_num] = !first_set;
	    _first[bucket_num] = _last[bucket_num] = INVALID;
	    
	    std::vector<Node> nqueue;
	    for (int i = 0; i < int(queue.size()); ++i) {
	      Node n = queue[i];
	      for (OutEdgeIt e(_graph, n); e != INVALID; ++e) {
		Node u = _graph.target(e);
		if (!reached[u] && _tolerance.positive((*_capacity)[e])) {
		  reached.set(u, true);
		  addItem(u, bucket_num);
		  nqueue.push_back(u);
		}
	      }
	    }
	    queue.swap(nqueue);
	    ++bucket_num;
	  }
	  _sets.front().pop_front();
	  --bucket_num;
	  first_set = false;
	}

	_bucket->set(_source, 0);
	_dormant[0] = true;
      }
      _source_set->set(_source, true);	  
	  
      Node target = _last[_sets.back().back()];
      {
	for (InEdgeIt e(_graph, _source); e != INVALID; ++e) {
	  if (_tolerance.positive((*_capacity)[e])) {
	    Node u = _graph.source(e);
	    _flow->set(e, (*_capacity)[e]);
	    _excess->set(u, (*_excess)[u] + (*_capacity)[e]);
	    if (!(*_active)[u] && u != _source) {
	      activate(u);
	    }
	  }
	}
	if ((*_active)[target]) {
	  deactivate(target);
	}
	
	_highest = _sets.back().begin();
	while (_highest != _sets.back().end() && 
	       !(*_active)[_first[*_highest]]) {
	  ++_highest;
	}
      }


      while (true) {
	while (_highest != _sets.back().end()) {
	  Node n = _first[*_highest];
	  Value excess = (*_excess)[n];
	  int next_bucket = _node_num;

	  int under_bucket;
	  if (++std::list<int>::iterator(_highest) == _sets.back().end()) {
	    under_bucket = -1;
	  } else {
	    under_bucket = *(++std::list<int>::iterator(_highest));
	  }

	  for (InEdgeIt e(_graph, n); e != INVALID; ++e) {
	    Node v = _graph.source(e);
	    if (_dormant[(*_bucket)[v]]) continue;
	    Value rem = (*_capacity)[e] - (*_flow)[e];
	    if (!_tolerance.positive(rem)) continue;
	    if ((*_bucket)[v] == under_bucket) {
	      if (!(*_active)[v] && v != target) {
		activate(v);
	      }
	      if (!_tolerance.less(rem, excess)) {
		_flow->set(e, (*_flow)[e] + excess);
		_excess->set(v, (*_excess)[v] + excess);
		excess = 0;
		goto no_more_push;
	      } else {
		excess -= rem;
		_excess->set(v, (*_excess)[v] + rem);
		_flow->set(e, (*_capacity)[e]);
	      }
	    } else if (next_bucket > (*_bucket)[v]) {
	      next_bucket = (*_bucket)[v];
	    }
	  }

	  for (OutEdgeIt e(_graph, n); e != INVALID; ++e) {
	    Node v = _graph.target(e);
	    if (_dormant[(*_bucket)[v]]) continue;
	    Value rem = (*_flow)[e];
	    if (!_tolerance.positive(rem)) continue;
	    if ((*_bucket)[v] == under_bucket) {
	      if (!(*_active)[v] && v != target) {
		activate(v);
	      }
	      if (!_tolerance.less(rem, excess)) {
		_flow->set(e, (*_flow)[e] - excess);
		_excess->set(v, (*_excess)[v] + excess);
		excess = 0;
		goto no_more_push;
	      } else {
		excess -= rem;
		_excess->set(v, (*_excess)[v] + rem);
		_flow->set(e, 0);
	      }
	    } else if (next_bucket > (*_bucket)[v]) {
	      next_bucket = (*_bucket)[v];
	    }
	  }
	  
	no_more_push:
	  
	  _excess->set(n, excess);
	  
	  if (excess != 0) {
	    if ((*_next)[n] == INVALID) {
	      typename std::list<std::list<int> >::iterator new_set = 
		_sets.insert(--_sets.end(), std::list<int>());
	      new_set->splice(new_set->end(), _sets.back(), 
			      _sets.back().begin(), ++_highest);
	      for (std::list<int>::iterator it = new_set->begin();
		   it != new_set->end(); ++it) {
		_dormant[*it] = true;
	      }
	      while (_highest != _sets.back().end() && 
		     !(*_active)[_first[*_highest]]) {
		++_highest;
	      }
	    } else if (next_bucket == _node_num) {
	      _first[(*_bucket)[n]] = (*_next)[n];
	      _prev->set((*_next)[n], INVALID);
	      
	      std::list<std::list<int> >::iterator new_set = 
		_sets.insert(--_sets.end(), std::list<int>());
	      
	      new_set->push_front(bucket_num);
	      _bucket->set(n, bucket_num);
	      _first[bucket_num] = _last[bucket_num] = n;
	      _next->set(n, INVALID);
	      _prev->set(n, INVALID);
	      _dormant[bucket_num] = true;
	      ++bucket_num;

	      while (_highest != _sets.back().end() && 
		     !(*_active)[_first[*_highest]]) {
		++_highest;
	      }
	    } else {
	      _first[*_highest] = (*_next)[n];
	      _prev->set((*_next)[n], INVALID);

	      while (next_bucket != *_highest) {
		--_highest;
	      }
	      if (_highest == _sets.back().begin()) {
		_sets.back().push_front(bucket_num);
		_dormant[bucket_num] = false;
		_first[bucket_num] = _last[bucket_num] = INVALID;
		++bucket_num;
	      }
	      --_highest;

	      _bucket->set(n, *_highest);
	      _next->set(n, _first[*_highest]);
	      if (_first[*_highest] != INVALID) {
		_prev->set(_first[*_highest], n);
	      } else {
		_last[*_highest] = n;
	      }
	      _first[*_highest] = n;	      
	    }
	  } else {

	    deactivate(n);
	    if (!(*_active)[_first[*_highest]]) {
	      ++_highest;
	      if (_highest != _sets.back().end() && 
		  !(*_active)[_first[*_highest]]) {
		_highest = _sets.back().end();
	      }
	    }
	  }
	}

	if ((*_excess)[target] < _min_cut) {
	  _min_cut = (*_excess)[target];
	  for (NodeIt i(_graph); i != INVALID; ++i) {
	    _min_cut_map->set(i, false);
	  }
	  for (std::list<int>::iterator it = _sets.back().begin();
	       it != _sets.back().end(); ++it) {
	    Node n = _first[*it];
	    while (n != INVALID) {
	      _min_cut_map->set(n, true);
	      n = (*_next)[n];
	    }
	  }
	}

	{
	  Node new_target;
	  if ((*_prev)[target] != INVALID) {
	    _last[(*_bucket)[target]] = (*_prev)[target];
	    _next->set((*_prev)[target], INVALID);
	    new_target = (*_prev)[target];
	  } else {
	    _sets.back().pop_back();
	    if (_sets.back().empty()) {
	      _sets.pop_back();
	      if (_sets.empty())
		break;
	      for (std::list<int>::iterator it = _sets.back().begin();
		   it != _sets.back().end(); ++it) {
		_dormant[*it] = false;
	      }
	    }
	    new_target = _last[_sets.back().back()];
	  }

	  _bucket->set(target, 0);

	  _source_set->set(target, true);	  
	  for (InEdgeIt e(_graph, target); e != INVALID; ++e) {
	    Value rem = (*_capacity)[e] - (*_flow)[e];
	    if (!_tolerance.positive(rem)) continue;
	    Node v = _graph.source(e);
	    if (!(*_active)[v] && !(*_source_set)[v]) {
	      activate(v);
	    }
	    _excess->set(v, (*_excess)[v] + rem);
	    _flow->set(e, (*_capacity)[e]);
	  }
	  
	  for (OutEdgeIt e(_graph, target); e != INVALID; ++e) {
	    Value rem = (*_flow)[e];
	    if (!_tolerance.positive(rem)) continue;
	    Node v = _graph.target(e);
	    if (!(*_active)[v] && !(*_source_set)[v]) {
	      activate(v);
	    }
	    _excess->set(v, (*_excess)[v] + rem);
	    _flow->set(e, 0);
	  }
	  
	  target = new_target;
	  if ((*_active)[target]) {
	    deactivate(target);
	  }

	  _highest = _sets.back().begin();
	  while (_highest != _sets.back().end() && 
		 !(*_active)[_first[*_highest]]) {
	    ++_highest;
	  }
	}
      }
    }

  public:

    /// \name Execution control
    /// The simplest way to execute the algorithm is to use
    /// one of the member functions called \c run(...).
    /// \n
    /// If you need more control on the execution,
    /// first you must call \ref init(), then the \ref calculateIn() or
    /// \ref calculateIn() functions.

    /// @{

    /// \brief Initializes the internal data structures.
    ///
    /// Initializes the internal data structures. It creates
    /// the maps, residual graph adaptors and some bucket structures
    /// for the algorithm. 
    void init() {
      init(NodeIt(_graph));
    }

    /// \brief Initializes the internal data structures.
    ///
    /// Initializes the internal data structures. It creates
    /// the maps, residual graph adaptor and some bucket structures
    /// for the algorithm. Node \c source  is used as the push-relabel
    /// algorithm's source.
    void init(const Node& source) {
      _source = source;
      
      _node_num = countNodes(_graph);
      
      _first.resize(_node_num);
      _last.resize(_node_num);

      _dormant.resize(_node_num);

      if (!_flow) {
	_flow = new FlowMap(_graph);
      }
      if (!_next) {
	_next = new typename Graph::template NodeMap<Node>(_graph);
      }
      if (!_prev) {
	_prev = new typename Graph::template NodeMap<Node>(_graph);
      }
      if (!_active) {
	_active = new typename Graph::template NodeMap<bool>(_graph);
      }
      if (!_bucket) {
	_bucket = new typename Graph::template NodeMap<int>(_graph);
      }
      if (!_excess) {
	_excess = new ExcessMap(_graph);
      }
      if (!_source_set) {
	_source_set = new SourceSetMap(_graph);
      }
      if (!_min_cut_map) {
	_min_cut_map = new MinCutMap(_graph);
      }

      _min_cut = std::numeric_limits<Value>::max();
    }


    /// \brief Calculates a minimum cut with \f$ source \f$ on the
    /// source-side.
    ///
    /// Calculates a minimum cut with \f$ source \f$ on the
    /// source-side (i.e. a set \f$ X\subsetneq V \f$ with \f$ source
    /// \in X \f$ and minimal out-degree).
    void calculateOut() {
      findMinCutOut();
    }

    /// \brief Calculates a minimum cut with \f$ source \f$ on the
    /// target-side.
    ///
    /// Calculates a minimum cut with \f$ source \f$ on the
    /// target-side (i.e. a set \f$ X\subsetneq V \f$ with \f$ source
    /// \in X \f$ and minimal out-degree).
    void calculateIn() {
      findMinCutIn();
    }


    /// \brief Runs the algorithm.
    ///
    /// Runs the algorithm. It finds nodes \c source and \c target
    /// arbitrarily and then calls \ref init(), \ref calculateOut()
    /// and \ref calculateIn().
    void run() {
      init();
      calculateOut();
      calculateIn();
    }

    /// \brief Runs the algorithm.
    ///
    /// Runs the algorithm. It uses the given \c source node, finds a
    /// proper \c target and then calls the \ref init(), \ref
    /// calculateOut() and \ref calculateIn().
    void run(const Node& s) {
      init(s);
      calculateOut();
      calculateIn();
    }

    /// @}
    
    /// \name Query Functions 
    /// The result of the %HaoOrlin algorithm
    /// can be obtained using these functions.
    /// \n
    /// Before using these functions, either \ref run(), \ref
    /// calculateOut() or \ref calculateIn() must be called.
    
    /// @{

    /// \brief Returns the value of the minimum value cut.
    /// 
    /// Returns the value of the minimum value cut.
    Value minCut() const {
      return _min_cut;
    }


    /// \brief Returns a minimum cut.
    ///
    /// Sets \c nodeMap to the characteristic vector of a minimum
    /// value cut: it will give a nonempty set \f$ X\subsetneq V \f$
    /// with minimal out-degree (i.e. \c nodeMap will be true exactly
    /// for the nodes of \f$ X \f$).  \pre nodeMap should be a
    /// bool-valued node-map.
    template <typename NodeMap>
    Value minCut(NodeMap& nodeMap) const {
      for (NodeIt it(_graph); it != INVALID; ++it) {
	nodeMap.set(it, (*_min_cut_map)[it]);
      }
      return minCut();
    }

    /// @}
    
  }; //class HaoOrlin 


} //namespace lemon

#endif //LEMON_HAO_ORLIN_H
