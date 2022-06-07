#ifndef CPQD_MRCP_WS_PARSER_H
#define CPQD_MRCP_WS_PARSER_H

#include "expected.h"

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <vector>

namespace cpqd {

enum class TokenType {
  SEP_OP,       ///< ":"
  ID,           ///< Any string including the version
  ASR_TITLE,    ///< The title
  CRLF,         ///< Used for new line
  EOFT          ///< End mark
};

/**
 * Token class
 */
struct Token {
  TokenType kind_;
  std::string content_;

  Token(TokenType kind, const std::string& content = "")
  : kind_(kind)
  , content_(content){}

  Token()
  : kind_(TokenType::EOFT)
  , content_(""){}

  Token(const Token& tk)
  : kind_(tk.kind_)
  , content_(tk.content_){}

  Token& operator=(const Token& tk) {
    kind_ = tk.kind_;
    content_ = tk.content_;

    return *this;
  }

  Token(Token&& tk)
  : kind_(tk.kind_)
  , content_(std::move(tk.content_)){}

  Token& operator=(Token&& tk) {
    kind_ = tk.kind_;
    content_ = std::move(tk.content_);

    return *this;
  }
};

/**
 * Model the tokenize from stream of string
 */
class TokenStream {
 public:

  /**
   * @brief constructor
   */
  TokenStream(const std::string& str);

  /**
   * @brief Default constructor
   */
  TokenStream();

  TokenStream(const TokenStream& ts);

  TokenStream& operator=(const TokenStream& ts);

  TokenStream(TokenStream&& ts);

  TokenStream& operator=(TokenStream&& ts);

  /**
   * @brief Tokenize a string
   *
   * This method split the string according
   * with a finite automato to split the
   * string into tokens
   */
  void Tokenize(const std::string& str);

  /**
   * @brief Gets the current Token
   *
   * @return Token
   */
  Token Get();

  /**
   * @brief Go back with the token
   *
   * The token has a pointer that indicates what token
   * is the current token, Unget function is like
   * --i operation on token pointer
   */
  void Unget();

  void Space(bool v);

 private:
  /**
   * @brief Gets the token and advance the pointer
   *
   * @return Token
   */
  Token GetNext();

  /**
   * @brief Check is the char is a symbol
   *
   * A symbol can be (-), (/), (.), (_)
   *
   * @return true or false
   */
  bool IsSymbol(char simb, bool);

  std::string str_;
  std::string::iterator it_;
  Token last_;
  bool unget_;
  bool after_sep_;
};

/**
 * @class WsParser
 * @brief Parser a string from protocol
 *
 * Convert a string on key value protocol
 * format on an map, and convert an map
 * on a string from the protocol according
 * bnf rules:
 *
 * STM    : CRLF    | Expr CRLF
 * Expr   : CmdExpr | KeyExpr
 * CmdExpr: ASR_TITLE ID ID
 * KeyExpr: ID ':' ID
 */
class WsParser {
 public:
  /**
   * @brief Constructor
   */
  explicit WsParser(const std::string& str);

  /**
   * @brief Default constructor
   */
  WsParser();

  WsParser(const WsParser& p);

  WsParser& operator=(const WsParser& p);

  WsParser(WsParser&& p);

  WsParser& operator=(WsParser&& p);

  /**
   * @brief Set the request type
   *
   * The cmd could be SEND_AUDIO, START_RECOGNITION
   *
   * @param str the name of the request
   */
  void SetCmd(const std::string& str);

  /**
   * @brief Gets the request type
   *
   * The cmd could be SEND_AUDIO, START_RECOGNITION
   *
   * @return the name of the request
   */
  std::string GetCmd() const;

  /**
   * @brief Gets the version of the protocol
   *
   * @return a string with the version
   */
  std::string GetVersion() const;

  /**
   * @brief Set the version of the protocol
   *
   * @param str string of the version
   */
  void SetVersion(const std::string& str);

  /**
   * @brief Gets the value of the key
   *
   * @return An expected string with the value
   */
  Expected<std::string> Get(const std::string& str) const;

  /**
   * @brief Gets sevaral values from keys
   *
   * This function return all values from keys
   * given on arrry
   *
   * @param vec vector with the keys
   * @return An expected array of string with the value
   */
  Expected<std::vector<std::string>> GetParams(const std::vector<std::string>& vec) const;

  /**
   * @brief Adds a new key on protocol package
   *
   * This method is used when the user is assembly the package
   *
   * @param str_key is the key
   * @param str_value is the value
   */
  void Add(const std::string& str_key, const std::string& str_value);

  /**
   * @brief Allows cast the package to std::string
   */
  operator std::string() const;

  /**
   * @brief Converts the package to std::string
   */
  std::string Str() const;

  /**
   * @brief Set the body of the package
   *
   * @param body string of content
   */
  void SetBody(const std::string& body);

  /**
   * @brief Gets the body of the package
   *
   * @return string of content
   */
  const std::string& GetBody() const;

  /**
   * @brief Gets all params
   *
   * @return a map with the params
   */
  std::map<std::string, std::string>& GetParams();

 private:
  /**
   * @brief Models the statment of the language
   *
   * Everything is a statment
   */
  void Stm();

  /**
   * @brief Models the expression of the language
   *
   * Is a command or a key value expression
   */
  void Expr();

  /**
   * @brief Models key value expression
   *
   * Expr   : CmdExpr | KeyExpr
   */
  void KeyExpr();

  /**
   * @brief Models command expression
   *
   * CmdExpr: ASR_TITLE ID ID
   */
  void CmdExpr();

  TokenStream tks_;

  std::string command_;
  std::string version_;
  std::string body_;
  bool has_body_;
  size_t size_body_;
  std::map<std::string, std::string> props_;
};

}  // namespace cpqd_mrcp

#endif  // CPQD_MRCP_WS_PARSER_H
