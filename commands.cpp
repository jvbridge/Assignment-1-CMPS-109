// $Id: commands.cpp,v 1.11 2014-06-11 13:49:31-07 - - $
// MODIFY IT!
#include "commands.h"
#include "debug.h"
#include <vector>

commands::commands(): map ({
   {"cat"   , fn_cat   },
   {"cd"    , fn_cd    },
   {"echo"  , fn_echo  },
   {"exit"  , fn_exit  },
   {"ls"    , fn_ls    },
   {"lsr"   , fn_lsr   },
   {"make"  , fn_make  },
   {"mkdir" , fn_mkdir },
   {"prompt", fn_prompt},
   {"pwd"   , fn_pwd   },
   {"rm"    , fn_rm    },
   {"quit"  , fn_exit  }, // added my own little "alias" that I use
}){}

// HELPER FUNCTIONS ====================================================
/**
 * Helper function to pop off the command
 * @param  words  a wordvec that consists of whole line
 * @return        returns the first word of the command
 */
wordvec pop_command(wordvec words){
   DEBUGF('c', "running pop_command");
   wordvec tmp = words;
   DEBUGF('c', "tmp = " << tmp);
   tmp.erase(tmp.begin());
   DEBUGF('c', "tmp modified, tmp = " << tmp);
   DEBUGF('c', "words = " << words);
   return tmp;
}

/**
 * Parses a string into an array of folder names
 * @param path String that describes the path
 */
string parse_path(string path){
   // TODO
   return path;
}

command_fn commands::at (const string& cmd) {
   // Note: value_type is pair<const key_type, mapped_type>
   // So: iterator->first is key_type (string)
   // So: iterator->second is mapped_type (command_fn)
   command_map::const_iterator result = map.find (cmd);
   if (result == map.end()) {
      throw yshell_exn (cmd + ": no such function");
   }
   return result->second;
}

/**
 * The contents of each file is copied to stdout. An error is reported
 * if no files are specified, a file does not exist, or there is no
 * directory.
 */
void fn_cat (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);

   // if no arguments are given,  print an error and return
   if (words.size() == 1){
      cout << "Error: no arguments given to cat" << endl;
      return;
   }

   // iterate through the commands and print out the contents
   // this essentially hands off the code to the inode state
   /*for (int i = 1; i < words.size(); i++){
         String path = words.at(i);
         cout << state.readfile(path) << endl;
   }*/

}

void fn_cd (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

/**
 * Prints out the words given to the arguemnt
 * @param state unused inode state
 * @param words the command given to the
 */
void fn_echo (inode_state& state, const wordvec& words){
   // Debug stuff (Unused)
   DEBUGF ('c', state);
   DEBUGF ('c', words);

   // use helper function to delete unecessary words
   wordvec tmp = pop_command(words);

   // print it to standard out
   cout << tmp << endl;
}

/**
 * Exits the program with the error
 * @param state the state of the current inode
 * @param words the command given
 */
void fn_exit (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);

   // if exit is the only word on the line, exit just like that
   if (words.size() == 1){
      DEBUGF('c', "EXITING! No arguments given");
      throw ysh_exit_exn();
   }

   /*
   otherwise we are given an exit status to set and should interpret
   it properly. A try/catch block is used here to make sure it's valid
    */
   try {
      // convert the argument is a string, interperet it as an int
      int exit_code = stoi(words.at(1));
      exit_status::set(exit_code);
   } catch (std::invalid_argument& e){
      // if invalid argument is given, exit with status 127
      exit_status::set(127);
   }
   throw ysh_exit_exn();
}

void fn_ls (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_lsr (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}


void fn_make (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_mkdir (inode_state& state, const wordvec& words){

   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_prompt (inode_state& state, const wordvec& words){
   // allocate new string
   string new_prompt;

   // set the string to be the correct one, starts at 1 since the first
   // word is "prompt"
   for ( unsigned i = 1; i != words.size(); i++ ) {
      new_prompt = new_prompt + words.at(i) + " ";
   }
   DEBUGF('c', "New prompt is: " << new_prompt);
   // use the inode state to set the prompt
   state.set_prompt(new_prompt);

   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_pwd (inode_state& state, const wordvec& words){

   cout << state.get_path() << endl;

   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_rm (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_rmr (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

int exit_status_message() {
   int exit_status = exit_status::get();
   cout << execname() << ": exit(" << exit_status << ")" << endl;
   return exit_status;
}
