#include "ws_parser.h"
#include "exceptions.h"

#include <iostream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <cstdio>

namespace cpqd {

TokenStream::TokenStream(const std::string& str)
  : str_(str)
  , it_(str_.begin())
  , unget_(false)
  , after_sep_(false) {
}

TokenStream::TokenStream(const TokenStream& ts)
  : str_(ts.str_)
  , it_(ts.it_)
  , last_(ts.last_)
  , unget_(ts.unget_)
  , after_sep_(ts.after_sep_) {}

TokenStream& TokenStream::operator=(const TokenStream& ts) {
  str_ = ts.str_;
  it_ = ts.it_;
  last_ = ts.last_;
  unget_ = ts.unget_;
  after_sep_ = ts.after_sep_;

  return *this;
}

TokenStream::TokenStream(TokenStream&& ts)
  : str_(std::move(ts.str_))
  , it_(ts.it_)
  , last_(std::move(ts.last_))
  , unget_(ts.unget_)
  , after_sep_(ts.after_sep_) {}

TokenStream& TokenStream::operator=(TokenStream&& ts) {
  str_ = std::move(ts.str_);
  it_ = ts.it_;
  last_ = std::move(ts.last_);
  unget_ = ts.unget_;
  after_sep_ = ts.after_sep_;

  return *this;
}

TokenStream::TokenStream()
  : str_("")
  , it_(std::string("").begin())
  , unget_(false)
  , after_sep_(false) {
}

void TokenStream::Tokenize(const std::string& str) {
  str_ = str;
  it_ = str_.begin();
  unget_ = false;
  after_sep_ = false;
}

Token TokenStream::Get() {
  if (unget_) {
    unget_ = false;
    return last_;
  }

  last_ = GetNext();
  return last_;
}

void TokenStream::Unget() {
  unget_ = true;
}

Token TokenStream::GetNext() {
  auto is_space = [](char c, bool check) -> bool {
    if (check)
      if (c == ' ')
        return true;

    return false;
  };

  while (it_ != str_.end()) {
    if (*it_ == ' ') {
      it_++;
      continue;
    }

    if (*it_ == ':') {
      it_++;
      after_sep_ = true;
      return Token(TokenType::SEP_OP);
    } else if (*it_ == '\n') {
      it_++;
      after_sep_ = false;
      return Token(TokenType::CRLF);
    } else if (isalpha(*it_) || isdigit(*it_) || IsSymbol(*it_, after_sep_)
               || is_space(*it_, after_sep_)) {
      std::string s;
      s += *it_;
      it_++;
      while (isalpha(*it_) || isdigit(*it_) || IsSymbol(*it_, after_sep_)
             || is_space(*it_, after_sep_)) {
        s += *it_;
        it_++;
      }

      if (s == "ASR") {
        return Token(TokenType::ASR_TITLE);
      }

      return Token(TokenType::ID, s);
    } else {
      THROW_EXCEPTION(Error::PARSER,"Invalid Token");
    }
  }

  after_sep_ = false;
  return Token(TokenType::EOFT);
}

bool TokenStream::IsSymbol(char simb, bool separtor) {
  switch (simb) {
    case '-':
    case '/':
    case '.':
    case ',':
    case '_':
    case '(':
    case ')':
    case '>':
    case '<':
      return true;
    case ':':
      if (separtor)
        return true;
      return false;
    default:
      return false;
  }
}

void TokenStream::Space(bool v) {
  after_sep_ = v;
}

WsParser::WsParser(const std::string& str)
  : has_body_(false)
  , size_body_(0) {
  if (str.empty())
    return;

  std::istringstream iss(str);

  std::string line;
  int iline = 1;
  while (std::getline(iss, line)) {
    if ((line == "\r") && (has_body_))
      break;
    line.pop_back();
    line = line + '\n';
    tks_.Tokenize(line);
    try {
      Stm();
    } catch (Exception &e) {
      break;
    }

    iline++;
  }

  if (has_body_) {
    for (size_t i = size_body_; i > 0; i--) {
      body_ += str[str.length() - i];
    }
  }
}

WsParser::WsParser()
  : has_body_(false)
  , size_body_(0){
}

WsParser::WsParser(const WsParser& p)
  : tks_(p.tks_)
  , command_(p.command_)
  , version_(p.version_)
  , body_(p.body_)
  , has_body_(p.has_body_)
  , size_body_(p.size_body_)
  , props_(p.props_) {}

WsParser& WsParser::operator=(const WsParser& p) {
  tks_ = p.tks_;
  command_ = p.command_;
  version_ = p.version_;
  body_ = p.body_;
  has_body_ = p.has_body_;
  size_body_ = p.size_body_;
  props_ = p.props_;

  return *this;
}

WsParser::WsParser(WsParser&& p)
  : tks_(std::move(p.tks_))
  , command_(std::move(p.command_))
  , version_(std::move(p.version_))
  , body_(std::move(p.body_))
  , has_body_(p.has_body_)
  , size_body_(p.size_body_)
  , props_(std::move(p.props_)) {}

WsParser& WsParser::operator=(WsParser&& p) {
  tks_ = std::move(p.tks_);
  command_ = std::move(p.command_);
  version_ = std::move(p.version_);
  body_ = std::move(p.body_);
  has_body_ = p.has_body_;
  size_body_ = p.size_body_;
  props_ = std::move(p.props_);

  return *this;
}

void WsParser::CmdExpr() {
  Token tk = tks_.Get();

  if (tk.kind_ != TokenType::ASR_TITLE)
    THROW_EXCEPTION(Error::PARSER, "Expected ASR Title expression");

  tk = tks_.Get();
  if (tk.kind_ != TokenType::ID)
    THROW_EXCEPTION(Error::PARSER, "Expected ID string");
  version_ = tk.content_;

  tk = tks_.Get();
  if (tk.kind_ != TokenType::ID)
    THROW_EXCEPTION(Error::PARSER, "Expected ID string");
  command_ = tk.content_;
}

void WsParser::KeyExpr() {
  Token tk = tks_.Get();
  std::string key;
  std::string value;

  if (tk.kind_ != TokenType::ID)
    THROW_EXCEPTION(Error::PARSER, "Expected ID string");
  key = tk.content_;

  tk = tks_.Get();
  if (tk.kind_ != TokenType::SEP_OP)
    THROW_EXCEPTION(Error::PARSER, "Expected separator operator");

  tk = tks_.Get();
  if (tk.kind_ != TokenType::ID && tk.kind_ != TokenType::CRLF)
    THROW_EXCEPTION(Error::PARSER, "Expected ID string or End of Line");
  value = tk.content_;

  if (key == "Content-Length") {
    has_body_ = true;
    size_body_ = std::stoi(value, nullptr);
  }

  props_.insert(std::pair<std::string, std::string>(key, value));
}

void WsParser::Expr() {
  Token tk = tks_.Get();
  tks_.Unget();

  if (tk.kind_ == TokenType::ASR_TITLE)
    CmdExpr();
  else
    KeyExpr();
}

void WsParser::Stm() {
  Token tk = tks_.Get();
  tks_.Unget();

  if (tk.kind_ == TokenType::CRLF)
    return;

  Expr();

  tk = tks_.Get();
  if (tk.kind_ != TokenType::CRLF && tk.kind_ != TokenType::EOFT)
    THROW_EXCEPTION(Error::PARSER, "Expected new line terminator");
}

Expected<std::string> WsParser::Get(const std::string& key) const{
  auto it = props_.find(key);

  if (it == props_.end())
    return Expected<std::string>::fromException(
        MAKE_EXCEPTION(Error::PARSER, "key not declared"));

  std::string ret = it->second;
  return std::move(ret);
}

Expected<std::vector< std::string >> WsParser::GetParams(
    const std::vector< std::string >& vec) const{
  std::vector<std::string> values;
  for (const auto& s : vec) {
    auto value = Get(s);
    if (!value.valid())
      return Expected<std::vector< std::string>>::fromException(
          MAKE_EXCEPTION(Error::PARSER, "key not %s declared", s.c_str()));

    values.push_back(value.get());
  }

  return std::move(values);
}

std::map<std::string, std::string>& WsParser::GetParams() {
  return props_;
}

void WsParser::Add(const std::string& str_key, const std::string& str_value) {
  props_.insert(std::pair<std::string, std::string>(str_key, str_value));
}

std::string WsParser::GetCmd() const{
  return command_;
}

void WsParser::SetCmd(const std::string& str) {
  command_ = str;
}

std::string WsParser::GetVersion() const{
  return version_;
}

void WsParser::SetVersion(const std::string& str) {
  version_ = str;
}

void WsParser::SetBody(const std::string& body) {
  body_ = body;
  has_body_ = true;
}

const std::string& WsParser::GetBody() const {
  return body_;
}

std::string WsParser::Str() const{
  std::string str = "ASR " + version_ + " " + command_ + "\r\n";

  for (const auto& kv : props_)
    str += kv.first + ":" + kv.second + "\r\n";

  if (!body_.empty()) {
    str += "\r\n";
    str += body_;
  }

  return std::move(str);
}

WsParser::operator std::string() const{
  return std::move(Str());
}

}
