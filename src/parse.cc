
/*
Copyright (C) 2013-2014  GuiQuan Zhang
Copyright (C) 2008-2009  Anders Hedstrom

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/


#include <stdlib.h>
#include <string.h>
#include <xsd2cc/parse.h>


namespace xsd2cc {

Parse::Parse()
   : the_str_(""),
     splits_(""),
     ord_(""),
     the_ptr_(0),
     breakchar_(0),
     enable_(0),
     disable_(0),
     nospace_(0),
     quote_(false)
{

}


Parse::Parse(const std::string& s)
   : the_str_(s),
     splits_(""),
     ord_(""),
     the_ptr_(0),
     breakchar_(0),
     enable_(0),
     disable_(0),
     nospace_(0),
     quote_(false)
{

}


Parse::Parse(const std::string& s, const std::string& sp)
   : the_str_(s),
     splits_(sp),
     ord_(""),
     the_ptr_(0),
     breakchar_(0),
     enable_(0),
     disable_(0),
     nospace_(0),
     quote_(false)
{

}


Parse::Parse(const std::string& s, const std::string& sp, short nospace)
   : the_str_(s),
     splits_(sp),
     ord_(""),
     the_ptr_(0),
     breakchar_(0),
     enable_(0),
     disable_(0),
     nospace_(1),
     quote_(false)
{

}


#define C ((the_ptr_<the_str_.size()) ? the_str_[the_ptr_] : 0)

short Parse::issplit(const char c)
{
  for (size_t i = 0; i < splits_.size(); i++) {
    if (splits_[i] == c) {
      return 1;
    }
  }

  return 0;
}


void Parse::getsplit()
{
  size_t x;

  if (C == '=') {
    x = the_ptr_++;
  } else {
    while (C && (issplit(C)))
      the_ptr_++;
    x = the_ptr_;
    while (C && !issplit(C) && C != '=')
      the_ptr_++;
  }

  if (x == the_ptr_ && C == '=') {
    the_ptr_++;
  }

  ord_ = (x < the_str_.size()) ? the_str_.substr(x,the_ptr_ - x) : "";
}


std::string Parse::getword()
{
  int     disabled = 0, quote = 0, rem = 0;
  size_t  x;

  if (nospace_) {
    while (C && issplit(C)) {
      the_ptr_++;
    }
    x = the_ptr_;
    while (C && !issplit(C) && (C != breakchar_ 
                               || !breakchar_ || disabled)) {
      if (breakchar_ && C == disable_) {
        disabled = 1;
        }

      if (breakchar_ && C == enable_) {
        disabled = 0;
        }

      if (nospace_ && C == '"') {
        quote = 1;
        }

      the_ptr_++;

      while (quote && C && C != '"') {
        the_ptr_++;
      }

     if (nospace_ && C == '"') {
        the_ptr_++;
      }

      quote = 0;
    } // while

  } else {

    if (C == breakchar_ && breakchar_) {
      x = the_ptr_++;
      rem = 1;
    } else {
      while (C && (C == ' ' || C == 9 || C == 13 || C == 10 || issplit(C))) {
        the_ptr_++;
      }
      x = the_ptr_;
      while (C && C != ' ' && C != 9 && C != 13 && C != 10 && !issplit(C) &&
       (C != breakchar_ || !breakchar_ || disabled)) {
        if (breakchar_ && C == disable_)
          disabled = 1;
        if (breakchar_ && C == enable_)
          disabled = 0;
        if (nospace_ && C == '"') {
          quote = 1;
          the_ptr_++;
          while (quote && C && C != '"') {
            the_ptr_++;
            }
          if (nospace_ && C == '"') {
            the_ptr_++;
            }
        } else {
          the_ptr_++;
          }
        quote = 0;
      }
      the_ptr_++;
      rem = 1;
    }

    if (x == the_ptr_ && C == breakchar_ && breakchar_) {
      the_ptr_++;
     }

  }

 if (x < the_str_.size()) {
    ord_ = the_str_.substr(x,the_ptr_ - x - rem);
  } else {
    ord_ = "";
  }

  return ord_;
}


void Parse::getword(std::string&s)
{
  s = Parse::getword();
}


void Parse::getsplit(std::string&s)
{
  Parse::getsplit();
  s = ord_;
}


void Parse::getword(std::string& s,std::string& fill,int l)
{
  Parse::getword();
  s.clear();

  while (s.size() + ord_.size() < (size_t)l) {
    s += fill;
  }

  s += ord_;
}


std::string Parse::getrest()
{
  std::string s;

  while (C && (C == ' ' || C == 9 || issplit(C))) {
    the_ptr_++;
  }

  s = (the_ptr_ < the_str_.size()) ? the_str_.substr(the_ptr_) : "";

  return s;
}


void Parse::getrest(std::string&s)
{
  while (C && (C == ' ' || C == 9 || issplit(C))) {
    the_ptr_++;
  }

  s = (the_ptr_ < the_str_.size()) ? the_str_.substr(the_ptr_) : "";
}


long Parse::getvalue()
{
  Parse::getword();

  return atol(ord_.c_str());
}


void Parse::setbreak(const char c)
{
  breakchar_ = c;
}


int Parse::getwordlen()
{
  size_t  x, y = the_ptr_,len;

  if (C == breakchar_ && breakchar_) {
    x = the_ptr_++;

  } else {

    while (C && (C == ' ' || C == 9 || C == 13 
      || C == 10 || issplit(C))) 
    {
      the_ptr_++;
    }

    x = the_ptr_;

    while (C && C != ' ' && C != 9 && C != 13 && C != 10 
          && !issplit(C) && (C != breakchar_ || !breakchar_))
    {
      the_ptr_++;
    }

  }

  if (x == the_ptr_ && C == breakchar_ && breakchar_) {
    the_ptr_++;
  }

  len = the_ptr_ - x;
  the_ptr_ = y;

  return (int)len;
}


int Parse::getrestlen()
{
  size_t  len, y = the_ptr_;

  while (C && (C == ' ' || C == 9 || issplit(C))) {
    the_ptr_++;
  }

  len = strlen(the_str_.c_str() + the_ptr_);
  the_ptr_ = y;

  return (int)len;
}


void Parse::getline()
{
  size_t  x;

  x = the_ptr_;
  while (C && C != 13 && C != 10) {
    the_ptr_++;
  }

  ord_ = (x < the_str_.size()) ? the_str_.substr(x,the_ptr_ - x) : "";

  if (C == 10 || C == 13) {
    the_ptr_++;
  }

}


void Parse::getline(std::string& s)
{
  getline();
  s = ord_;
}

} //namespace xsd2cc

