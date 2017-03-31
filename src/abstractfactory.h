#ifndef ABSTRACTFACTORY_H
#define ABSTRACTFACTORY_H

#include <map>
#include <memory>
#include <tuple>

#include <functional>

namespace {

template<int ...S> struct seq {};
template<int N, int ...S> struct gens : gens < N - 1, N - 1, S... > {};
template<int ...S> struct gens<0, S...> { typedef seq<S...> type; };

template<typename T>
class Creator
{
  public:
    virtual ~Creator() {}
    virtual T* create() const = 0;
};

template<typename N, typename T, typename... Args>
class TCreator : public Creator<T>
{
  public:
    TCreator(Args... args) : args_(args...) {}

    T* create() const override {
      return create(typename gens<sizeof...(Args)>::type(), args_);
    }

    template<typename... FArgs>
    T* create(FArgs... args) const {
      return new N(args...);
    }

  protected:
    template<int ...S>
    T* create(seq<S...>, std::tuple<Args...> args) const {
      return create(std::get<S>(args)...);
    }

    std::tuple<Args...> args_;
};

template<typename N, typename T>
class TCreator<N, T> : public Creator<T>
{
  public:
    T* create() const override {
      return new N;
    }
};

}

template<typename T, typename Key = int, template<typename...> class C = std::map>
class Factory
{
  public:
    template<typename N, typename... Args>
    void Register(Key id, Args... args) {
      map_[id] = std::unique_ptr<Creator<T>>(new TCreator<N, T, Args...>(args...));
    }

    template<template <typename...> class Ptr>
    Ptr<T> create(Key id) const { return Ptr<T>(create(id)); }

    T* create(Key id) const { return map_.at(id)->create(); }

  private:
    C<Key, std::unique_ptr<Creator<T>>> map_;
};


#endif // ABSTRACTFACTORY_H
