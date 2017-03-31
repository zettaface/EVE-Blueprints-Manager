#ifndef REQUEST_H
#define REQUEST_H

#include <QString>

namespace eve {

class Request
{
  public:
    Request() = default;
    virtual ~Request() = default;

    virtual QString path() const { return ""; }
    virtual QString query() const { return ""; }
    virtual QString url() const { return ""; }
    virtual void parse(QByteArray& data) = 0;

    int priority() const { return priority_; }
    void setPriority(int priority) { priority_ = priority; }

  protected:
    int priority_ {0};
};

} //namespace eve

#endif // REQUEST_H
