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

#include "risk_analysis_tests.h"

#include <gtest/gtest.h>

#include "bdd.h"
#include "fault_tree.h"

namespace scram {
namespace test {

TEST_F(RiskAnalysisTest, BddTest) {
  std::string tree_input =
      "./share/scram/input/fta/correct_tree_input_with_probs.xml";
  settings.probability_analysis(true);
  ASSERT_NO_THROW(ProcessInputFile(tree_input));
  GatePtr top_gate = fault_tree()->top_events().front();
  BooleanGraph* graph = new BooleanGraph(top_gate);
  Bdd* bdd = new Bdd(graph);
  bdd->Analyze();
  EXPECT_EQ(0.646, bdd->p_graph());
  delete graph;
  delete bdd;
}

TEST_F(RiskAnalysisTest, BddProb) {
  std::vector<std::string> input_files;
  input_files.push_back("./share/scram/input/Chinese/chinese.xml");
  input_files.push_back("./share/scram/input/Chinese/chinese-basic-events.xml");
  settings.probability_analysis(true);
  ASSERT_NO_THROW(ProcessInputFiles(input_files));
  GatePtr top_gate = fault_tree()->top_events().front();
  BooleanGraph* graph = new BooleanGraph(top_gate);
  Bdd* bdd = new Bdd(graph);
  bdd->Analyze();
  EXPECT_NEAR(0.0045691, bdd->p_graph(), 1e-5);
  delete graph;
  delete bdd;
}

}  // namespace test
}  // namespace scram