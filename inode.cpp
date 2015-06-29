// $Id: inode.cpp,v 1.12 2014-07-03 13:29:57-07 - - $

#include <iostream>
#include <stdexcept>

using namespace std;

#include "debug.h"
#include "inode.h"

int inode::next_inode_nr {1};

inode::inode(inode_t init_type):
   inode_nr (next_inode_nr++), type (init_type)
{
   switch (type) {
      case PLAIN_INODE:
           contents = make_shared<plain_file>();
           break;
      case DIR_INODE:
           contents = make_shared<directory>();
           break;
   }
   DEBUGF ('i', "inode " << inode_nr << ", type = " << type);
}

int inode::get_inode_nr() const {
   DEBUGF ('i', "inode = " << inode_nr);
   return inode_nr;
}

plain_file_ptr plain_file_ptr_of (file_base_ptr ptr) {
   plain_file_ptr pfptr = dynamic_pointer_cast<plain_file> (ptr);
   if (pfptr == nullptr) throw invalid_argument ("plain_file_ptr_of");
   return pfptr;
}

directory_ptr directory_ptr_of (file_base_ptr ptr) {
   directory_ptr dirptr = dynamic_pointer_cast<directory> (ptr);
   if (dirptr == nullptr) throw invalid_argument ("directory_ptr_of");
   return dirptr;
}


size_t plain_file::size() const {
   size_t size {0};
   DEBUGF ('i', "size = " << size);
   return size;
}

const wordvec& plain_file::readfile() const {
   DEBUGF ('i', data);
   return data;
}

void plain_file::writefile (const wordvec& words) {
   DEBUGF ('i', words);
}

size_t directory::size() const {
   size_t size {0};
   DEBUGF ('i', "size = " << size);
   return size;
}

void directory::remove (const string& filename) {
   DEBUGF ('i', filename);
}

/**
 * the constructor for inode_state. Initializes the root and current
 * working directory to be appropriate for the start.
 */
inode_state::inode_state() {
   // set the root to be a pointer to the inode, specify the enum.
   this->root = make_shared<inode>(DIR_INODE);

   directory_ptr_of(root->contents)->set_dot(root);
   directory_ptr_of(root->contents)->set_dotdot(root);

   // set the current working directory to be the root
   this->cwd = root;

   DEBUGF ('i', "root = " << root << ", cwd = " << cwd
          << ", prompt = \"" << prompt << "\"");
}

ostream& operator<< (ostream& out, const inode_state& state) {
   out << "inode_state: root = " << state.root
       << ", cwd = " << state.cwd;
   return out;
}

// MY FUNCTIONS =======================================================

// INODE state ========================================================
/**
 * setter for the shell's prompt
 * @param new_prompt a string that will be the new prompt. Will
 * automatically have a space appended to it.
 */
void inode_state::set_prompt(const string& new_prompt){
   this->prompt = new_prompt;
};

/**
 * getter for the shell's prompt
 * @return a string for the prompt
 */
const string& inode_state::get_prompt(){
   return this->prompt;
}

// directory ==========================================================
/**
 * Setter for the parent of a director
 * @param parent inode pointer pointing to the parent of the directory
 * Will always point at another directory except for the root which
 * will point at itself.
 */
void directory::set_dotdot(inode_ptr parent){
   if (dirents.find("..") == dirents.end()){
      // directory .. does not exist, insert it;
      pair<string, inode_ptr> dot_dot = std::make_pair("..", parent);
      dirents.insert(dot_dot);
   }else {
      dirents.at("..") =  parent;
   }
}

/**
 * Sets the directory's "dot". Will always point at itself.
 * @param dot inode pointer that refers to directory
 */
void directory::set_dot(inode_ptr dot){
   if (dirents.find(".") == dirents.end()){
      // if directory . does not exist, insert it in
      pair<string, inode_ptr> new_dot = std::make_pair(".", dot);
      dirents.insert(new_dot);
   }
   else {
      dirents.at(".") = dot;
   }
}
