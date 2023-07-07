#pragma once
#if !defined( TRACEER_H_HEADER_GUARD )
#define TRACEER_H_HEADER_GUARD 1
/**
   MFC の TRACE のような 出力マクロ群

   - wide文字専用で、OutputDebugStringW で使うのに便利なように必ずキャリッジリターンが追加される。
   - どこから出力されたか判別出来るように 関数名:ソースコードファイル名,L.ソースコード行番号 が付与される。
   - NDEBUG（通常は assert の無効化） が定義された場合は一切出力しない。
   
   ENTER_BLOCK は C++専用
   TRACEER は、C/C++両方から利用出来る。
   
   ENTER_BLOCK( fmt , ... )
   スコープ単位で、ログを出力するためのマクロ 実体は、
   _wh_trace_block_variable_at_xxxx (xxxxは行番号) の wh::TraceBlock のインスタンスが作成され
   コンストラクタとデストラクタで、 [ENTER] [LEAVE] のログを出力する

   この ENTER は、インデントを追加し LEAVE はインデントを抜ける
   このインデントは、TRACEに影響しスレッド毎にインデントの深さを持つ

   TRACEER( fmt , ... )
   TRACE のようなマクロ。 ENTER_BLOCK のインデントの影響を受ける。
   fmt と可変長引数は、 printf系 の書式を参考に。

   ヒント：
   L"%ls" wide の文字列
   L"%hs" narrow の文字列
   
*/

/**************************
   MFC VERIFY macro like  define
**************************/

#if defined( __cplusplus )
# include <cassert>
#else /* defined( __cplusplus ) */
# include <assert.h>
#endif /* defined( __cplusplus ) */

#ifndef VERIFY
# ifdef NDEBUG 
#  define VERIFY(expression) do{ ( expression ); }while( false )
# else /* NDEBUG */
#  define VERIFY(expression) assert( expression )
# endif /* NDEBUG */
#endif  /* VERIFY*/

#include <string.h>
#include <malloc.h>
#include <strsafe.h>

#if defined( __cplusplus )
# include <cstdarg>
#endif /* defined( __cplusplus ) */

#if !defined( STR0 )
# define STR0( exp )      #exp
#endif /* !defined( STR0 ) */

#if !defined( STRINGIFY )
# define STRINGIFY( exp ) STR0( exp )
#endif /* !defined( STRINGIFY ) */

#if !defined( TRACEER )
# if defined( NDEBUG )
#  define TRACEER( ... )   ( void )( __VA_ARGS__ ) 
# else /* defined( NDEBUG ) */
#  define TRACEER( ... )   ( (void)debug_output_traceer( L" (" __FUNCTION__ "():" __FILE__ ",L." STRINGIFY( __LINE__ ) ")\n", __VA_ARGS__ ) )
# endif /* defined( NDEBUG ) */
#endif /* !defined( TRACEER ) */


#if defined(__cplusplus ) 

# if !defined(DECLEAR_IDENTITY_VARIABLE_0)
#  define DECLEAR_IDENTITY_VARIABLE_0( exp , num ) exp ## num
# endif /* !defined(DECLEAR_IDENTITY_VARIABLE_0) */

# if !defined( DECLEAR_IDENTITY_VARIABLE )
#  define DECLEAR_IDENTITY_VARIABLE( exp , num )   DECLEAR_IDENTITY_VARIABLE_0(exp,num)
# endif /* !defined( DECLEAR_IDENTITY_VARIABLE ) */

# if !defined( ENTER_BLOCK )
#  if defined( NDEBUG )
#   define ENTER_BLOCK( ... ) (void)( __VA_ARGS__ )
#  else /* !defined( NDEBUG ) */
#   define ENTER_BLOCK( ... )                                             \
  auto DECLEAR_IDENTITY_VARIABLE( _wh_trace_block_variable_at_ , __LINE__ ) \
  = wh::trace_block_concreate( L" (" __FUNCTION__ "():" __FILE__ ",L." STRINGIFY( __LINE__ ) ")\n", __VA_ARGS__ )
#  endif /* !defined( NDEBUG ) */
# endif /* !defined( ENTER_BLOCK ) */

#endif /* defined( __cplusplus ) */



#if defined( TRACEER_CPP_IMPLEMENT ) && !defined( _DLL )
#define TRACEER_DLL_EXPORT __declspec( dllexport )
#else /* defined( TRACEER_CPP_IMPLEMENT ) && !defined( _DLL ) */
#define TRACEER_DLL_EXPORT /* */
#endif /* defined( TRACEER_CPP_IMPLEMENT ) && !defined( _DLL ) */

#if defined(__cplusplus ) 
extern "C"{
#endif /* defined(__cplusplus )  */

  TRACEER_DLL_EXPORT void debug_output_traceer( const wchar_t* where , const wchar_t* fmt, ... );
  TRACEER_DLL_EXPORT void debug_output_trace_block_enter( const wchar_t* where , const wchar_t* what );
  TRACEER_DLL_EXPORT void debug_output_trace_block_leave( const wchar_t* wheret , const wchar_t* what );

#if defined( __cplusplus )
};
#endif /* defined(__cplusplus )  */

#if defined(__cplusplus ) 
namespace wh{
  enum { debug_output_storage_size = 1024 };

  TRACEER_DLL_EXPORT int&
  debug_output_indent_level() noexcept;

  TRACEER_DLL_EXPORT wchar_t
  (&debug_output_storage() noexcept)[debug_output_storage_size];
  
  class TraceBlock{
    /** この where は、 ENTER_BLOCK の中で作成した文字列リテラルなので、 ( __FUNCTION__():__FILE__,L.xxx )\n の文字列なので開放は不要 */
    const wchar_t * const where;
    /** こちらは、 trace_block_concreate の中の _wcsdup() で作成した ので free() で開放が必要*/
    const wchar_t * msg; 
  public:
    TraceBlock() = delete;
    inline TraceBlock( const wchar_t *where , const wchar_t *what )
      : where( where ),
        msg( what )
    {
      debug_output_trace_block_enter( where , msg);
    }
    inline ~TraceBlock()
    {
      debug_output_trace_block_leave( where , msg);
      free( (void*)msg ); 
    }
  };
  
  template<size_t N>
  static inline TraceBlock 
  trace_block_concreate( const wchar_t (&where)[N] , const wchar_t *fmt ,...){    
    va_list args;
    va_start( args , fmt );
    wchar_t *dst_end = nullptr;
    size_t remaining{0};
    static_assert( std::extent< std::remove_reference<decltype( debug_output_storage() )>::type  >::value  == 1024 ) ;
    if( S_OK == StringCchVPrintfExW( debug_output_storage() ,
                                     std::extent< std::remove_reference<decltype( debug_output_storage())>::type >::value ,
                                     &dst_end ,
                                     &remaining ,
                                     STRSAFE_FILL_BEHIND_NULL | STRSAFE_NULL_ON_FAILURE ,
                                     fmt ,
                                     args )){
    }
    va_end( args );
    return TraceBlock{where, _wcsdup( debug_output_storage() )};
  }
};

#endif /* defined( __cplusplus ) */

#endif /* !defined( TRACEER_H_HEADER_GUARD ) */
