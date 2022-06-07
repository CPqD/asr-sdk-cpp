#ifndef CPQD_MRCP_EXCEPTIONS_H
#define CPQD_MRCP_EXCEPTIONS_H

#include <exception>
#include <string>

#include <sstream>
#include <string>
#include <memory>
#include <map>

namespace cpqd {

enum class Error {
  PARSER,
  LIB_ASR,
  WS_ASR,
  CONFIG,
  INVALID_HEADER,

  /** Completion cause */
  GRAMMAR_LOAD_FAILURE,         //004        | grammar-compilation-failure  | RECOGNIZE failed due to grammar load failure 
  GRAMMAR_COMPILATION_FAILURE,  //005        | grammar-load-failure         | RECOGNIZE failed due to grammar compilation failure
  RECOGNIZER_ERROR,             //006        | recognizer-error             | RECOGNIZE request terminated prematurely due to a recognizer error
  URI_FAILURE,                  //009        | uri-failure                  | Failure accessing a URI


  /** Status Code */
  ILLEGAL_VALUE_HEADER,         //404
  MISSING_REQUIRED_HEADER       //406
};

template <typename Enumeration>
auto as_integer(Enumeration const value)
    -> typename std::underlying_type<Enumeration>::type {
  return static_cast<typename std::underlying_type<Enumeration>::type>(value);
}

/**
 * @brief Class passed to an error.
 *
 * This class encapsulates all or almost all necessary
 * information about the error happened in the program. The exception is
 * usually constructed and thrown implicitly via MRCP_Error macros.
 */
class Exception : public std::exception {
 public:
  /**
   * Default constructor
   */
  Exception();

  /**
   * Full constructor. Normally the constuctor is not called explicitly.
   * Instead, the macro MRCP_Error is used.
   */
  Exception(Error code, const std::string& err, const std::string& func,
            const std::string& src_file, unsigned int line)
    : code_(code)
    , err_(err)
    , func_(func)
    , src_file_(src_file)
    , line_(line)
  {
    FormatMessage();
  }

  virtual ~Exception() noexcept  = default;

  /**
   * @return the error description and the context as a text string.
   */
  virtual const char* what() const noexcept {
    return msg_.c_str();
  }

  Error code() const noexcept {
    return code_;
  }

  virtual void FormatMessage() {
    std::stringstream ss;
//     ss << "Error " << as_integer(code_) << ": " << err_;
    ss << err_;
#ifndef NDEBUG
    ss << " on: " << func_ << " in: " << src_file_ << " line: " << line_;
#endif
    msg_ = ss.str();
  }

 private:
  std::string msg_;         // The formatted error message

  Error code_;              // error code
  std::string err_;         // error description
  std::string func_;        // function name. Available only when the compiler supports
                            // getting it
  std::string src_file_;    // source file name where the error has occured
  unsigned int line_;       // line number in the source file where the error has occured
};

/**
 * @brief std::string formatting like sprintf
 */
template <typename... Ts>
std::string format(const std::string &fmt, Ts&&... vs) {
  char b;
  // the +1 is necessary, while the first parameter
  // can also be set to nullptr
  unsigned required = std::snprintf(&b, 0, fmt.c_str(), std::forward<Ts>(vs)...) + 1;

  std::unique_ptr<char[]> bytes(new char[required]);

  std::snprintf(bytes.get(), required, fmt.c_str(), std::forward<Ts>(vs)...);

  return std::string(bytes.get());
}

#define MAKE_EXCEPTION(code, ...)                                   \
  cpqd::Exception(code, cpqd::format(__VA_ARGS__),        \
                            __FUNCTION__, __FILE__, __LINE__)

#define THROW_EXCEPTION(code, ...)                                  \
  throw cpqd::Exception(code, cpqd::format(__VA_ARGS__),  \
                            __FUNCTION__, __FILE__, __LINE__)

class ExceptionParameter : public Exception {
 public:
  using ParamsHeader = std::map<std::string, std::string>;
  /**
   * Default constructor
   */
  ExceptionParameter();

  /**
   * Full constructor. Normally the constuctor is not called explicitly.
   * Instead, the macro MRCP_Error is used.
   */
  ExceptionParameter(Error code, const std::string& err, const std::string& func,
            const std::string& src_file, unsigned int line, ParamsHeader&& headers)
    : Exception(code, err, func, src_file, line)
    , headers_(std::move(headers))
  {}

  virtual ~ExceptionParameter() noexcept  = default;

  const ParamsHeader& Params() const noexcept {
    return headers_;
  }

 private:
  ParamsHeader headers_;
};

}

#endif  // CPQD_MRCP_EXCEPTIONS_H
