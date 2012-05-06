/*
 ****************************************************************
 *                           nanocalc                           *
 *               Lightweight Scientific Calculator              *
 *                                                              *
 * Author: Arash Partow (2001)                                  *
 * URL: http://www.partow.net/programming/nanocalc/index.html   *
 *                                                              *
 * Copyright notice:                                            *
 * Free use of the nanocalc scientific calculator is permitted  *
 * under the guidelines and in accordance with the most current *
 * version of the Common Public License.                        *
 * http://www.opensource.org/licenses/cpl1.0.php                *
 *                                                              *
 * Example expressions:                                         *
 * (00) (y+x/y)*(x-y/x)                                         *
 * (01) (x^2/sin(2*pi/y))-x/2                                   *
 * (02) sqrt(1-(x^2))                                           *
 * (03) 1-sin(2*x)+cos(pi/y)                                    *
 * (04) a*exp(2*t)+c                                            *
 * (05) if(((x+2)==3)and((y+5)<=9),1+w,2/z)                     *
 * (06) if(avg(x,y)<=x+y,x-y,x*y)+2*pi/x                        *
 * (07) z:=x+sin(2*pi/y)                                        *
 * (08) u<-2*(pi*z)/(w:=x+cos(y/pi))                            *
 * (09) clamp(-1,sin(2*pi*x)+cos(y/2*pi),+1)                    *
 * (10) inrange(-2,m,+2)==if(({-2<=m}and[m<=+2]),1,0)           *
 * (11) (12.34sin(x)cos(2y)7+1)==(12.34*sin(x)*cos(2*y)*7+1)    *
 *                                                              *
 ****************************************************************
*/


#include <iostream>
#include <string>
#include <vector>

#define strtk_no_tr1_or_boost

#include "strtk.hpp"
#include "exprtk.hpp"

template<typename T>
class nanocalc
{
public:

   nanocalc()
   : display_precision_(15)
   {
      symbol_table_.add_constants();
      expression_.register_symbol_table(symbol_table_);
   }

   void run()
   {
      std::cout << "nanocalc - lightweight scientific calculator.\n";
      std::cout << "Type 'help' for a list of commands.\n";
      std::cout.precision(display_precision_);
      for (;;)
      {
         std::string command;

         std::cout << "> ";
         std::getline(std::cin,command);
         if (command.empty())
            continue;

         if (cmd_history_.empty() || !strtk::imatch(cmd_history_.back(),command))
         {
            cmd_history_.push_back(command);
         }

         if (0 == strtk::ifind("def",command))
         {
            process_symbol_definition(command);
            continue;
         }
         else if (strtk::imatch("precision",command))
         {
            std::cout << "Precision: " << display_precision_ << "\n";
            continue;
         }
         else if (0 == strtk::ifind("precision",command))
         {
            process_precision_command(command);
            continue;
         }
         else if (strtk::imatch("ls",command))
         {
            std::vector<std::pair<std::string,T> > vlist;
            symbol_table_.get_variable_list(vlist);
            if (vlist.empty())
            {
               std::cout << "No variables defined.\n";
               continue;
            }
            std::cout <<"Variable\tValue\n";
            std::cout <<"--------\t-----\n";
            for (std::size_t i = 0; i < vlist.size(); ++i)
            {
               std::cout << vlist[i].first << "\t\t" << vlist[i].second << "\n";
            }
         }
         else if (strtk::imatch("history",command))
         {
            if (cmd_history_.empty())
               continue;
            for (std::size_t i = 0; i < cmd_history_.size(); ++i)
            {
               std::cout << i << "\t" << cmd_history_[i] << "\n";
            }
            continue;
         }
         else if (strtk::imatch("clear history",command))
         {
            cmd_history_.clear();
            continue;
         }
         else if (strtk::imatch("help",command))
         {
            std::string help = "Command             Definition                     \n"
                               "-------             ----------                     \n"
                               "def <name>=<value>  Define a variable and its value\n"
                               "ls                  List all variables.            \n"
                               "history             List all previous commands.    \n"
                               "precision=<value>   Set the display precision.     \n"
                               "precision           Display precision.             \n"
                               "help gen            List of general functions      \n"
                               "help logic          List of logic functions        \n"
                               "help trig           List of trigonometry functions \n"
                               "quit                Quit program.                  \n";
            std::cout << help;
            continue;
         }
         else if (0 == strtk::ifind("help",command))
         {
            process_help(command);
            continue;
         }
         else if (strtk::imatch("quit",command) || strtk::imatch("exit",command))
         {
            break;
         }
         else
         {
            if (!parser_.compile(command,expression_))
            {
               std::cout << "ERROR - " << parser_.error() << "\tExpression: " << command << std::endl;
               continue;
            }
            T result = expression_.value();
            std::cout << result << "\n";
         }
      }
   }

private:

   void process_symbol_definition(const std::string& cmd)
   {
      std::string def_token;
      std::string var_token;
      unsigned char equal_sign;
      T val_token;
      if(!strtk::parse(cmd," \t\r\n",def_token,var_token,equal_sign,val_token))
      {
         std::cout << "ERROR - Invalid variable definition: [" << cmd << "]\n";
         return;
      }
      else if (!strtk::imatch(def_token,"def"))
      {
         std::cout << "ERROR - Invalid variable definition: [" << cmd << "]\n";
         return;
      }
      else if (symbol_table_.symbol_exists(var_token))
      {
         symbol_table_.get_variable(var_token)->ref() = val_token;
         return;
      }
      if (!symbol_table_.create_variable(var_token,val_token))
      {
         std::cout << "ERROR - Failed to define variable: [" << var_token << "]\n";
         return;
      }
   }

   void process_precision_command(const std::string& cmd)
   {
      std::string prec_token;
      unsigned int val_token = 0;
      if(!strtk::parse(cmd," =",prec_token,val_token))
      {
         std::cout << "ERROR - Invalid variable definition: [" << cmd << "]\n";
         return;
      }
      else if (prec_token != "precision")
      {
         std::cout << "ERROR - Invalid precision definition: [" << cmd << "]\n";
         return;
      }
      else if (val_token > 20)
      {
         std::cout << "ERROR - Invalid precision value: [" << val_token << "], should be <= 20\n";
         return;
      }
      display_precision_ = val_token;
      std::cout.precision(display_precision_);
   }

   void process_help(const std::string& cmd)
   {
      if (strtk::imatch("help trig",cmd))
      {
         std::string help_trig = "Function    Definition         \n"
                                 "--------    ----------         \n"
                                 "acos        ArcCosine          \n"
                                 "asin        ArcSine            \n"
                                 "atan        ArcTangent         \n"
                                 "atan2       ArcTangent         \n"
                                 "cos         Cosine             \n"
                                 "sin         Sine               \n"
                                 "tan         Tangent            \n"
                                 "cosh        Hyperbolic Cosine  \n"
                                 "sinh        Hyperbolic Sine    \n"
                                 "tanh        Hyperbolic Tangent \n"
                                 "sec         Secant             \n"
                                 "csc         Cosecant           \n"
                                 "cot         Cotangent          \n"
                                 "rad2deg     Radians to Degrees \n"
                                 "deg2rad     Degrees to Radians \n"
                                 "deg2grad    Degrees to Gradians\n"
                                 "grad2deg    Gradians to Degrees\n"
                                 "hyp         Hypotenus          \n";

         std::cout << help_trig;
      }
      else if (strtk::imatch("help gen",cmd))
      {
         std::string help_gen = "Function      Definition         \n"
                                "--------      ----------         \n"
                                "abs           Absolute value     \n"
                                "min           Minimum of values  \n"
                                "max           Maximum of values  \n"
                                "avg           Average of values  \n"
                                "sum           Summation of values\n"
                                "ceil          Ceiling of value   \n"
                                "floor         Floor of value     \n"
                                "exp           e^value            \n"
                                "log           Natural logarithm  \n"
                                "log10         Logarithm base 10  \n"
                                "logn          Logarithm base N   \n"
                                "round         Round 1/10th       \n"
                                "roundn        Round n digits     \n"
                                "root          Root-n of value    \n"
                                "sqrt          Square root        \n"
                                "clamp         Clamp value        \n";
         std::cout << help_gen;
      }
      else if (strtk::imatch("help logic",cmd))
      {
         std::string help_logic = "Function     Definition        \n"
                                  "--------     ----------        \n"
                                  "and          Logical AND       \n"
                                  "or           Logical OR        \n"
                                  "xor          Exclusive OR      \n"
                                  "not          Logical NOT       \n"
                                  "nand         Logical NOT-AND   \n"
                                  "nor          Logical NOT-OR    \n"
                                  "shr          Shift Right N     \n"
                                  "shl          Shift Left N      \n"
                                  "inrange      Value within range\n";
         std::cout << help_logic;
      }
   }

   std::deque<std::string> cmd_history_;
   exprtk::symbol_table<T> symbol_table_;
   exprtk::expression<T>   expression_;
   exprtk::parser<T>       parser_;
   std::size_t             display_precision_;
};

int main()
{
   nanocalc<double> nc;
   nc.run();
   return 0;
}
