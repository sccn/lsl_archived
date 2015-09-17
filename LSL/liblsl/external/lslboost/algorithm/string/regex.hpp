//  Boost string_algo library regex.hpp header file  ---------------------------//

//  Copyright Pavol Droba 2002-2003.
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.lslboost.org/LICENSE_1_0.txt)

//  See http://www.lslboost.org/ for updates, documentation, and revision history.

#ifndef BOOST_STRING_REGEX_HPP
#define BOOST_STRING_REGEX_HPP

#include <lslboost/algorithm/string/config.hpp>
#include <lslboost/regex.hpp>

#include <lslboost/range/iterator_range_core.hpp>
#include <lslboost/range/begin.hpp>
#include <lslboost/range/end.hpp>
#include <lslboost/range/iterator.hpp>
#include <lslboost/range/as_literal.hpp>

#include <lslboost/algorithm/string/find_format.hpp>
#include <lslboost/algorithm/string/regex_find_format.hpp>
#include <lslboost/algorithm/string/formatter.hpp>
#include <lslboost/algorithm/string/iter_find.hpp>

/*! \file
    Defines regex variants of the algorithms. 
*/

namespace lslboost {
    namespace algorithm {

//  find_regex  -----------------------------------------------//

        //! Find regex algorithm
        /*!
            Search for a substring matching the given regex in the input.
            
            \param Input A container which will be searched.
            \param Rx A regular expression
            \param Flags Regex options
            \return 
                An \c iterator_range delimiting the match. 
                Returned iterator is either \c RangeT::iterator or 
                \c RangeT::const_iterator, depending on the constness of 
                the input parameter.

              \note This function provides the strong exception-safety guarantee
        */
        template< 
            typename RangeT, 
            typename CharT, 
            typename RegexTraitsT>
        inline iterator_range< 
            BOOST_STRING_TYPENAME range_iterator<RangeT>::type >
        find_regex( 
            RangeT& Input, 
            const basic_regex<CharT, RegexTraitsT>& Rx,
            match_flag_type Flags=match_default )
        {
            iterator_range<BOOST_STRING_TYPENAME range_iterator<RangeT>::type> lit_input(::lslboost::as_literal(Input));

            return ::lslboost::algorithm::regex_finder(Rx,Flags)(
                ::lslboost::begin(lit_input), ::lslboost::end(lit_input) );
        }

//  replace_regex --------------------------------------------------------------------//

        //! Replace regex algorithm
        /*!
            Search for a substring matching given regex and format it with 
            the specified format.             
            The result is a modified copy of the input. It is returned as a sequence 
            or copied to the output iterator.

            \param Output An output iterator to which the result will be copied
            \param Input An input string
            \param Rx A regular expression
            \param Format Regex format definition
            \param Flags Regex options
            \return An output iterator pointing just after the last inserted character or
                    a modified copy of the input   

             \note The second variant of this function provides the strong exception-safety guarantee
        */
        template< 
            typename OutputIteratorT,
            typename RangeT, 
            typename CharT, 
            typename RegexTraitsT,
            typename FormatStringTraitsT, typename FormatStringAllocatorT >
        inline OutputIteratorT replace_regex_copy(
            OutputIteratorT Output,
            const RangeT& Input,
            const basic_regex<CharT, RegexTraitsT>& Rx,
            const std::basic_string<CharT, FormatStringTraitsT, FormatStringAllocatorT>& Format,
            match_flag_type Flags=match_default | format_default )
        {
            return ::lslboost::algorithm::find_format_copy( 
                Output,
                Input,
                ::lslboost::algorithm::regex_finder( Rx, Flags ),
                ::lslboost::algorithm::regex_formatter( Format, Flags ) );
        }

        //! Replace regex algorithm
        /*!
            \overload
        */
        template< 
            typename SequenceT, 
            typename CharT, 
            typename RegexTraitsT,
            typename FormatStringTraitsT, typename FormatStringAllocatorT >
        inline SequenceT replace_regex_copy( 
            const SequenceT& Input,
            const basic_regex<CharT, RegexTraitsT>& Rx,
            const std::basic_string<CharT, FormatStringTraitsT, FormatStringAllocatorT>& Format,
            match_flag_type Flags=match_default | format_default )
        {
            return ::lslboost::algorithm::find_format_copy( 
                Input,
                ::lslboost::algorithm::regex_finder( Rx, Flags ),
                ::lslboost::algorithm::regex_formatter( Format, Flags ) );
        }

        //! Replace regex algorithm
        /*!
            Search for a substring matching given regex and format it with 
            the specified format. The input string is modified in-place.

            \param Input An input string
            \param Rx A regular expression
            \param Format Regex format definition
            \param Flags Regex options
        */
        template< 
            typename SequenceT, 
            typename CharT, 
            typename RegexTraitsT,
            typename FormatStringTraitsT, typename FormatStringAllocatorT >
        inline void replace_regex( 
            SequenceT& Input,
            const basic_regex<CharT, RegexTraitsT>& Rx,
            const std::basic_string<CharT, FormatStringTraitsT, FormatStringAllocatorT>& Format,
            match_flag_type Flags=match_default | format_default )
        {
            ::lslboost::algorithm::find_format( 
                Input,
                ::lslboost::algorithm::regex_finder( Rx, Flags ),
                ::lslboost::algorithm::regex_formatter( Format, Flags ) );
        }

//  replace_all_regex --------------------------------------------------------------------//

        //! Replace all regex algorithm
        /*!
            Format all substrings, matching given regex, with the specified format. 
            The result is a modified copy of the input. It is returned as a sequence 
            or copied to the output iterator.

            \param Output An output iterator to which the result will be copied
            \param Input An input string
            \param Rx A regular expression
            \param Format Regex format definition
            \param Flags Regex options
            \return An output iterator pointing just after the last inserted character or
                    a modified copy of the input     

              \note The second variant of this function provides the strong exception-safety guarantee
        */
        template< 
            typename OutputIteratorT,
            typename RangeT, 
            typename CharT, 
            typename RegexTraitsT,
            typename FormatStringTraitsT, typename FormatStringAllocatorT >
        inline OutputIteratorT replace_all_regex_copy(
            OutputIteratorT Output,
            const RangeT& Input,
            const basic_regex<CharT, RegexTraitsT>& Rx,
            const std::basic_string<CharT, FormatStringTraitsT, FormatStringAllocatorT>& Format,
            match_flag_type Flags=match_default | format_default )
        {
            return ::lslboost::algorithm::find_format_all_copy( 
                Output,
                Input,
                ::lslboost::algorithm::regex_finder( Rx, Flags ),
                ::lslboost::algorithm::regex_formatter( Format, Flags ) );
        }

        //! Replace all regex algorithm
        /*!
            \overload
        */
        template< 
            typename SequenceT, 
            typename CharT, 
            typename RegexTraitsT,
            typename FormatStringTraitsT, typename FormatStringAllocatorT >
        inline SequenceT replace_all_regex_copy( 
            const SequenceT& Input,
            const basic_regex<CharT, RegexTraitsT>& Rx,
            const std::basic_string<CharT, FormatStringTraitsT, FormatStringAllocatorT>& Format,
            match_flag_type Flags=match_default | format_default )
        {
            return ::lslboost::algorithm::find_format_all_copy( 
                Input,
                ::lslboost::algorithm::regex_finder( Rx, Flags ),
                ::lslboost::algorithm::regex_formatter( Format, Flags ) );
        }

        //! Replace all regex algorithm
        /*!
            Format all substrings, matching given regex, with the specified format. 
            The input string is modified in-place.

            \param Input An input string
            \param Rx A regular expression
            \param Format Regex format definition
            \param Flags Regex options            
        */
        template< 
            typename SequenceT, 
            typename CharT, 
            typename RegexTraitsT,
            typename FormatStringTraitsT, typename FormatStringAllocatorT >
        inline void replace_all_regex( 
            SequenceT& Input,
            const basic_regex<CharT, RegexTraitsT>& Rx,
            const std::basic_string<CharT, FormatStringTraitsT, FormatStringAllocatorT>& Format,
            match_flag_type Flags=match_default | format_default )
        {
            ::lslboost::algorithm::find_format_all( 
                Input,
                ::lslboost::algorithm::regex_finder( Rx, Flags ),
                ::lslboost::algorithm::regex_formatter( Format, Flags ) );
        }

//  erase_regex --------------------------------------------------------------------//

        //! Erase regex algorithm
        /*!
            Remove a substring matching given regex from the input.
            The result is a modified copy of the input. It is returned as a sequence 
            or copied to the output iterator.                        

            \param Output An output iterator to which the result will be copied
            \param Input An input string
            \param Rx A regular expression
            \param Flags Regex options
            \return An output iterator pointing just after the last inserted character or
                       a modified copy of the input    

             \note The second variant of this function provides the strong exception-safety guarantee
       */
        template< 
            typename OutputIteratorT,
            typename RangeT, 
            typename CharT, 
            typename RegexTraitsT >
        inline OutputIteratorT erase_regex_copy(
            OutputIteratorT Output,
            const RangeT& Input,
            const basic_regex<CharT, RegexTraitsT>& Rx,
            match_flag_type Flags=match_default )
        {
            return ::lslboost::algorithm::find_format_copy(
                Output,
                Input,
                ::lslboost::algorithm::regex_finder( Rx, Flags ),
                ::lslboost::algorithm::empty_formatter( Input ) );
        }

        //! Erase regex algorithm
        /*!
            \overload
        */
        template< 
            typename SequenceT, 
            typename CharT, 
            typename RegexTraitsT >
        inline SequenceT erase_regex_copy( 
            const SequenceT& Input,
            const basic_regex<CharT, RegexTraitsT>& Rx,
            match_flag_type Flags=match_default )
        {
            return ::lslboost::algorithm::find_format_copy( 
                Input, 
                ::lslboost::algorithm::regex_finder( Rx, Flags ),
                ::lslboost::algorithm::empty_formatter( Input ) );
        }

        //! Erase regex algorithm
        /*!
            Remove a substring matching given regex from the input.
            The input string is modified in-place.

            \param Input An input string
            \param Rx A regular expression
            \param Flags Regex options
        */
        template< 
            typename SequenceT, 
            typename CharT, 
            typename RegexTraitsT >
        inline void erase_regex( 
            SequenceT& Input,
            const basic_regex<CharT, RegexTraitsT>& Rx,
            match_flag_type Flags=match_default )
        {
            ::lslboost::algorithm::find_format( 
                Input, 
                ::lslboost::algorithm::regex_finder( Rx, Flags ),
                ::lslboost::algorithm::empty_formatter( Input ) );
        }

//  erase_all_regex --------------------------------------------------------------------//

        //! Erase all regex algorithm
        /*!
            Erase all substrings, matching given regex, from the input.
            The result is a modified copy of the input. It is returned as a sequence 
            or copied to the output iterator.
            

            \param Output An output iterator to which the result will be copied
            \param Input An input string
            \param Rx A regular expression
            \param Flags Regex options
            \return An output iterator pointing just after the last inserted character or
                    a modified copy of the input                        

             \note The second variant of this function provides the strong exception-safety guarantee
        */
        template< 
            typename OutputIteratorT,
            typename RangeT, 
            typename CharT, 
            typename RegexTraitsT >
        inline OutputIteratorT erase_all_regex_copy(
            OutputIteratorT Output,
            const RangeT& Input,
            const basic_regex<CharT, RegexTraitsT>& Rx,
            match_flag_type Flags=match_default )
        {
            return ::lslboost::algorithm::find_format_all_copy(
                Output,
                Input,
                ::lslboost::algorithm::regex_finder( Rx, Flags ),
                ::lslboost::algorithm::empty_formatter( Input ) );
        }

        //! Erase all regex algorithm
        /*!
            \overload
        */
        template< 
            typename SequenceT, 
            typename CharT, 
            typename RegexTraitsT >
        inline SequenceT erase_all_regex_copy( 
            const SequenceT& Input,
            const basic_regex<CharT, RegexTraitsT>& Rx,
            match_flag_type Flags=match_default )
        {
            return ::lslboost::algorithm::find_format_all_copy( 
                Input, 
                ::lslboost::algorithm::regex_finder( Rx, Flags ),
                ::lslboost::algorithm::empty_formatter( Input ) );
        }

        //! Erase all regex algorithm
        /*!
            Erase all substrings, matching given regex, from the input.
            The input string is modified in-place.

            \param Input An input string
            \param Rx A regular expression
            \param Flags Regex options
        */
        template< 
            typename SequenceT, 
            typename CharT, 
            typename RegexTraitsT>
        inline void erase_all_regex( 
            SequenceT& Input,
            const basic_regex<CharT, RegexTraitsT>& Rx,
            match_flag_type Flags=match_default )
        {
            ::lslboost::algorithm::find_format_all( 
                Input, 
                ::lslboost::algorithm::regex_finder( Rx, Flags ),
                ::lslboost::algorithm::empty_formatter( Input ) );
        }

//  find_all_regex ------------------------------------------------------------------//

        //! Find all regex algorithm
        /*!
            This algorithm finds all substrings matching the give regex
            in the input.             
            
            Each part is copied and added as a new element to the output container.
            Thus the result container must be able to hold copies
            of the matches (in a compatible structure like std::string) or
            a reference to it (e.g. using the iterator range class).
            Examples of such a container are \c std::vector<std::string>
            or \c std::list<lslboost::iterator_range<std::string::iterator>>

            \param Result A container that can hold copies of references to the substrings.
            \param Input A container which will be searched.
            \param Rx A regular expression
            \param Flags Regex options
            \return A reference to the result

            \note Prior content of the result will be overwritten.

             \note This function provides the strong exception-safety guarantee
        */
        template< 
            typename SequenceSequenceT, 
            typename RangeT,         
            typename CharT, 
            typename RegexTraitsT >
        inline SequenceSequenceT& find_all_regex(
            SequenceSequenceT& Result,
            const RangeT& Input,
            const basic_regex<CharT, RegexTraitsT>& Rx,
            match_flag_type Flags=match_default )
        {
            return ::lslboost::algorithm::iter_find(
                Result,
                Input,
                ::lslboost::algorithm::regex_finder(Rx,Flags) );         
        }

//  split_regex ------------------------------------------------------------------//

        //! Split regex algorithm
        /*! 
            Tokenize expression. This function is equivalent to C strtok. Input
            sequence is split into tokens, separated  by separators. Separator
            is an every match of the given regex.
            Each part is copied and added as a new element to the output container.
            Thus the result container must be able to hold copies
            of the matches (in a compatible structure like std::string) or
            a reference to it (e.g. using the iterator range class).
            Examples of such a container are \c std::vector<std::string>
            or \c std::list<lslboost::iterator_range<std::string::iterator>>
    
            \param Result A container that can hold copies of references to the substrings.          
            \param Input A container which will be searched.
            \param Rx A regular expression
            \param Flags Regex options
            \return A reference to the result

            \note Prior content of the result will be overwritten.

               \note This function provides the strong exception-safety guarantee
        */
        template< 
            typename SequenceSequenceT, 
            typename RangeT,         
            typename CharT, 
            typename RegexTraitsT >
        inline SequenceSequenceT& split_regex(
            SequenceSequenceT& Result,
            const RangeT& Input,
            const basic_regex<CharT, RegexTraitsT>& Rx,
            match_flag_type Flags=match_default )
        {
            return ::lslboost::algorithm::iter_split(
                Result,
                Input,
                ::lslboost::algorithm::regex_finder(Rx,Flags) );         
        }

//  join_if ------------------------------------------------------------------//

#ifndef BOOST_NO_FUNCTION_TEMPLATE_ORDERING

        //! Conditional join algorithm
        /*!
            This algorithm joins all strings in a 'list' into one long string.
            Segments are concatenated by given separator. Only segments that
            match the given regular expression will be added to the result

            This is a specialization of join_if algorithm.

            \param Input A container that holds the input strings. It must be a container-of-containers.
            \param Separator A string that will separate the joined segments.
            \param Rx A regular expression
            \param Flags Regex options
            \return Concatenated string.

            \note This function provides the strong exception-safety guarantee
        */
        template< 
            typename SequenceSequenceT, 
            typename Range1T,             
            typename CharT, 
            typename RegexTraitsT >
        inline typename range_value<SequenceSequenceT>::type 
        join_if(
            const SequenceSequenceT& Input,
            const Range1T& Separator,
            const basic_regex<CharT, RegexTraitsT>& Rx,
            match_flag_type Flags=match_default )
        {
            // Define working types
            typedef typename range_value<SequenceSequenceT>::type ResultT;
            typedef typename range_const_iterator<SequenceSequenceT>::type InputIteratorT;

            // Parse input
            InputIteratorT itBegin=::lslboost::begin(Input);
            InputIteratorT itEnd=::lslboost::end(Input);

            // Construct container to hold the result
            ResultT Result;


            // Roll to the first element that will be added
            while(
                itBegin!=itEnd && 
                !::lslboost::regex_match(::lslboost::begin(*itBegin), ::lslboost::end(*itBegin), Rx, Flags)) ++itBegin;

            // Add this element
            if(itBegin!=itEnd)
            {
                detail::insert(Result, ::lslboost::end(Result), *itBegin);
                ++itBegin;
            }

            for(;itBegin!=itEnd; ++itBegin)
            {
                if(::lslboost::regex_match(::lslboost::begin(*itBegin), ::lslboost::end(*itBegin), Rx, Flags))
                {
                    // Add separator
                    detail::insert(Result, ::lslboost::end(Result), ::lslboost::as_literal(Separator));
                    // Add element
                    detail::insert(Result, ::lslboost::end(Result), *itBegin);
                }
            }

            return Result;
        }

#else  // BOOST_NO_FUNCTION_TEMPLATE_ORDERING

                //! Conditional join algorithm
        /*!
            This algorithm joins all strings in a 'list' into one long string.
            Segments are concatenated by given separator. Only segments that
            match the given regular expression will be added to the result

            This is a specialization of join_if algorithm.

            \param Input A container that holds the input strings. It must be a container-of-containers.
            \param Separator A string that will separate the joined segments.
            \param Rx A regular expression
            \param Flags Regex options
            \return Concatenated string.

            \note This function provides the strong exception-safety guarantee
        */
        template< 
            typename SequenceSequenceT, 
            typename Range1T,             
            typename CharT, 
            typename RegexTraitsT >
        inline typename range_value<SequenceSequenceT>::type 
        join_if_regex(
            const SequenceSequenceT& Input,
            const Range1T& Separator,
            const basic_regex<CharT, RegexTraitsT>& Rx,
            match_flag_type Flags=match_default )
        {
            // Define working types
            typedef typename range_value<SequenceSequenceT>::type ResultT;
            typedef typename range_const_iterator<SequenceSequenceT>::type InputIteratorT;

            // Parse input
            InputIteratorT itBegin=::lslboost::begin(Input);
            InputIteratorT itEnd=::lslboost::end(Input);

            // Construct container to hold the result
            ResultT Result;


            // Roll to the first element that will be added
            while(
                itBegin!=itEnd && 
                !::lslboost::regex_match(::lslboost::begin(*itBegin), ::lslboost::end(*itBegin), Rx, Flags)) ++itBegin;

            // Add this element
            if(itBegin!=itEnd)
            {
                detail::insert(Result, ::lslboost::end(Result), *itBegin);
                ++itBegin;
            }

            for(;itBegin!=itEnd; ++itBegin)
            {
                if(::lslboost::regex_match(::lslboost::begin(*itBegin), ::lslboost::end(*itBegin), Rx, Flags))
                {
                    // Add separator
                    detail::insert(Result, ::lslboost::end(Result), ::lslboost::as_literal(Separator));
                    // Add element
                    detail::insert(Result, ::lslboost::end(Result), *itBegin);
                }
            }

            return Result;
        }


#endif // BOOST_NO_FUNCTION_TEMPLATE_ORDERING

    } // namespace algorithm

    // pull names into the lslboost namespace
    using algorithm::find_regex;
    using algorithm::replace_regex;
    using algorithm::replace_regex_copy;
    using algorithm::replace_all_regex;
    using algorithm::replace_all_regex_copy;
    using algorithm::erase_regex;
    using algorithm::erase_regex_copy;
    using algorithm::erase_all_regex;
    using algorithm::erase_all_regex_copy;
    using algorithm::find_all_regex;
    using algorithm::split_regex;

#ifndef BOOST_NO_FUNCTION_TEMPLATE_ORDERING
    using algorithm::join_if;
#else  // BOOST_NO_FUNCTION_TEMPLATE_ORDERING
    using algorithm::join_if_regex;
#endif // BOOST_NO_FUNCTION_TEMPLATE_ORDERING

} // namespace lslboost


#endif  // BOOST_STRING_REGEX_HPP
