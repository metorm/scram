/*
 * Copyright (C) 2015 Olzhas Rakhimov
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/// @file zbdd.h
/// Zero-Suppressed Binary Decision Diagram facilities.

#ifndef SCRAM_SRC_ZBDD_H_
#define SCRAM_SRC_ZBDD_H_

#include "bdd.h"

namespace scram {

/// @enum SetOp
/// Operations on sets.
enum class SetOp {
  Without = 0  ///< Without '\' operator.
};

/// @class SetNode
/// Representation of non-terminal nodes in ZBDD.
class SetNode : public NonTerminal {
 public:
  using NonTerminal::NonTerminal;  ///< Constructor with index and order.

  /// @returns Whatever count is stored in this node.
  int count() const { return count_; }

  /// Stores numerical value for later retrieval.
  /// This is a helper functionality
  /// for counting the number of sets or nodes.
  ///
  /// @param[in] number  A number with a meaning for the caller.
  void count(int number) { count_ = number; }

  /// @returns Cut sets found in the ZBDD represented by this node.
  const std::vector<std::vector<int>>& cut_sets() const { return cut_sets_; }

  /// Sets the cut sets belonging to this ZBDD.
  ///
  /// @param[in] cut_sets  Cut sets calculated from low and high edges.
  void cut_sets(const std::vector<std::vector<int>>& cut_sets) {
    cut_sets_ = cut_sets;
  }

  /// Recovers a shared pointer to SetNode from a pointer to Vertex.
  ///
  /// @param[in] vertex  Pointer to a Vertex known to be a SetNode.
  ///
  /// @return Casted pointer to SetNode.
  static std::shared_ptr<SetNode> Ptr(const std::shared_ptr<Vertex>& vertex) {
    return std::static_pointer_cast<SetNode>(vertex);
  }

 private:
  std::vector<std::vector<int>> cut_sets_;  ///< Cut sets of this node.
  int count_ = 0;  ///< The number of cut sets, nodes, or anything else.
};

/// @class Zbdd
/// Zero-Suppressed Binary Decision Diagrams for set manipulations.
class Zbdd {
 public:
  /// Converts Reduced Ordered BDD
  /// into Zero-Suppressed BDD.
  ///
  /// @param[in] bdd  ROBDD with the ITE vertices.
  ///
  /// @pre Boolean graph is coherent (monotonic).
  /// @pre BDD has attributed edges with only one terminal (1/True).
  explicit Zbdd(const Bdd* bdd) noexcept;

  /// Runs the analysis
  /// with the representation of a Boolean graph
  /// as ZBDD.
  ///
  /// @pre Only coherent analysis is requested.
  void Analyze() noexcept;

  /// @returns Cut sets generated by the analysis.
  const std::vector<std::vector<int>>& cut_sets() const { return cut_sets_; }

 private:
  using VertexPtr = std::shared_ptr<Vertex>;
  using TerminalPtr = std::shared_ptr<Terminal>;
  using ItePtr = std::shared_ptr<Ite>;
  using SetNodePtr = std::shared_ptr<SetNode>;
  using UniqueTable = TripletTable<SetNodePtr>;
  using ComputeTable = TripletTable<VertexPtr>;
  using CutSet = std::vector<int>;

  Zbdd() noexcept;  ///< Default constructor to initialize member variables.

  /// Converts BDD graph into ZBDD graph.
  ///
  /// @param[in] vertex  Vertex of the ROBDD graph.
  /// @param[in] complement  Interpretation of the vertex as complement.
  /// @param[in] bdd_graph  The main ROBDD as helper database.
  ///
  /// @returns Pointer to the root vertex of the ZBDD graph.
  VertexPtr ConvertBdd(const VertexPtr& vertex, bool complement,
                       const Bdd* bdd_graph) noexcept;

  /// Removes subsets in ZBDD.
  ///
  /// @param[in] vertex  The variable node in the set.
  ///
  /// @returns Processed vertex.
  VertexPtr Minimize(const VertexPtr& vertex) noexcept;

  /// Applies subsume operation on two sets.
  /// Subsume operation removes
  /// paths that exist in Low branch from High branch.
  ///
  /// @param[in] high  True/then/high branch of a variable.
  /// @param[in] low  False/else/low branch of a variable.
  ///
  /// @returns Minimized high branch for a variable.
  VertexPtr Subsume(const VertexPtr& high, const VertexPtr& low) noexcept;

  /// Traverses the reduced ZBDD graph to generate cut sets.
  ///
  /// @param[in] vertex  The root node in traversal.
  ///
  /// @returns A collection of cut sets
  ///          generated from the ZBDD subgraph.
  std::vector<std::vector<int>>
  GenerateCutSets(const VertexPtr& vertex) noexcept;

  /// Counts the number of SetNodes.
  ///
  /// @param[in] vertex  The root vertex to start counting.
  ///
  /// @returns The total number of SetNode vertices
  ///          including vertices in modules.
  ///
  /// @pre SetNode marks are clear (false).
  int CountSetNodes(const VertexPtr& vertex) noexcept;

  /// Counts the total number of sets in ZBDD.
  ///
  /// @param[in] vertex  The root vertex of ZBDD.
  ///
  /// @returns The number of cut sets in ZBDD.
  ///
  /// @pre SetNode marks are clear (false).
  ///
  /// @warning Integer may overflow for a large ZBDD.
  int CountCutSets(const VertexPtr& vertex) noexcept;

  /// Cleans up non-terminal vertex marks
  /// by setting them to "false".
  ///
  /// @param[in] vertex  The root vertex of the graph.
  ///
  /// @pre The graph is marked "true" contiguously.
  void ClearMarks(const VertexPtr& vertex) noexcept;

  /// Table of unique SetNodes denoting sets.
  /// The key consists of (index, id_high, id_low) triplet.
  UniqueTable unique_table_;

  /// Table of processed computations over sets.
  /// The key must convey the semantics of the operation over sets.
  /// The argument functions are recorded with their IDs (not vertex indices).
  /// In order to keep only unique computations,
  /// the argument IDs must be ordered.
  ComputeTable compute_table_;

  VertexPtr root_;  ///< The root vertex of ZBDD.
  std::unordered_map<int, VertexPtr> ites_;  ///< Processed function graphs.
  std::unordered_map<int, VertexPtr> modules_;  ///< Module graphs.
  const TerminalPtr kBase_;  ///< Terminal Base (Unity/1) set.
  const TerminalPtr kEmpty_;  ///< Terminal Empty (Null/0) set.
  int set_id_;  ///< Identification assignment for new set graphs.
  std::unordered_map<int, VertexPtr> minimal_results_;  ///< Memorize minimal.
  std::vector<CutSet> cut_sets_;  ///< Generated cut sets.
};

}  // namespace scram

#endif  // SCRAM_SRC_ZBDD_H_
