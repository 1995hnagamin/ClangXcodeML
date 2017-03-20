#include <map>
#include <memory>
#include <string>
#include <vector>
#include "llvm/ADT/Optional.h"
#include "llvm/Support/Casting.h"
#include "StringTree.h"
#include "XcodeMlType.h"
#include "XcodeMlEnvironment.h"

#include "XcodeMlNns.h"

using CXXCodeGen::makeTokenNode;

namespace XcodeMl {

Nns::Nns(NnsKind k, const NnsRef& nr, const NnsIdent& ni):
  parent(nr->getParent()),
  kind(k),
  ident(ni)
{}

Nns::Nns(NnsKind k, const NnsIdent& par, const NnsIdent& ident):
  parent(par),
  kind(k),
  ident(ident)
{}

Nns::~Nns() = default;

NnsKind
Nns::getKind() const {
  return kind;
}

CodeFragment
Nns::makeDeclaration(
    const Environment& env,
    const NnsMap& nnss) const
{
  const auto par = getParent();
  if (par.hasValue()) {
    const auto p = nnss.at(*par);
    const auto prefix = p->makeDeclaration(
        env,
        nnss);
    return prefix + makeNestedNameSpec(env, nnss);
  } else {
    return makeNestedNameSpec(env, nnss);
  }
}

llvm::Optional<NnsIdent>
Nns::getParent() const {
  return parent;
}

GlobalNns::GlobalNns():
  Nns(NnsKind::Global, nullptr, "global")
{}

Nns*
GlobalNns::clone() const {
  GlobalNns *copy = new GlobalNns(*this);
  return copy;
}

bool
GlobalNns::classof(const Nns *N) {
  return N->getKind() == NnsKind::Global;
}

CodeFragment
GlobalNns::makeNestedNameSpec(
    const Environment&,
    const NnsMap&) const
{
  return makeTokenNode("::");
}

llvm::Optional<NnsIdent>
GlobalNns::getParent() const {
  return llvm::Optional<NnsIdent>();
}

ClassNns::ClassNns(
    const NnsIdent& ni,
    const NnsRef& parent,
    const DataTypeIdent& di):
  Nns(NnsKind::Class, parent, ni),
  dtident(di)
{}

Nns*
ClassNns::clone() const {
  ClassNns *copy = new ClassNns(*this);
  return copy;
}

CodeFragment
ClassNns::makeNestedNameSpec(
    const Environment& env,
    const NnsMap&) const
{
  const auto T = env.at(dtident);
  const auto classT = llvm::cast<XcodeMl::ClassType>(T.get());
  assert(classT);
  const auto name = classT->name();
  assert(name.hasValue());
  return (*name) + makeTokenNode("::");
}

bool
ClassNns::classof(const Nns* N) {
  return N->getKind() == NnsKind::Class;
}

} // namespace XcodeMl
