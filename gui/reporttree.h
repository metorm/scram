/*
 * Copyright (C) 2017-2018 Olzhas Rakhimov
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

/// @file
/// Report tree model to represent the analysis results.

#pragma once

#include <vector>

#include <QAbstractItemModel>

#ifndef Q_MOC_RUN
#include "src/risk_analysis.h"
#endif

namespace scram {
namespace gui {

/// The report is organized by its top items as analysis identifiers
/// and its descendant items as analysis result types.
///
/// @note The report tree does not track the data change.
///       If analysis is redone,
///       the report tree must be replaced.
class ReportTree : public QAbstractItemModel
{
    Q_OBJECT

public:
    /// The child row analysis item types.
    enum class Row { Products, Probability, Importance };

    /// @param[in] results  The analysis result data.
    /// @param[in,out] parent  The optional owner of the object.
    ///
    /// @pre The analysis results do not change over the report tree lifetime.
    /// @pre The analysis results live at least as long as this report tree.
    /// @pre At least fault tree analysis has been performed.
    explicit ReportTree(const std::vector<core::RiskAnalysis::Result> *results,
                        QObject *parent = nullptr);

    /// Required standard member functions of QAbstractItemModel interface.
    /// @{
    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    /// @}

private:
    const std::vector<core::RiskAnalysis::Result> &m_results; ///< The data.
};

}
} // namespace scram::gui
