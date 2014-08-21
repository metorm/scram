/// @file fault_tree_analysis.cc
/// Implementation of fault tree analysis.
#include "fault_tree_analysis.h"

#include <algorithm>
#include <cmath>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <sstream>
#include <vector>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/date_time.hpp>

namespace fs = boost::filesystem;
namespace pt = boost::posix_time;

namespace scram {

FaultTreeAnalysis::FaultTreeAnalysis(std::string analysis,
                                     std::string approx,
                                     int limit_order, int nsums)
    : warnings_(""),
      top_event_index_(-1),
      input_file_("deal_in_future"),
      prob_requested_(false),
      analysis_done_(false),
      max_order_(1),
      p_total_(0),
      exp_time_(0),
      mcs_time_(0),
      p_time_(0) {
  // Check for valid analysis type.
  if (analysis != "default" && analysis != "mc") {
    std::string msg = "The analysis type is not recognized.";
    throw scram::ValueError(msg);
  }
  analysis_ = analysis;

  // Check for right limit order.
  if (limit_order < 1) {
    std::string msg = "The limit on the order of minimal cut sets "
                      "cannot be less than one.";
    throw scram::ValueError(msg);
  }
  limit_order_ = limit_order;

  // Check for right limit order.
  if (nsums < 1) {
    std::string msg = "The number of sums in the probability calculation "
                      "cannot be less than one";
    throw scram::ValueError(msg);
  }
  nsums_ = nsums;

  // Check the right approximation for probability calculations.
  if (approx != "no" && approx != "rare" && approx != "mcub") {
    std::string msg = "The probability approximation is not recognized.";
    throw scram::ValueError(msg);
  }
  approx_ = approx;

  // Add valid gates.
  gates_.insert("and");
  gates_.insert("or");
  gates_.insert("not");
  gates_.insert("nor");
  gates_.insert("nand");
  gates_.insert("xor");
  gates_.insert("null");
  gates_.insert("inhibit");
  gates_.insert("vote");

  // Add valid primary event types.
  types_.insert("basic");
  types_.insert("undeveloped");
  types_.insert("house");
  types_.insert("conditional");

  // Pointer to the top event.
  GatePtr top_event_;

  // Initialize a fault tree with a default name.
  FaultTreePtr fault_tree_;
}

void FaultTreeAnalysis::Analyze(
    const FaultTreePtr& fault_tree,
    const std::map<std::string, std::string>& orig_ids,
    bool prob_requested) {

  // Timing Initialization
  std::clock_t start_time;
  start_time = std::clock();
  // End of Timing Initialization

  // Container for cut sets with intermediate events.
  std::vector< SupersetPtr > inter_sets;

  // Container for cut sets with primary events only.
  std::vector< std::set<int> > cut_sets;

  orig_ids_ = orig_ids;
  prob_requested_ = prob_requested;

  FaultTreeAnalysis::AssignIndices(fault_tree);

  FaultTreeAnalysis::ExpandSets(top_event_index_, inter_sets);

  // An iterator for a vector with sets of ids of events.
  std::vector< std::set<int> >::iterator it_vec;

  // An iterator for a vector with Supersets.
  std::vector< SupersetPtr >::iterator it_sup;

  // Generate cut sets.
  while (!inter_sets.empty()) {
    // Get rightmost set.
    SupersetPtr tmp_set = inter_sets.back();
    // Delete rightmost set.
    inter_sets.pop_back();

    // Discard this tmp set if it is larger than the limit.
    if (tmp_set->NumOfPrimeEvents() > limit_order_) continue;

    if (tmp_set->NumOfGates() == 0) {
      // This is a set with primary events only.
      cut_sets.push_back(tmp_set->primes());
      continue;
    }

    // To hold sets of children.
    std::vector< SupersetPtr > children_sets;

    FaultTreeAnalysis::ExpandSets(tmp_set->PopGate(), children_sets);

    // Attach the original set into this event's sets of children.
    for (it_sup = children_sets.begin(); it_sup != children_sets.end();
         ++it_sup) {
      // Add this set to the original inter_sets.
      if ((*it_sup)->InsertSet(tmp_set)) inter_sets.push_back(*it_sup);
    }
  }

  // Duration of the expansion.
  exp_time_ = (std::clock() - start_time) / static_cast<double>(CLOCKS_PER_SEC);

  // At this point cut sets are generated.
  // Now we need to reduce them to minimal cut sets.

  // First, defensive check if cut sets exist for the specified limit order.
  if (cut_sets.empty()) {
    std::stringstream msg;
    msg << "No cut sets for the limit order " <<  limit_order_;
    warnings_ += msg.str();
    analysis_done_ = true;
    return;
  }

  // Choose to convert vector to a set to get rid of any duplications.
  std::set< std::set<int> > unique_cut_sets;
  for (it_vec = cut_sets.begin(); it_vec != cut_sets.end(); ++it_vec) {
    if (it_vec->size() == 1) {
      // Minimal cut set is detected.
      imcs_.insert(*it_vec);
      continue;
    }
    unique_cut_sets.insert(*it_vec);
  }

  FaultTreeAnalysis::FindMCS(unique_cut_sets, imcs_, 2);
  // Duration of MCS generation.
  mcs_time_ = (std::clock() - start_time) / static_cast<double>(CLOCKS_PER_SEC);
  FaultTreeAnalysis::SetsToString();  // MCS with event ids.

  analysis_done_ = true;  // Main analysis enough for reporting is done.

  // Compute probabilities only if requested.
  if (!prob_requested_) return;

  // Maximum number of sums in the series.
  if (nsums_ > imcs_.size()) nsums_ = imcs_.size();

  // Perform Monte Carlo Uncertainty analysis.
  if (analysis_ == "mc") {
    // Generate the equation.
    FaultTreeAnalysis::MProbOr(imcs_, 1, nsums_);
    // Sample probabilities and generate data.
    FaultTreeAnalysis::MSample();
    return;
  }

  // Iterator for minimal cut sets.
  std::set< std::set<int> >::iterator it_min;

  // Iterate minimal cut sets and find probabilities for each set.
  for (it_min = imcs_.begin(); it_min != imcs_.end(); ++it_min) {
    // Calculate a probability of a set with AND relationship.
    double p_sub_set = FaultTreeAnalysis::ProbAnd(*it_min);
    // Update a container with minimal cut sets and probabilities.
    prob_of_min_sets_.insert(std::make_pair(imcs_to_smcs_[*it_min],
                                            p_sub_set));
    ordered_min_sets_.insert(std::make_pair(p_sub_set,
                                            imcs_to_smcs_[*it_min]));
  }

  // Check if the rare event approximation is requested.
  if (approx_ == "rare") {
    warnings_ += "Using the rare event approximation\n";
    bool rare_event_legit = true;
    std::map< std::set<std::string>, double >::iterator it_pr;
    for (it_pr = prob_of_min_sets_.begin();
         it_pr != prob_of_min_sets_.end(); ++it_pr) {
      // Check if a probability of a set does not exceed 0.1,
      // which is required for the rare event approximation to hold.
      if (rare_event_legit && (it_pr->second > 0.1)) {
        rare_event_legit = false;
        warnings_ += "The rare event approximation may be inaccurate for this"
            "\nfault tree analysis because one of minimal cut sets'"
            "\nprobability exceeded 0.1 threshold requirement.\n\n";
      }
      p_total_ += it_pr->second;
    }

  } else if (approx_ == "mcub") {
    warnings_ += "Using the MCUB approximation\n";
    double m = 1;
    std::map< std::set<std::string>, double >::iterator it;
    for (it = prob_of_min_sets_.begin(); it != prob_of_min_sets_.end();
         ++it) {
      m *= 1 - it->second;
    }
    p_total_ = 1 - m;

  } else {  // No approximation technique is assumed.
    // Exact calculation of probability of cut sets.
    p_total_ = FaultTreeAnalysis::ProbOr(imcs_, nsums_);
  }

  // Calculate failure contributions of each primary event.
  boost::unordered_map<std::string, PrimaryEventPtr>::iterator it_prime;
  for (it_prime = primary_events_.begin(); it_prime != primary_events_.end();
       ++it_prime) {
    double contrib_pos = 0;  // Total positive contribution of this event.
    double contrib_neg = 0;  // Negative event contribution.
    std::map< std::set<std::string>, double >::iterator it_pr;
    for (it_pr = prob_of_min_sets_.begin();
         it_pr != prob_of_min_sets_.end(); ++it_pr) {
      if (it_pr->first.count(it_prime->first)) {
        contrib_pos += it_pr->second;
      } else if (it_pr->first.count("not " + it_prime->first)) {
        contrib_neg += it_pr->second;
      }
    }
    imp_of_primaries_.insert(std::make_pair(it_prime->first, contrib_pos));
    ordered_primaries_.insert(std::make_pair(contrib_pos, it_prime->first));
    if (contrib_neg > 0) {
      imp_of_primaries_.insert(std::make_pair("not " + it_prime->first,
                                              contrib_neg));
      ordered_primaries_.insert(std::make_pair(contrib_neg,
                                               "not " + it_prime->first));
    }
  }
  // Duration of probability related operations.
  p_time_ = (std::clock() - start_time) / static_cast<double>(CLOCKS_PER_SEC);}

void FaultTreeAnalysis::ExpandSets(int inter_index,
                                    std::vector< SupersetPtr >& sets) {
  // Populate intermediate and primary events of the top.
  const std::map<std::string, EventPtr>* children =
      &int_to_inter_[std::abs(inter_index)]->children();

  std::string gate = int_to_inter_[std::abs(inter_index)]->type();

  // Iterator for children of top and intermediate events.
  std::map<std::string, EventPtr>::const_iterator it_children;
  std::vector<int> events_children;
  std::vector<int>::iterator it_child;

  for (it_children = children->begin();
       it_children != children->end(); ++it_children) {
    if (inter_events_.count(it_children->first)) {
      events_children.push_back(inter_to_int_[it_children->first]);
    } else {
      events_children.push_back(prime_to_int_[it_children->first]);
    }
  }

  // Type dependent logic.
  if (gate == "or") {
    assert(events_children.size() > 1);
    if (inter_index > 0) {
      FaultTreeAnalysis::SetOr(events_children, sets);
    } else {
      FaultTreeAnalysis::SetAnd(events_children, sets, -1);
    }
  } else if (gate == "and") {
    assert(events_children.size() > 1);
    if (inter_index > 0) {
      FaultTreeAnalysis::SetAnd(events_children, sets);
    } else {
      FaultTreeAnalysis::SetOr(events_children, sets, -1);
    }
  } else if (gate == "not") {
    int mult = 1;
    if (inter_index < 0) mult = -1;
    // Only one child is expected.
    assert(events_children.size() == 1);
    FaultTreeAnalysis::SetAnd(events_children, sets, -1 * mult);
  } else if (gate == "nor") {
    assert(events_children.size() > 1);
    if (inter_index > 0) {
      FaultTreeAnalysis::SetAnd(events_children, sets, -1);
    } else {
      FaultTreeAnalysis::SetOr(events_children, sets);
    }
  } else if (gate == "nand") {
    assert(events_children.size() > 1);
    if (inter_index > 0) {
      FaultTreeAnalysis::SetOr(events_children, sets, -1);
    } else {
      FaultTreeAnalysis::SetAnd(events_children, sets);
    }
  } else if (gate == "xor") {
    assert(events_children.size() == 2);
    SupersetPtr tmp_set_one(new scram::Superset());
    SupersetPtr tmp_set_two(new scram::Superset());
    if (inter_index > 0) {
      int j = 1;
      for (it_child = events_children.begin();
           it_child != events_children.end(); ++it_child) {
        if (*it_child > top_event_index_) {
          tmp_set_one->InsertGate(j * (*it_child));
          tmp_set_two->InsertGate(-1 * j * (*it_child));
        } else {
          tmp_set_one->InsertPrimary(j * (*it_child));
          tmp_set_two->InsertPrimary(-1 * j * (*it_child));
        }
        j = -1;
      }
    } else {
      for (it_child = events_children.begin();
           it_child != events_children.end(); ++it_child) {
        if (*it_child > top_event_index_) {
          tmp_set_one->InsertGate(*it_child);
          tmp_set_two->InsertGate(-1 * (*it_child));
        } else {
          tmp_set_one->InsertPrimary(*it_child);
          tmp_set_two->InsertPrimary(-1 * (*it_child));
        }
      }
    }
    sets.push_back(tmp_set_one);
    sets.push_back(tmp_set_two);
  } else if (gate == "null") {
    int mult = 1;
    if (inter_index < 0) mult = -1;
    // Only one child is expected.
    assert(events_children.size() == 1);
    FaultTreeAnalysis::SetAnd(events_children, sets, mult);
  } else if (gate == "inhibit") {
    assert(events_children.size() == 2);
    if (inter_index > 0) {
      FaultTreeAnalysis::SetAnd(events_children, sets);
    } else {
      FaultTreeAnalysis::SetOr(events_children, sets, -1);
    }
  } else if (gate == "vote") {
    int vote_number = int_to_inter_[std::abs(inter_index)]->vote_number();
    assert(vote_number > 1);
    assert(events_children.size() >= vote_number);
    std::set< std::set<int> > all_sets;
    int size = events_children.size();

    for (int j = 0; j < size; ++j) {
      std::set<int> set;
      set.insert(events_children[j]);
      all_sets.insert(set);
    }

    int mult = 1;
    if (inter_index < 0) {
      mult = -1;
      vote_number = size - vote_number + 1;  // The main trick for negation.
    }

    for (int i = 1; i < vote_number; ++i) {
      std::set< std::set<int> > tmp_sets;
      std::set< std::set<int> >::iterator it_sets;
      for (it_sets = all_sets.begin(); it_sets != all_sets.end(); ++it_sets) {
        for (int j = 0; j < size; ++j) {
          std::set<int> set = *it_sets;
          set.insert(events_children[j]);
          if (set.size() > i) {
            tmp_sets.insert(set);
          }
        }
      }
      all_sets = tmp_sets;
    }

    std::set< std::set<int> >::iterator it_sets;
    for (it_sets = all_sets.begin(); it_sets != all_sets.end(); ++it_sets) {
      SupersetPtr tmp_set_c(new scram::Superset());
      std::set<int>::iterator it;
      for (it = it_sets->begin(); it != it_sets->end(); ++it) {
        if (*it > top_event_index_) {
          tmp_set_c->InsertGate(*it * mult);
        } else {
          tmp_set_c->InsertPrimary(*it * mult);
        }
      }
      sets.push_back(tmp_set_c);
    }

  } else {
    boost::to_upper(gate);
    std::string msg = "No algorithm defined for " + gate;
    throw scram::ValueError(msg);
  }
}

void FaultTreeAnalysis::SetOr(std::vector<int>& events_children,
                       std::vector<SupersetPtr>& sets, int mult) {
  std::vector<int>::iterator it_child;
  for (it_child = events_children.begin();
       it_child != events_children.end(); ++it_child) {
    SupersetPtr tmp_set_c(new scram::Superset());
    if (*it_child > top_event_index_) {
      tmp_set_c->InsertGate(*it_child * mult);
    } else {
      tmp_set_c->InsertPrimary(*it_child * mult);
    }
    sets.push_back(tmp_set_c);
  }
}

void FaultTreeAnalysis::SetAnd(std::vector<int>& events_children,
                        std::vector<SupersetPtr>& sets, int mult) {
  SupersetPtr tmp_set_c(new scram::Superset());
  std::vector<int>::iterator it_child;
  for (it_child = events_children.begin();
       it_child != events_children.end(); ++it_child) {
    if (*it_child > top_event_index_) {
      tmp_set_c->InsertGate(*it_child * mult);
    } else {
      tmp_set_c->InsertPrimary(*it_child * mult);
    }
  }
  sets.push_back(tmp_set_c);
}

void FaultTreeAnalysis::FindMCS(const std::set< std::set<int> >& cut_sets,
                         const std::set< std::set<int> >& mcs_lower_order,
                         int min_order) {
  if (cut_sets.empty()) return;

  // Iterator for cut_sets.
  std::set< std::set<int> >::iterator it_uniq;

  // Iterator for minimal cut sets.
  std::set< std::set<int> >::iterator it_min;

  std::set< std::set<int> > temp_sets;  // For mcs of a level above.
  std::set< std::set<int> > temp_min_sets;  // For mcs of this level.

  for (it_uniq = cut_sets.begin();
       it_uniq != cut_sets.end(); ++it_uniq) {
    bool include = true;  // Determine to keep or not.

    for (it_min = mcs_lower_order.begin(); it_min != mcs_lower_order.end();
         ++it_min) {
      if (std::includes(it_uniq->begin(), it_uniq->end(),
                        it_min->begin(), it_min->end())) {
        // Non-minimal cut set is detected.
        include = false;
        break;
      }
    }
    // After checking for non-minimal cut sets,
    // all minimum sized cut sets are guaranteed to be minimal.
    if (include) {
      if (it_uniq->size() == min_order) {
        temp_min_sets.insert(*it_uniq);
        // Update maximum order of the sets.
        if (min_order > max_order_) max_order_ = min_order;
      } else {
        temp_sets.insert(*it_uniq);
      }
    }
    // Ignore the cut set because include = false.
  }
  imcs_.insert(temp_min_sets.begin(), temp_min_sets.end());
  min_order++;
  FaultTreeAnalysis::FindMCS(temp_sets, temp_min_sets, min_order);
}

// -------------------- Algorithm for Cut Sets and Probabilities -----------
void FaultTreeAnalysis::AssignIndices(const FaultTreePtr& fault_tree) {
  // Assign an index to each primary event, and populate relevant
  // databases.

  // Getting events from the fault tree object.
  top_event_ = fault_tree->top_event();
  inter_events_ = fault_tree->inter_events();
  primary_events_ = fault_tree->primary_events();

  int j = 1;
  boost::unordered_map<std::string, PrimaryEventPtr>::iterator itp;
  // Dummy primary event at index 0.
  int_to_prime_.push_back(PrimaryEventPtr(new PrimaryEvent("dummy")));
  iprobs_.push_back(0);
  for (itp = primary_events_.begin(); itp != primary_events_.end(); ++itp) {
    int_to_prime_.push_back(itp->second);
    prime_to_int_.insert(std::make_pair(itp->second->id(), j));
    if (prob_requested_) iprobs_.push_back(itp->second->p());
    ++j;
  }

  // Assign an index to each top and intermediate event and populate
  // relevant databases.
  top_event_index_ = j;
  int_to_inter_.insert(std::make_pair(j, top_event_));
  inter_to_int_.insert(std::make_pair(top_event_->id(), j));
  ++j;
  boost::unordered_map<std::string, GatePtr>::iterator iti;
  for (iti = inter_events_.begin(); iti != inter_events_.end(); ++iti) {
    int_to_inter_.insert(std::make_pair(j, iti->second));
    inter_to_int_.insert(std::make_pair(iti->second->id(), j));
    ++j;
  }
}

void FaultTreeAnalysis::SetsToString() {
  std::set< std::set<int> >::iterator it_min;
  for (it_min = imcs_.begin(); it_min != imcs_.end(); ++it_min) {
    std::set<std::string> pr_set;
    std::set<int>::iterator it_set;
    for (it_set = it_min->begin(); it_set != it_min->end(); ++it_set) {
      if (*it_set < 0) {  // NOT logic.
        pr_set.insert("not " + int_to_prime_[std::abs(*it_set)]->id());
      } else {
        pr_set.insert(int_to_prime_[*it_set]->id());
      }
    }
    imcs_to_smcs_.insert(std::make_pair(*it_min, pr_set));
    min_cut_sets_.insert(pr_set);
  }
}

double FaultTreeAnalysis::ProbOr(std::set< std::set<int> >& min_cut_sets, int nsums) {
  // Recursive implementation.
  if (min_cut_sets.empty()) return 0;

  if (nsums == 0) return 0;

  // Base case.
  if (min_cut_sets.size() == 1) {
    // Get only element in this set.
    return FaultTreeAnalysis::ProbAnd(*min_cut_sets.begin());
  }

  // Get one element.
  std::set< std::set<int> >::iterator it = min_cut_sets.begin();
  std::set<int> element_one = *it;

  // Delete element from the original set. WARNING: the iterator is invalidated.
  min_cut_sets.erase(it);
  std::set< std::set<int> > combo_sets;
  FaultTreeAnalysis::CombineElAndSet(element_one, min_cut_sets, combo_sets);

  return FaultTreeAnalysis::ProbAnd(element_one) +
         FaultTreeAnalysis::ProbOr(min_cut_sets, nsums) -
         FaultTreeAnalysis::ProbOr(combo_sets, nsums - 1);
}

double FaultTreeAnalysis::ProbAnd(const std::set<int>& min_cut_set) {
  // Test just in case the min cut set is empty.
  if (min_cut_set.empty()) return 0;

  double p_sub_set = 1;  // 1 is for multiplication.
  std::set<int>::iterator it_set;
  for (it_set = min_cut_set.begin(); it_set != min_cut_set.end(); ++it_set) {
    if (*it_set > 0) {
      p_sub_set *= iprobs_[*it_set];
    } else {
      p_sub_set *= 1 - iprobs_[std::abs(*it_set)];
    }
  }
  return p_sub_set;
}

void FaultTreeAnalysis::CombineElAndSet(const std::set<int>& el,
                                 const std::set< std::set<int> >& set,
                                 std::set< std::set<int> >& combo_set) {
  std::set<int> member_set;
  std::set<int>::iterator it;
  std::set< std::set<int> >::iterator it_set;
  for (it_set = set.begin(); it_set != set.end(); ++it_set) {
    member_set = *it_set;
    bool include = true;
    for (it = el.begin(); it != el.end(); ++it) {
      if (it_set->count(-1 * (*it))) {
        include = false;
        break;
      }
      member_set.insert(*it);
    }
    if (include) combo_set.insert(member_set);
  }
}

// ----------------------------------------------------------------------
// ----- Algorithm for Total Equation for Monte Carlo Simulation --------
// Generation of the representation of the original equation.
void FaultTreeAnalysis::MProbOr(std::set< std::set<int> >& min_cut_sets,
                         int sign, int nsums) {
  // Recursive implementation.
  if (min_cut_sets.empty()) return;

  if (nsums == 0) return;

  // Get one element.
  std::set< std::set<int> >::iterator it = min_cut_sets.begin();
  std::set<int> element_one = *it;

  // Delete element from the original set. WARNING: the iterator is invalidated.
  min_cut_sets.erase(it);

  // Put this element into the equation.
  if ((sign % 2) == 1) {
    // This is a positive member.
    pos_terms_.push_back(element_one);
  } else {
    // This must be a negative member.
    neg_terms_.push_back(element_one);
  }

  std::set< std::set<int> > combo_sets;
  FaultTreeAnalysis::CombineElAndSet(element_one, min_cut_sets, combo_sets);
  FaultTreeAnalysis::MProbOr(min_cut_sets, sign, nsums);
  FaultTreeAnalysis::MProbOr(combo_sets, sign + 1, nsums - 1);
  return;
}

void FaultTreeAnalysis::MSample() {}
// ----------------------------------------------------------------------

}  // namespace scram
