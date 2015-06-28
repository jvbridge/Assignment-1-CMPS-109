// $Id: main.cpp,v 1.3 2014-06-11 13:52:31-07 - - $

#include <cstdlib>
#include <iostream>
#include <string>
#include <utility>
#include <unistd.h>

using namespace std;

#include "commands.h"
#include "debug.h"
#include "inode.h"
#include "util.h"

//
// scan_options
//    Options analysis:  The only option is -Dflags. 
//

/**
 * Scans the options and sets flags as appropriate
 * @param argc The number of arguments given to main
 * @param argv The arguments given to main
 */
void scan_options (int argc, char** argv) {
   opterr = 0; // count of all the options
   for (;;) {
      // option is a 
      int option = getopt (argc, argv, "@:");
      if (option == EOF) break;
      switch (option) {
         case '@':
            debugflags::setflags (optarg);
            break;
         default:
            complain() << "-" << (char) option << ": invalid option"
                       << endl;
            break;
      }
   }
   if (optind < argc) {
      complain() << "operands not permitted" << endl;
   }
}

/**
 * Helper function that checks if the line is commented
 * @param  words  the command given
 * @return        true if the line starts with "#" false otherwise
 */
bool check_comment(wordvec words){
   string first_word = words.front();
   char first_char = first_word.at(0);
   if (first_char == ('#')){
      DEBUGF('c', "Line is commented!")
      return true;
   } else {
      DEBUGF('c', "Line is NOT commented!")
      return false;
   }
}



//
// main -
//    Main program which loops reading commands until end of file.
//

int main (int argc, char** argv) {
   // 
   execname (argv[0]);
   cout << boolalpha; // Print false or true instead of 0 or 1.
   cerr << boolalpha;
   cout << argv[0] << " build " << __DATE__ << " " << __TIME__ << endl;
   scan_options (argc, argv);
   bool need_echo = want_echo();
   commands cmdmap;
   string prompt = "%";
   inode_state state;
   try {
      for (;;) {
         try {
   
            // Read a line, break at EOF, and echo print the prompt
            // if one is needed.
            cout << prompt << " ";
            string line;
            getline (cin, line);
            if (cin.eof()) {
               if (need_echo) cout << "^D";
               cout << endl;
               DEBUGF ('y', "EOF");
               break;
            }
            if (need_echo) cout << line << endl;
   
            // Split the line into words and lookup the appropriate
            // function.  Complain or call it.
            wordvec words = split (line, " \t");
            DEBUGF ('y', "words = " << words);

            // if the line is commented ignore it
            if (check_comment(words)) continue;

            command_fn fn = cmdmap.at(words.at(0));
            fn (state, words);
         }catch (yshell_exn& exn) {
            // If there is a problem discovered in any function, an
            // exn is thrown and printed here.
            complain() << exn.what() << endl;
         }
      }
   } catch (ysh_exit_exn& ) {
      // This catch intentionally left blank.
   }

   return exit_status_message();
}
