#ifndef XCODEMLTYPE_H
#define XCODEMLTYPE_H

namespace XcodeMl {

class Type;
using TypeRef = std::shared_ptr<Type>;

/* data type identifier (3.1 data type identifier) */
using DataTypeIdent = std::string;

using CodeFragment = CXXCodeGen::StringTreeRef;

class Environment;

enum class TypeKind {
 /*! basic data type (3.4 <basicType> element) */
  Reserved,
  /*! pointer (3.5 <pointerType> element) */
  Pointer,
  /*! function (3.6 <functionType> element) */
  Function,
  /*! C-style array (3.7 <ArrayType> element) */
  Array,
  /*! C-Style struct (3.xx <structType> element) */
  Struct,
  /*! C++-style class */
  Class,
};

TypeKind typeKind(TypeRef);
CodeFragment makeDecl(TypeRef, CodeFragment, const Environment&);

CodeFragment TypeRefToString(TypeRef, const Environment& env);

/*!
 * \brief A class that represents data types in XcodeML.
 */
class Type {
public:
  Type(TypeKind, DataTypeIdent, bool = false, bool = false);
  virtual ~Type() = 0;
  virtual Type* clone() const = 0;
  virtual CodeFragment makeDeclaration(CodeFragment, const Environment&) = 0;
  virtual CodeFragment addConstQualifier(CodeFragment) const;
  virtual CodeFragment addVolatileQualifier(CodeFragment) const;
  bool isConst() const;
  bool isVolatile() const;
  void setConst(bool);
  void setVolatile(bool);
  DataTypeIdent dataTypeIdent();
  TypeKind getKind() const;
protected:
  Type(const Type&);
private:
  TypeKind kind;
  DataTypeIdent ident;
  bool constness;
  bool volatility;
};

class Reserved : public Type {
public:
  Reserved(DataTypeIdent, CodeFragment);
  CodeFragment makeDeclaration(CodeFragment, const Environment&) override;
  ~Reserved() override;
  Type* clone() const override;
  static bool classof(const Type *);
protected:
  Reserved(const Reserved&);
private:
  CodeFragment name;
};

class Pointer : public Type {
public:
  Pointer(DataTypeIdent, TypeRef);
  Pointer(DataTypeIdent, DataTypeIdent);
  CodeFragment makeDeclaration(CodeFragment, const Environment&) override;
  ~Pointer() override;
  Type* clone() const override;
  static bool classof(const Type *);
protected:
  Pointer(const Pointer&);
private:
  DataTypeIdent ref;
};

class Function : public Type {
public:
  using Params = std::vector<std::tuple<DataTypeIdent, CodeFragment>>;
  Function(DataTypeIdent, TypeRef, const std::vector<DataTypeIdent>&);
  Function(DataTypeIdent, TypeRef, const Params&);
  CodeFragment makeDeclaration(CodeFragment, const Environment&) override;
  ~Function() override;
  Type* clone() const override;
  static bool classof(const Type *);
protected:
  Function(const Function&);
private:
  bool isParamListEmpty() const;

  DataTypeIdent returnValue;
  Params params;
};

class Array : public Type {
public:
  struct Size {
    enum class Kind {
      Integer,
      Variable,
      /* Expression, // FIXME: Unimplemented */
    };
    Kind kind;
    size_t size;
    static Size makeIntegerSize(size_t);
    static Size makeVariableSize();
  private:
    Size(Kind, size_t);
  };

public:
  Array(DataTypeIdent, DataTypeIdent, Size);
  Array(DataTypeIdent, DataTypeIdent, size_t);
  CodeFragment makeDeclaration(CodeFragment, const Environment&) override;
  ~Array() override;
  Type* clone() const override;
  CodeFragment addConstQualifier(CodeFragment) const override;
  CodeFragment addVolatileQualifier(CodeFragment) const override;
  static bool classof(const Type *);
protected:
  Array(const Array&);
private:
  DataTypeIdent element;
  Size size;
};

class Struct : public Type {
public:
  class BitSize { /* represents a size of bit-field */
  public:
    BitSize();
    BitSize(size_t);
    bool isValid() const;
    size_t size() const;
  private:
    bool valid;
    size_t size_;
  };

  class Member {
  public:
    Member() = delete;
    Member(const DataTypeIdent&, const CodeFragment&);
    Member(const DataTypeIdent&, const CodeFragment&, size_t);
    Member(const Member&) = default;
    ~Member() = default;
    DataTypeIdent type() const;
    CodeFragment name() const;
    bool isBitField() const;
    size_t getSize() const;
  private:
    DataTypeIdent dataTypeIdent;
    CodeFragment name_;
    BitSize size;
  };
  using MemberList = std::vector<Member>;

public:
  Struct(const DataTypeIdent&, const CodeFragment&, const MemberList&);
  CodeFragment makeDeclaration(CodeFragment, const Environment&) override;
  ~Struct() override;
  Type* clone() const override;
  void setTagName(const CodeFragment&);
  MemberList members() const;
  CodeFragment tagName() const;
  static bool classof(const Type *);
protected:
  Struct(const Struct&);
private:
  CodeFragment tag;
  MemberList fields;
};

enum class AccessSpec {
  Public,
  Private,
  Protected,
};

std::string string_of_accessSpec(AccessSpec);
AccessSpec accessSpec_of_string(const std::string&);

class ClassType : public Type {
public:
  ClassType(const DataTypeIdent&, const CodeFragment&, xmlNodePtr);
  CodeFragment makeDeclaration(CodeFragment, const Environment&) override;
  ~ClassType() override = default;
  Type* clone() const override;
  xmlNodePtr getNode() const;
  CodeFragment name() const;
  static bool classof(const Type *);
protected:
  ClassType(const ClassType&);
private:
  CodeFragment name_;
  xmlNodePtr declNode; // nullable
};

TypeRef makeReservedType(DataTypeIdent, CodeFragment, bool = false, bool = false);
TypeRef makePointerType(DataTypeIdent, TypeRef);
TypeRef makePointerType(DataTypeIdent, DataTypeIdent);
TypeRef makeFunctionType(DataTypeIdent, TypeRef, const Function::Params&);
TypeRef makeArrayType(DataTypeIdent, TypeRef, size_t);
TypeRef makeArrayType(DataTypeIdent, TypeRef, size_t);
TypeRef makeArrayType(DataTypeIdent, TypeRef, Array::Size);
TypeRef makeArrayType(DataTypeIdent, DataTypeIdent, Array::Size);
TypeRef makeArrayType(DataTypeIdent, DataTypeIdent, size_t);
TypeRef makeStructType(const DataTypeIdent&, const CodeFragment&, const Struct::MemberList&);

}
#endif /* !XCODEMLTYPE_H */
