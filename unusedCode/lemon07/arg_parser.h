/* -*- C++ -*-
 *
 * This file is a part of LEMON, a generic C++ optimization library
 *
 * Copyright (C) 2003-2008
 * Egervary Jeno Kombinatorikus Optimalizalasi Kutatocsoport
 * (Egervary Research Group on Combinatorial Optimization, EGRES).
 *
 * Permission to use, modify and distribute this software is granted
 * provided that this copyright notice appears in all copies. For
 * precise terms see the accompanying LICENSE file.
 *
 * This software is provided "AS IS" with no warranty of any kind,
 * express or implied, and with no claim as to its suitability for any
 * purpose.
 *
 */

#ifndef LEMON_ARG_PARSER
#define LEMON_ARG_PARSER

#include <vector>
#include <map>
#include <list>
#include <string>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <lemon/error.h>

///\ingroup misc
///\file
///\brief A tools to parse command line arguments.
///
///\author Alpar Juttner

namespace lemon {

  ///Command line arguments parser

  ///\ingroup misc
  ///Command line arguments parser
  ///
  class ArgParser {
    
    static void _showHelp(void *p);
  protected:
    
    int _argc;
    const char **_argv;
    
    enum OptType { UNKNOWN=0, BOOL=1, STRING=2, DOUBLE=3, INTEGER=4, FUNC=5 };
    
    class ParData {
    public:
      union {
	bool *bool_p;
	int *int_p;
	double *double_p;
	std::string *string_p;
	struct {
	  void (*p)(void *);
	  void *data;
	} func_p;
	  
      };
      std::string help;
      bool mandatory;
      OptType type;
      bool set;
      bool ingroup;
      bool has_syn;
      bool syn;
      bool self_delete;
      ParData() : mandatory(false), type(UNKNOWN), set(false), ingroup(false),
		  has_syn(false), syn(false), self_delete(false) {}
    };

    typedef std::map<std::string,ParData> Opts;
    Opts _opts;

    class GroupData 
    {
    public:
      typedef std::list<std::string> Opts;
      Opts opts;
      bool only_one;
      bool mandatory;
      GroupData() :only_one(false), mandatory(false) {}
    };
      
    typedef std::map<std::string,GroupData> Groups;
    Groups _groups;

    struct OtherArg
    {
      std::string name;
      std::string help;
      OtherArg(std::string n, std::string h) :name(n), help(h) {}

    };
      
    std::vector<OtherArg> _others_help;
    std::vector<std::string> _file_args;
    std::string _command_name;
    
  public:

    ///\e
    ArgParser(int argc, const char **argv);

    ~ArgParser();

    ///Add a new integer type option

    ///\param name The name of the option. The leading '-' must be omitted.
    ///\param help A help string.
    ///\retval value The value of the argument will be written to this variable.
    ///\param obl Indicate if the option is mandatory.
    ArgParser &intOption(const std::string &name,
		    const std::string &help,
		    int value=0, bool obl=false);

    ///Add a new floating type option

    ///\param name The name of the option. The leading '-' must be omitted.
    ///\param help A help string.
    ///\retval value The value of the argument will be written to this variable.
    ///\param obl Indicate if the option is mandatory.
    ArgParser &doubleOption(const std::string &name,
		      const std::string &help,
		      double value=0, bool obl=false);

    ///Add a new bool type option

    ///\param name The name of the option. The leading '-' must be omitted.
    ///\param help A help string.
    ///\retval value The value of the argument will be written to this variable.
    ///\param obl Indicate if the option is mandatory.
    ////\note A mandatory bool obtion is of very little use.)
    ArgParser &boolOption(const std::string &name,
		      const std::string &help,
		      bool value=false, bool obl=false);

    ///Add a new string type option

    ///\param name The name of the option. The leading '-' must be omitted.
    ///\param help A help string.
    ///\retval value The value of the argument will be written to this variable.
    ///\param obl Indicate if the option is mandatory.
    ArgParser &stringOption(const std::string &name,
		      const std::string &help,
		      std::string value="", bool obl=false);
    
    ///Bind a function to an option.

    ///\param name The name of the option. The leading '-' must be omitted.
    ///\param help A help string.
    ///\retval func The function to be called when the option is given. It
    ///  must be of type "void f(void *)"
    ///\param data Data to be passed to \c func
    ArgParser &funcOption(const std::string &name,
		    const std::string &help,
		    void (*func)(void *),void *data);

    ///\name Options with an external strorage.
    ///Using this functions, the value of the option will be directly written
    ///into a variable once the option appears in the command line.

    ///@{

    ///Add a new integer type option with a storage reference

    ///\param name The name of the option. The leading '-' must be omitted.
    ///\param help A help string.
    ///\retval ref The value of the argument will be written to this variable.
    ///\param obl Indicate if the option is mandatory.
    ArgParser &refOption(const std::string &name,
		    const std::string &help,
		    int &ref, bool obl=false);

    ///Add a new floating type option with a storage reference

    ///\param name The name of the option. The leading '-' must be omitted.
    ///\param help A help string.
    ///\retval ref The value of the argument will be written to this variable.
    ///\param obl Indicate if the option is mandatory.
    ArgParser &refOption(const std::string &name,
		      const std::string &help,
		      double &ref, bool obl=false);

    ///Add a new bool type option with a storage reference

    ///\param name The name of the option. The leading '-' must be omitted.
    ///\param help A help string.
    ///\retval ref The value of the argument will be written to this variable.
    ///\param obl Indicate if the option is mandatory.
    ////\note A mandatory bool obtion is of very little use.)
    ArgParser &refOption(const std::string &name,
		      const std::string &help,
		      bool &ref, bool obl=false);

    ///Add a new string type option with a storage reference

    ///\param name The name of the option. The leading '-' must be omitted.
    ///\param help A help string.
    ///\retval ref The value of the argument will be written to this variable.
    ///\param obl Indicate if the option is mandatory.
    ArgParser &refOption(const std::string &name,
		      const std::string &help,
		      std::string &ref, bool obl=false);
    
    ///@}

    ///\name Option Groups and Synonyms
    ///
    
    ///@{

    ///Boundle some options into a group

    /// You can group some option by calling this function repeatedly for each
    /// option to be grupped with the same groupname.
    ///\param group The group name
    ///\param opt The option name
    ArgParser &optionGroup(const std::string &group,
			   const std::string &opt);

    ///Make the members of a group exclusive

    ///If you call this function for a group, than at most one of them can be
    ///given at the same time
    ArgParser &onlyOneGroup(const std::string &group);
  
    ///Make a group mandatory

    ///Using this function, at least one of the members of \c group
    ///must be given.
    ArgParser &mandatoryGroup(const std::string &group);
    
    ///Create synonym to an option

    ///With this function you can create a sysnonym called \c sys of the
    ///option \c opt.
    ArgParser &synonym(const std::string &syn,
			   const std::string &opt);
    
    ///@}

    ///Give help string for non-parsed arguments.

    ///With this function you can give help string for non-parsed arguments.
    ///the parameter \c name will be printed in the short usage line, while
    ///\c help gives a more detailed description.
    ArgParser &other(const std::string &name,
		     const std::string &help="");
    
    ///Non option type arguments.

    ///Gives back a reference to a vector consisting of the program arguments
    ///not starting with a '-' character.
    std::vector<std::string> &files() { return _file_args; }

    ///Give back the command name (the 0th argument)
    const std::string &commandName() { return _command_name; }

    void show(std::ostream &os,Opts::iterator i);
    void show(std::ostream &os,Groups::iterator i);
    void showHelp(Opts::iterator i);
    void showHelp(std::vector<OtherArg>::iterator i);
    void shortHelp();
    void showHelp();

    void unknownOpt(std::string arg);

    void requiresValue(std::string arg, OptType t);
    void checkMandatories();
    
    ///Start the parsing process
    ArgParser &parse();

    /// Synonym for parse()
    ArgParser &run() 
    {
      return parse();
    }
    
    ///Check if an opion has been given to the command.
    bool given(std::string op) 
    {
      Opts::iterator i = _opts.find(op);
      return i!=_opts.end()?i->second.set:false;
    }


    ///Magic type for operator[]
    
    ///This is the type of the return value of ArgParser::operator[]().
    ///It automatically converts to int, double, bool or std::string, if it
    ///match the type of the option, otherwise it throws an exception.
    ///(i.e. it performs runtime type checking).
    class RefType 
    {
      ArgParser &_parser;
      std::string _name;
    public:
      ///\e
      RefType(ArgParser &p,const std::string &n) :_parser(p),_name(n) {}
      ///\e
      operator bool() 
      {
	Opts::iterator i = _parser._opts.find(_name);
	LEMON_ASSERT(i==_parser._opts.end(),
		     std::string()+"Unkown option: '"+_name+"'");
	LEMON_ASSERT(i->second.type!=ArgParser::BOOL,
		     std::string()+"'"+_name+"' is a bool option");
	return *(i->second.bool_p);
      }
      ///\e
      operator std::string()
      {
	Opts::iterator i = _parser._opts.find(_name);
	LEMON_ASSERT(i==_parser._opts.end(),
		     std::string()+"Unkown option: '"+_name+"'");
	LEMON_ASSERT(i->second.type!=ArgParser::STRING,
		     std::string()+"'"+_name+"' is a string option");
	return *(i->second.string_p);
      }
      ///\e
      operator double() 
      {
	Opts::iterator i = _parser._opts.find(_name);
	LEMON_ASSERT(i==_parser._opts.end(),
		     std::string()+"Unkown option: '"+_name+"'");
	LEMON_ASSERT(i->second.type!=ArgParser::DOUBLE &&
		     i->second.type!=ArgParser::INTEGER,
		     std::string()+"'"+_name+"' is a floating point option");
	return i->second.type==ArgParser::DOUBLE ?
	  *(i->second.double_p) : *(i->second.int_p);
      }
      ///\e
      operator int() 
      {
	Opts::iterator i = _parser._opts.find(_name);
	LEMON_ASSERT(i==_parser._opts.end(),
		     std::string()+"Unkown option: '"+_name+"'");
	LEMON_ASSERT(i->second.type!=ArgParser::INTEGER,
		     std::string()+"'"+_name+"' is an integer option");
	return *(i->second.int_p);
      }

    };

    ///Give back the value of an option
    
    ///Give back the value of an option
    ///\sa RefType
    RefType operator[](const std::string &n)
    {
      return RefType(*this, n);
    }    
 
  };
}

    

#endif // LEMON_MAIN_PARAMS
