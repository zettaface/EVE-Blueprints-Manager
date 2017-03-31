#ifndef SIMPLEX_H
#define SIMPLEX_H

#include <algorithm>
#include <tuple>
#include <vector>
#include <iostream>

class Variable
{
  public:
    enum class Type
    {
      Normal,
      Slack
    };

    Variable(int num, Type type, double val = 0.):
      num_(num),
      type_(type),
      val_(val) {

    }

    double val() const { return val_; }
    void setVal(double val) { val_ = val; }

  private:
    int num_;
    Type type_ { Type::Normal };
    double val_ {0.};

    friend bool operator<(const Variable& v1, const Variable& v2);

};

bool operator<(const Variable& v1, const Variable& v2)
{
  return std::tie(v1.type_, v1.num_) < std::tie(v2.type_, v2.num_);
}

template<typename CC, typename C, typename T>
class Simplex
{
  public:
    Simplex(const CC& a,
            const C& x,
            const C& b,
            bool isMinimization = false) :
      isMinimization_(isMinimization) {
      //static_assert(a.size() != x.size(), "a.size != x.size");
      if (a.size() != b.size())
        throw std::runtime_error("A.size() != B.size()");

      initSimplexMatrix(a, x, b);
    }

    std::vector<double> optimize() {
      while (!isOptimal() && iterations < 1000) {
        pivotSimplex();
        ++iterations;
      }

      return result();
    }

    double getVal() const { return val; }

  private:
    void initSimplexMatrix(const CC& a,
                           const C& x,
                           const C& b) {
      varNum = x.size();
      constraintNum = b.size();
      matrixWidth = varNum + constraintNum + 1;
      matrixHeight = constraintNum + 1;
      simplex_.clear();
      simplex_.reserve(matrixHeight);

      for (int i = 0; i < a.size(); i++) {
        std::vector<double> row(matrixWidth, 0.);

        std::copy(a[i].begin(), a[i].end(), row.begin());
        row[varNum + i] = 1.;
        row[matrixWidth - 1] = b[i];

        simplex_.push_back(std::move(row));
      }

      std::vector<double> lastRow(matrixWidth, 0.);
      std::copy(x.begin(), x.end(), lastRow.begin());
      std::transform(lastRow.begin(),
                     lastRow.end(),
                     lastRow.begin(),
                     std::bind1st(std::multiplies<double>(), -1));

      simplex_.push_back(lastRow);

      for (int i = 0; i < varNum; i++)
        bottom_.push_back(Variable(i, Variable::Type::Normal, simplex_.back()[i]));
      for (int i = 0; i < constraintNum; i++)
        bottom_.push_back(Variable(i, Variable::Type::Slack, simplex_.back()[varNum + i]));

      for (int i = 0; i < constraintNum; i++)
        right_.push_back(Variable(i, Variable::Type::Slack, simplex_[i].back()));
    }

    void pivotSimplex() {
      const std::vector<double>& lastRow = simplex_.back();
      int enteringI = std::distance(lastRow.begin(), std::min_element(lastRow.begin(), lastRow.end()));
      int departingI = 0;
      double pivot = -1;// simplex_[0][enteringI] / simplex_[0].back();

      //find pivot
      for (int i = 0; i < simplex_.size() - 1; i++) {
        double t = simplex_[i].back() / simplex_[i][enteringI];

        if ((t >= 0 && t < pivot) || pivot < 0) {
          departingI = i;
          pivot = t;
        }
      }

      double pivotVal = simplex_[departingI][enteringI];

      std::transform(simplex_[departingI].begin(),
                     simplex_[departingI].end(),
                     simplex_[departingI].begin(),
      [pivotVal](double val) {
        return val / pivotVal;
      });

      //improve solution
      for (int i = 0; i < simplex_.size(); i++) {
        if (i == departingI)
          continue;

        std::vector<double> row;

        for (int j = 0; j < simplex_[i].size(); j++) {
          double res = simplex_[i][j] - simplex_[departingI][j] * simplex_[i][enteringI];
          row.push_back(res);
        }

        std::swap(simplex_[i], row);
      }

      std::swap(bottom_[enteringI], right_[departingI]);

      for (int i = 0; i < simplex_.size(); i++)
        right_[i].setVal(simplex_[i].back());
      //bottom_[enteringI].setVal(simplex_.back()[enteringI]);

    }

    bool isOptimal() {
      auto min = std::min_element(simplex_.back().begin(), simplex_.back().end());
      return *min >= 0.;
    }

    void printSimplex() const {
      for (int i = 0; i < simplex_.size(); i++) {
        for (int j = 0; j < simplex_[i].size(); j++)
          std::cout << simplex_[i][j] << " ";

        std::cout << std::endl;
      }
    }

    std::vector<double> result() {
      std::vector<double> result;

      if (isMinimization_)
        result.insert(result.begin(), simplex_.back().begin() + varNum, simplex_.back().begin() + varNum + constraintNum);
      else {
        std::vector<Variable> variables;
        variables.insert(variables.end(), right_.begin(), right_.end());

        std::sort(variables.begin(), variables.end());

        for (int i = 0; i < variables.size(); i++)
          result.push_back(variables[i].val());
      }

      val = simplex_.back().back();

      return result;
    }

    std::vector<std::vector<double>> simplex_;
    bool isMinimization_ {false};
    std::vector<Variable> bottom_;
    std::vector<Variable> right_;

    int constraintNum;
    int varNum;
    int matrixWidth;
    int matrixHeight;
    int iterations {0};
    double val {0.};
};

#endif // SIMPLEX_H
