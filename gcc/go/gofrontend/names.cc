// names.cc -- Names used by gofrontend generated code.

// Copyright 2017 The Go Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "go-system.h"

#include "gogo.h"
#include "go-encode-id.h"
#include "types.h"
#include "expressions.h"

// This file contains functions that generate names that appear in the
// assembly code.  This is not used for names that appear only in the
// debug info.

// Return the assembler name to use for an exported function, a
// method, or a function/method declaration.  This is not called if
// the function has been given an explicit name via a magic //extern
// or //go:linkname comment.  GO_NAME is the name that appears in the
// Go code.  PACKAGE is the package where the function is defined, and
// is NULL for the package being compiled.  For a method, RTYPE is
// the method's receiver type; for a function, RTYPE is NULL.

std::string
Gogo::function_asm_name(const std::string& go_name, const Package* package,
			const Type* rtype)
{
  std::string ret = (package == NULL
		     ? this->pkgpath_symbol()
		     : package->pkgpath_symbol());

  if (rtype != NULL
      && Gogo::is_hidden_name(go_name)
      && Gogo::hidden_name_pkgpath(go_name) != this->pkgpath())
    {
      // This is a method created for an unexported method of an
      // imported embedded type.  Use the pkgpath of the imported
      // package.
      std::string p = Gogo::hidden_name_pkgpath(go_name);
      ret = this->pkgpath_symbol_for_package(p);
    }

  ret.append(1, '.');
  ret.append(Gogo::unpack_hidden_name(go_name));

  if (rtype != NULL)
    {
      ret.append(1, '.');
      ret.append(rtype->mangled_name(this));
    }

  return go_encode_id(ret);
}

// Return the name to use for a function descriptor.  These symbols
// are globally visible.

std::string
Gogo::function_descriptor_name(Named_object* no)
{
  std::string var_name;
  if (no->is_function_declaration()
      && !no->func_declaration_value()->asm_name().empty()
      && Linemap::is_predeclared_location(no->location()))
    {
      if (no->func_declaration_value()->asm_name().substr(0, 8) != "runtime.")
	var_name = no->func_declaration_value()->asm_name() + "_descriptor";
      else
	var_name = no->func_declaration_value()->asm_name() + "$descriptor";
    }
  else
    {
      if (no->package() == NULL)
	var_name = this->pkgpath_symbol();
      else
	var_name = no->package()->pkgpath_symbol();
      var_name.push_back('.');
      var_name.append(Gogo::unpack_hidden_name(no->name()));
      var_name.append("$descriptor");
    }
  return var_name;
}

// Return the name to use for a generated stub method.  MNAME is the
// method name.  These functions are globally visible.  Note that this
// is the function name that corresponds to the name used for the
// method in Go source code, if this stub method were written in Go.
// The assembler name will be generated by Gogo::function_asm_name,
// and because this is a method that name will include the receiver
// type.

std::string
Gogo::stub_method_name(const std::string& mname)
{
  return mname + "$stub";
}

// Return the names of the hash and equality functions for TYPE.  If
// NAME is not NULL it is the name of the type.  Set *HASH_NAME and
// *EQUAL_NAME.

void
Gogo::specific_type_function_names(const Type* type, const Named_type* name,
				   std::string *hash_name,
				   std::string *equal_name)
{
  std::string base_name;
  if (name == NULL)
    {
      // Mangled names can have '.' if they happen to refer to named
      // types in some way.  That's fine if this is simply a named
      // type, but otherwise it will confuse the code that builds
      // function identifiers.  Remove '.' when necessary.
      base_name = type->mangled_name(this);
      size_t i;
      while ((i = base_name.find('.')) != std::string::npos)
	base_name[i] = '$';
      base_name = this->pack_hidden_name(base_name, false);
    }
  else
    {
      // This name is already hidden or not as appropriate.
      base_name = name->name();
      unsigned int index;
      const Named_object* in_function = name->in_function(&index);
      if (in_function != NULL)
	{
	  base_name.append(1, '$');
	  const Typed_identifier* rcvr =
	    in_function->func_value()->type()->receiver();
	  if (rcvr != NULL)
	    {
	      Named_type* rcvr_type = rcvr->type()->deref()->named_type();
	      base_name.append(Gogo::unpack_hidden_name(rcvr_type->name()));
	      base_name.append(1, '$');
	    }
	  base_name.append(Gogo::unpack_hidden_name(in_function->name()));
	  if (index > 0)
	    {
	      char buf[30];
	      snprintf(buf, sizeof buf, "%u", index);
	      base_name += '$';
	      base_name += buf;
	    }
	}
    }
  *hash_name = base_name + "$hash";
  *equal_name = base_name + "$equal";
}

// Return the assembler name to use for a global variable.  GO_NAME is
// the name that appears in the Go code.  PACKAGE is the package where
// the variable is defined, and is NULL for the package being
// compiled.

std::string
Gogo::global_var_asm_name(const std::string& go_name, const Package* package)
{
  std::string ret = (package != NULL
		     ? package->pkgpath_symbol()
		     : this->pkgpath_symbol());
  ret.push_back('.');
  ret.append(Gogo::unpack_hidden_name(go_name));
  return go_encode_id(ret);
}

// Return an erroneous name that indicates that an error has already
// been reported.

std::string
Gogo::erroneous_name()
{
  static int erroneous_count;
  char name[50];
  snprintf(name, sizeof name, "$erroneous%d", erroneous_count);
  ++erroneous_count;
  return name;
}

// Return whether a name is an erroneous name.

bool
Gogo::is_erroneous_name(const std::string& name)
{
  return name.compare(0, 10, "$erroneous") == 0;
}

// Return a name for a thunk object.

std::string
Gogo::thunk_name()
{
  static int thunk_count;
  char thunk_name[50];
  snprintf(thunk_name, sizeof thunk_name, "$thunk%d", thunk_count);
  ++thunk_count;
  return thunk_name;
}

// Return whether a function is a thunk.

bool
Gogo::is_thunk(const Named_object* no)
{
  return no->name().compare(0, 6, "$thunk") == 0;
}

// Return the name to use for an init function.  There can be multiple
// functions named "init" so each one needs a different name.

std::string
Gogo::init_function_name()
{
  static int init_count;
  char buf[30];
  snprintf(buf, sizeof buf, ".$init%d", init_count);
  ++init_count;
  return buf;
}

// Return the name to use for a nested function.

std::string
Gogo::nested_function_name()
{
  static int nested_count;
  char buf[30];
  snprintf(buf, sizeof buf, ".$nested%d", nested_count);
  ++nested_count;
  return buf;
}

// Return the name to use for a sink function, a function whose name
// is simply underscore.  We don't really need these functions but we
// do have to generate them for error checking.

std::string
Gogo::sink_function_name()
{
  static int sink_count;
  char buf[30];
  snprintf(buf, sizeof buf, ".$sink%d", sink_count);
  ++sink_count;
  return buf;
}

// Return the name to use for a redefined function.  These functions
// are erroneous but we still generate them for further error
// checking.

std::string
Gogo::redefined_function_name()
{
  static int redefinition_count;
  char buf[30];
  snprintf(buf, sizeof buf, ".$redefined%d", redefinition_count);
  ++redefinition_count;
  return buf;
}

// Return the name to use for a recover thunk for the function NAME.
// If the function is a method, RTYPE is the receiver type.

std::string
Gogo::recover_thunk_name(const std::string& name, const Type* rtype)
{
  std::string ret(name);
  if (rtype != NULL)
    {
      ret.push_back('$');
      ret.append(rtype->mangled_name(this));
    }
  ret.append("$recover");
  return ret;
}

// Return the name to use for a GC root variable.  The GC root
// variable is a composite literal that is passed to
// runtime.registerGCRoots.  There is at most one of these variables
// per compilation.

std::string
Gogo::gc_root_name()
{
  return "gc0";
}

// Return the name to use for a composite literal or string
// initializer.  This is a local name never referenced outside of this
// file.

std::string
Gogo::initializer_name()
{
  static unsigned int counter;
  char buf[30];
  snprintf(buf, sizeof buf, "C%u", counter);
  ++counter;
  return buf;
}

// Return the name of the variable used to represent the zero value of
// a map.  This is a globally visible common symbol.

std::string
Gogo::map_zero_value_name()
{
  return "go$zerovalue";
}

// Return the name to use for the import control function.

const std::string&
Gogo::get_init_fn_name()
{
  if (this->init_fn_name_.empty())
    {
      go_assert(this->package_ != NULL);
      if (this->is_main_package())
	{
	  // Use a name that the runtime knows.
	  this->init_fn_name_ = "__go_init_main";
	}
      else
	{
	  std::string s = this->pkgpath_symbol();
	  s.append("..import");
	  this->init_fn_name_ = s;
	}
    }

  return this->init_fn_name_;
}

// Return a mangled name for a type.  These names appear in symbol
// names in the assembler file for things like type descriptors and
// methods.

std::string
Type::mangled_name(Gogo* gogo) const
{
  std::string ret;

  // The do_mangled_name virtual function should set RET to the
  // mangled name.  For a composite type it should append a code for
  // the composition and then call do_mangled_name on the components.
  this->do_mangled_name(gogo, &ret);

  return ret;
}

// The mangled name is implemented as a method on each instance of
// Type.

void
Error_type::do_mangled_name(Gogo*, std::string* ret) const
{
  ret->push_back('E');
}

void
Void_type::do_mangled_name(Gogo*, std::string* ret) const
{
  ret->push_back('v');
}

void
Boolean_type::do_mangled_name(Gogo*, std::string* ret) const
{
  ret->push_back('b');
}

void
Integer_type::do_mangled_name(Gogo*, std::string* ret) const
{
  char buf[100];
  snprintf(buf, sizeof buf, "i%s%s%de",
	   this->is_abstract_ ? "a" : "",
	   this->is_unsigned_ ? "u" : "",
	   this->bits_);
  ret->append(buf);
}

void
Float_type::do_mangled_name(Gogo*, std::string* ret) const
{
  char buf[100];
  snprintf(buf, sizeof buf, "f%s%de",
	   this->is_abstract_ ? "a" : "",
	   this->bits_);
  ret->append(buf);
}

void
Complex_type::do_mangled_name(Gogo*, std::string* ret) const
{
  char buf[100];
  snprintf(buf, sizeof buf, "c%s%de",
	   this->is_abstract_ ? "a" : "",
	   this->bits_);
  ret->append(buf);
}

void
String_type::do_mangled_name(Gogo*, std::string* ret) const
{
  ret->push_back('z');
}

void
Function_type::do_mangled_name(Gogo* gogo, std::string* ret) const
{
  ret->push_back('F');

  if (this->receiver_ != NULL)
    {
      ret->push_back('m');
      this->append_mangled_name(this->receiver_->type(), gogo, ret);
    }

  const Typed_identifier_list* params = this->parameters();
  if (params != NULL)
    {
      ret->push_back('p');
      for (Typed_identifier_list::const_iterator p = params->begin();
	   p != params->end();
	   ++p)
	this->append_mangled_name(p->type(), gogo, ret);
      if (this->is_varargs_)
	ret->push_back('V');
      ret->push_back('e');
    }

  const Typed_identifier_list* results = this->results();
  if (results != NULL)
    {
      ret->push_back('r');
      for (Typed_identifier_list::const_iterator p = results->begin();
	   p != results->end();
	   ++p)
	this->append_mangled_name(p->type(), gogo, ret);
      ret->push_back('e');
    }

  ret->push_back('e');
}

void
Pointer_type::do_mangled_name(Gogo* gogo, std::string* ret) const
{
  ret->push_back('p');
  this->append_mangled_name(this->to_type_, gogo, ret);
}

void
Nil_type::do_mangled_name(Gogo*, std::string* ret) const
{
  ret->push_back('n');
}

void
Struct_type::do_mangled_name(Gogo* gogo, std::string* ret) const
{
  ret->push_back('S');

  const Struct_field_list* fields = this->fields_;
  if (fields != NULL)
    {
      for (Struct_field_list::const_iterator p = fields->begin();
	   p != fields->end();
	   ++p)
	{
	  if (p->is_anonymous())
	    ret->append("0_");
	  else
            {

              std::string n(Gogo::mangle_possibly_hidden_name(p->field_name()));
	      char buf[20];
	      snprintf(buf, sizeof buf, "%u_",
		       static_cast<unsigned int>(n.length()));
	      ret->append(buf);
	      ret->append(n);
	    }

	  // For an anonymous field with an alias type, the field name
	  // is the alias name.
	  if (p->is_anonymous()
	      && p->type()->named_type() != NULL
	      && p->type()->named_type()->is_alias())
	    p->type()->named_type()->append_mangled_type_name(gogo, true, ret);
	  else
	    this->append_mangled_name(p->type(), gogo, ret);
	  if (p->has_tag())
	    {
	      const std::string& tag(p->tag());
	      std::string out;
	      for (std::string::const_iterator p = tag.begin();
		   p != tag.end();
		   ++p)
		{
		  if (ISALNUM(*p) || *p == '_')
		    out.push_back(*p);
		  else
		    {
		      char buf[20];
		      snprintf(buf, sizeof buf, ".%x.",
			       static_cast<unsigned int>(*p));
		      out.append(buf);
		    }
		}
	      char buf[20];
	      snprintf(buf, sizeof buf, "T%u_",
		       static_cast<unsigned int>(out.length()));
	      ret->append(buf);
	      ret->append(out);
	    }
	}
    }

  if (this->is_struct_incomparable_)
    ret->push_back('x');

  ret->push_back('e');
}

void
Array_type::do_mangled_name(Gogo* gogo, std::string* ret) const
{
  ret->push_back('A');
  this->append_mangled_name(this->element_type_, gogo, ret);
  if (this->length_ != NULL)
    {
      Numeric_constant nc;
      if (!this->length_->numeric_constant_value(&nc))
	{
	  go_assert(saw_errors());
	  return;
	}
      mpz_t val;
      if (!nc.to_int(&val))
	{
	  go_assert(saw_errors());
	  return;
	}
      char *s = mpz_get_str(NULL, 10, val);
      ret->append(s);
      free(s);
      mpz_clear(val);
      if (this->is_array_incomparable_)
	ret->push_back('x');
    }
  ret->push_back('e');
}

void
Map_type::do_mangled_name(Gogo* gogo, std::string* ret) const
{
  ret->push_back('M');
  this->append_mangled_name(this->key_type_, gogo, ret);
  ret->append("__");
  this->append_mangled_name(this->val_type_, gogo, ret);
}

void
Channel_type::do_mangled_name(Gogo* gogo, std::string* ret) const
{
  ret->push_back('C');
  this->append_mangled_name(this->element_type_, gogo, ret);
  if (this->may_send_)
    ret->push_back('s');
  if (this->may_receive_)
    ret->push_back('r');
  ret->push_back('e');
}

void
Interface_type::do_mangled_name(Gogo* gogo, std::string* ret) const
{
  go_assert(this->methods_are_finalized_);

  ret->push_back('I');

  const Typed_identifier_list* methods = this->all_methods_;
  if (methods != NULL && !this->seen_)
    {
      this->seen_ = true;
      for (Typed_identifier_list::const_iterator p = methods->begin();
	   p != methods->end();
	   ++p)
	{
	  if (!p->name().empty())
	    {
	      std::string n(Gogo::mangle_possibly_hidden_name(p->name()));
	      char buf[20];
	      snprintf(buf, sizeof buf, "%u_",
		       static_cast<unsigned int>(n.length()));
	      ret->append(buf);
	      ret->append(n);
	    }
	  this->append_mangled_name(p->type(), gogo, ret);
	}
      this->seen_ = false;
    }

  ret->push_back('e');
}

void
Named_type::do_mangled_name(Gogo* gogo, std::string* ret) const
{
  this->append_mangled_type_name(gogo, false, ret);
}

void
Forward_declaration_type::do_mangled_name(Gogo* gogo, std::string* ret) const
{
  if (this->is_defined())
    this->append_mangled_name(this->real_type(), gogo, ret);
  else
    {
      const Named_object* no = this->named_object();
      std::string name;
      if (no->package() == NULL)
	name = gogo->pkgpath_symbol();
      else
	name = no->package()->pkgpath_symbol();
      name += '.';
      name += Gogo::unpack_hidden_name(no->name());
      char buf[20];
      snprintf(buf, sizeof buf, "N%u_",
	       static_cast<unsigned int>(name.length()));
      ret->append(buf);
      ret->append(name);
    }
}

// Append the mangled name for a named type to RET.  For an alias we
// normally use the real name, but if USE_ALIAS is true we use the
// alias name itself.

void
Named_type::append_mangled_type_name(Gogo* gogo, bool use_alias,
				     std::string* ret) const
{
  if (this->is_error_)
    return;
  if (this->is_alias_ && !use_alias)
    {
      if (this->seen_alias_)
	return;
      this->seen_alias_ = true;
      this->append_mangled_name(this->type_, gogo, ret);
      this->seen_alias_ = false;
      return;
    }
  Named_object* no = this->named_object_;
  std::string name;
  if (this->is_builtin())
    go_assert(this->in_function_ == NULL);
  else
    {
      const std::string& pkgpath(no->package() == NULL
				 ? gogo->pkgpath_symbol()
				 : no->package()->pkgpath_symbol());
      name = pkgpath;
      name.append(1, '.');
      if (this->in_function_ != NULL)
	{
	  const Typed_identifier* rcvr =
	    this->in_function_->func_value()->type()->receiver();
	  if (rcvr != NULL)
	    {
	      Named_type* rcvr_type = rcvr->type()->deref()->named_type();
	      name.append(Gogo::unpack_hidden_name(rcvr_type->name()));
	      name.append(1, '.');
	    }
	  name.append(Gogo::unpack_hidden_name(this->in_function_->name()));
	  name.append(1, '$');
	  if (this->in_function_index_ > 0)
	    {
	      char buf[30];
	      snprintf(buf, sizeof buf, "%u", this->in_function_index_);
	      name.append(buf);
	      name.append(1, '$');
	    }
	}
    }
  name.append(Gogo::unpack_hidden_name(no->name()));
  char buf[20];
  snprintf(buf, sizeof buf, "N%u_", static_cast<unsigned int>(name.length()));
  ret->append(buf);
  ret->append(name);
}

// Return the name for the type descriptor symbol for TYPE.  This can
// be a global, common, or local symbol, depending.  NT is not NULL if
// it is the name to use.

std::string
Gogo::type_descriptor_name(Type* type, Named_type* nt)
{
  // The type descriptor symbol for the unsafe.Pointer type is defined
  // in libgo/runtime/go-unsafe-pointer.c, so just use a reference to
  // that symbol.
  if (type->is_unsafe_pointer_type())
    return "__go_tdn_unsafe.Pointer";

  if (nt == NULL)
    return "__go_td_" + type->mangled_name(this);

  Named_object* no = nt->named_object();
  unsigned int index;
  const Named_object* in_function = nt->in_function(&index);
  std::string ret = "__go_tdn_";
  if (nt->is_builtin())
    go_assert(in_function == NULL);
  else
    {
      const std::string& pkgpath(no->package() == NULL
				 ? this->pkgpath_symbol()
				 : no->package()->pkgpath_symbol());
      ret.append(pkgpath);
      ret.append(1, '.');
      if (in_function != NULL)
	{
	  const Typed_identifier* rcvr =
	    in_function->func_value()->type()->receiver();
	  if (rcvr != NULL)
	    {
	      Named_type* rcvr_type = rcvr->type()->deref()->named_type();
	      ret.append(Gogo::unpack_hidden_name(rcvr_type->name()));
	      ret.append(1, '.');
	    }
	  ret.append(Gogo::unpack_hidden_name(in_function->name()));
	  ret.append(1, '.');
	  if (index > 0)
	    {
	      char buf[30];
	      snprintf(buf, sizeof buf, "%u", index);
	      ret.append(buf);
	      ret.append(1, '.');
	    }
	}
    }

  std::string mname(Gogo::mangle_possibly_hidden_name(no->name()));
  ret.append(mname);

  return ret;
}

// Return the name for the GC symbol for a type.  This is used to
// initialize the gcdata field of a type descriptor.  This is a local
// name never referenced outside of this assembly file.  (Note that
// some type descriptors will initialize the gcdata field with a name
// generated by ptrmask_symbol_name rather than this method.)

std::string
Gogo::gc_symbol_name(Type* type)
{
  return this->type_descriptor_name(type, type->named_type()) + "$gc";
}

// Return the name for a ptrmask variable.  PTRMASK_SYM_NAME is a
// base64 string encoding the ptrmask (as returned by Ptrmask::symname
// in types.cc).  This name is used to intialize the gcdata field of a
// type descriptor.  These names are globally visible.  (Note that
// some type descriptors will initialize the gcdata field with a name
// generated by gc_symbol_name rather than this method.)

std::string
Gogo::ptrmask_symbol_name(const std::string& ptrmask_sym_name)
{
  return "runtime.gcbits." + ptrmask_sym_name;
}

// Return the name to use for an interface method table used for the
// ordinary type TYPE converted to the interface type ITYPE.
// IS_POINTER is true if this is for the method set for a pointer
// receiver.

std::string
Gogo::interface_method_table_name(Interface_type* itype, Type* type,
				  bool is_pointer)
{
  return ((is_pointer ? "__go_pimt__" : "__go_imt_")
	  + itype->mangled_name(this)
	  + "__"
	  + type->mangled_name(this));
}
