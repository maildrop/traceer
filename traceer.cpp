/**


 */

#define TRACEER_CPP_IMPLEMENT 1
#include "traceer.h"

#if defined( __cplusplus )
#include <cstdarg>
#else /* defined( __cplusplus ) */
#include <stdarg.h>
#endif /* defined( __cplusplus ) */

#include <iostream>
#include <type_traits>

#include <tchar.h>
#include <windows.h>

static void debug_output_traceer_v( const wchar_t* where , const wchar_t* fmt , va_list args ) noexcept;

namespace wh{
  static void debug_output( const wchar_t* msg ) noexcept
  {
    std::wcout << msg << std::flush;
    OutputDebugStringW( msg );
  }

  int& debug_output_indent_level() noexcept
  {
    __declspec( thread ) static int indent_level = 0;
    return indent_level;
  }
  
  wchar_t (&debug_output_storage() noexcept)[debug_output_storage_size]
    {
      __declspec( thread ) static wchar_t storage[debug_output_storage_size] = {0};
      return storage;
    }
};

static void debug_output_traceer_v( const wchar_t* where , const wchar_t* fmt , va_list args ) noexcept
{
#if defined( NDEBUG )
  (void)( where );
  (void)( fmt );
  (void)( args );
#else /* defined( NDEBUG ) */
  const auto end = std::end( wh::debug_output_storage() );
  auto ite = std::next( std::begin( wh::debug_output_storage() ),
                        (size_t)( (std::max)( 0 , wh::debug_output_indent_level() )));
  std::fill( std::begin( wh::debug_output_storage() ), ite , (wchar_t)' ' );
  vswprintf_s( &*ite , std::distance( ite , end ), fmt , args );
  wcscat_s( wh::debug_output_storage() , where );

  wh::debug_output( wh::debug_output_storage() );
#endif /* defined( NDEBUG ) */
  return;
}

void debug_output_traceer( const wchar_t* where , const wchar_t* fmt, ... ) noexcept 
{
  va_list args;
  va_start( args , fmt );
  debug_output_traceer_v( where , fmt , args );
  va_end( args );
  return;
}

void debug_output_trace_block_enter( const wchar_t* where , const wchar_t* what ) noexcept
{
  using namespace wh;
  StringCchPrintfW( debug_output_storage() , std::extent<std::remove_reference<decltype( debug_output_storage() )>::type >::value ,
                    L"%*ls[ENTER] %ls%ls" , debug_output_indent_level(), L"" , what , where );
  ++(debug_output_indent_level());
  debug_output( debug_output_storage() );
}
void debug_output_trace_block_leave( const wchar_t* where , const wchar_t* what ) noexcept
{
  using namespace wh;
  --(debug_output_indent_level());
  StringCchPrintfW( debug_output_storage() , std::extent<std::remove_reference<decltype( debug_output_storage() )>::type >::value ,
                    L"%*ls[LEAVE] %ls%ls" , debug_output_indent_level() , L"" , what , where );
  debug_output( debug_output_storage() );
}

